/* chash.c */
/* This library implements a consistent hash ring */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "chash.h"

struct bucket_t {
    char *key;
    unsigned int point;
} bucket_t;

struct chash_t {
    struct bucket_t *blist;
    int nbuckets;
    char **keys;
    int nkeys;
} chash_t;

static int cmpbucket(const void *a, const void *b)
{
    struct bucket_t *b1 = (struct bucket_t *) a;
    struct bucket_t *b2 = (struct bucket_t *) b;

    if (b1->point < b2->point) {
	return -1;
    }

    if (b1->point > b2->point) {
	return 1;
    }

    return 0;
}


/* from leveldb, a murmur-lite */
static uint32_t leveldb_bloom_hash(char *b, size_t len)
{

    const uint seed = 0xbc9f1d34;
    const uint m = 0xc6a4a793;

    int h = seed ^ len * m;
    while (len >= 4) {
	/* TODO(dgryski): fix endianness */
	h += *(uint32_t *) b;
	h *= m;
	h ^= h >> 16;
	b += 4;
	len -= 4;
    }

    switch (len) {
    case 3:
	h += b[2] << 16;
    case 2:
	h += b[1] << 8;
    case 1:
	h += b[0];
	h *= m;
	h ^= h >> 24;
    }
    return h;
}

struct chash_t *chash_create(char **keys, int nkeys, int replicas)
{

    struct chash_t *chash;

    struct bucket_t *blist =
	(struct bucket_t *) malloc(sizeof(bucket_t) * nkeys * replicas);
    char **klist = (char **) malloc(sizeof(char *) * nkeys);
    int bidx = 0;

    char buffer[256];

    for (int k = 0; k < nkeys; k++) {
	klist[k] = keys[k];
	for (int r = 0; r < replicas; r++) {
	    blist[bidx].key = keys[k];
	    int len = snprintf(buffer, sizeof(buffer), "%d%s", r, keys[k]);
	    /* TODO(dgryski): complain if keys[k] is too large */
	    blist[bidx].point = leveldb_bloom_hash(buffer, len);
	    bidx++;
	}
    }

    qsort(blist, bidx, sizeof(struct bucket_t), cmpbucket);

    chash = malloc(sizeof(chash_t));
    chash->blist = blist;
    chash->nbuckets = bidx;
    chash->keys = klist;
    chash->nkeys = nkeys;

    return chash;
}

char *chash_lookup(struct chash_t *chash, char *key, int len)
{

    struct bucket_t *b = chash->blist;
    struct bucket_t *end = chash->blist + chash->nbuckets;

    int point = leveldb_bloom_hash(key, len);

    /* linear search, we expect at most a couple hundred entries */
    /* branch-prediction will make this fast */
    while (b < end && b->point < point) {
	b++;
    }

    if (b == end) {
	return chash->blist[0].key;
    }

    return b->key;
}

void chash_free(struct chash_t *chash)
{
    for (int i = 0; i < chash->nkeys; i++) {
	free(chash->keys[i]);
    }
    free(chash->keys);
    free(chash->blist);
    free(chash);
}
