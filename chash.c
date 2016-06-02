/* chash.c */
/* This library implements a consistent hash ring */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "chash.h"

struct bucket_t {
    uint32_t node_idx;
    uint32_t point;
} bucket_t;

struct chash_t {
    struct bucket_t *blist;
    size_t nbuckets;
    char **node_names;
    size_t *name_lens;
    size_t num_names;
} chash_t;

static int cmpbucket(const void *a, const void *b)
{
    const struct bucket_t *b1 = (const struct bucket_t *) a;
    const struct bucket_t *b2 = (const struct bucket_t *) b;

    if (b1->point < b2->point) {
	return -1;
    }

    if (b1->point > b2->point) {
	return 1;
    }

    return 0;
}

/* from leveldb, a murmur-lite */
static uint32_t leveldb_bloom_hash(const unsigned char *b, size_t len)
{
    const uint32_t seed = 0xbc9f1d34;
    const uint32_t m = 0xc6a4a793;

    uint32_t h = seed ^ len * m;
    while (len >= 4) {
	h += b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
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

struct chash_t *chash_create(const char **node_names, size_t *name_lens,
			     size_t num_names, size_t replicas)
{
    struct chash_t *chash;
    struct bucket_t *blist;
    char **nlist;
    size_t *lens;
    size_t i, n, r, len, len1, len2, bidx = 0;
    char buffer[256];

    blist = (struct bucket_t *) malloc(sizeof(bucket_t) * num_names * replicas);
    if (blist == NULL) {
	goto ERROR_BLIST;
    }
    nlist = (char **) malloc(sizeof(char *) * num_names);
    if (nlist == NULL) {
	goto ERROR_NLIST;
    }
    lens = (size_t *) malloc(sizeof(size_t) * num_names);
    if (lens == NULL) {
	goto ERROR_LENS;
    }

    for (n = 0; n < num_names; n++) {
	nlist[n] = (char *) malloc(sizeof(char) * name_lens[n]);
	if (nlist[n] == NULL) {
	    goto ERROR_NLIST_MEMBER;
	}
	lens[n] = name_lens[n];
	memcpy(nlist[n], node_names[n], lens[n]);
	for (r = 0; r < replicas; r++) {
	    blist[bidx].node_idx = n;
	    len1 = snprintf(buffer, sizeof(buffer), "%lu", (unsigned long) r);
	    if ((sizeof(buffer) - len1) < name_lens[n]) {
		len2 = sizeof(buffer) - len1;
	    } else {
		len2 = name_lens[n];
	    }
	    memcpy(buffer + len1, node_names[n], len2);
	    len = len1 + len2;
	    buffer[len] = '\0';
	    if (len >= 255) {
		fprintf(stderr, "Node name truncated to: %s\n", buffer);
	    }
	    blist[bidx].point =
		leveldb_bloom_hash((const unsigned char *) buffer, len);
	    bidx++;
	}
    }

    qsort(blist, bidx, sizeof(struct bucket_t), cmpbucket);

    chash = malloc(sizeof(chash_t));
    if (chash == NULL) {
	goto ERROR_CHASH;
    }
    chash->blist = blist;
    chash->nbuckets = bidx;
    chash->node_names = nlist;
    chash->name_lens = lens;
    chash->num_names = num_names;

    return chash;

  ERROR_CHASH:
  ERROR_NLIST_MEMBER:
    /* We know that nlist[n] is the first member not allocated,
       thus free every nlist[i] up to n. */
    for (i = 0; i < n; i++) {
	free(nlist[n]);
    }
    free(lens);
  ERROR_LENS:
    free(nlist);
  ERROR_NLIST:
    free(blist);
  ERROR_BLIST:
    return NULL;
}

void chash_lookup(struct chash_t *chash, const char *key, size_t len,
		  const char **node_name, size_t *name_len)
{
    uint32_t point = leveldb_bloom_hash((const unsigned char *) key, len);

    uint32_t low = 0, high = chash->nbuckets;
    uint32_t node_idx;

    /* binary search through blist */
    while (low < high) {
	uint32_t mid = low + (high - low) / 2;
	if (chash->blist[mid].point > point) {
	    high = mid;
	} else {
	    low = mid + 1;
	}
    }

    if (low >= chash->nbuckets) {
	low = 0;
    }

    node_idx = chash->blist[low].node_idx;
    *node_name = chash->node_names[node_idx];
    *name_len = chash->name_lens[node_idx];
}

void chash_free(struct chash_t *chash)
{
    size_t i;

    for (i = 0; i < chash->num_names; i++) {
	free(chash->node_names[i]);
    }
    free(chash->node_names);
    free(chash->name_lens);
    free(chash->blist);
    free(chash);
}
