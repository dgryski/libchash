#include <stdio.h>
#include <string.h>

#include "chash.h"

int main()
{
    const char *node_names[] =
	{ "server1", "server2", "server3", "server4", "server5" };
    size_t lens[5];

    int expected[] = {
	19236,
	21802,
	21468,
	17602,
	19892,
    };

    int servers[5];
    int i, l, n_idx;
    const char *key;
    size_t key_len;
    struct chash_t *chash;
    char line[100];
    unsigned int warns = 0;

    for (i = 0; i < 5; i++) {
	lens[i] = strlen(node_names[i]);
	servers[i] = 0;
    }

    chash = chash_create(node_names, lens, 5, 160);

    for (i = 0; i < 100000; i++) {
	l = snprintf(line, sizeof(line), "foo%d\n", i);
	chash_lookup(chash, line, l, &key, &key_len);
	n_idx = key[6] - '1';
	if ((key_len != lens[n_idx]) && (warns++ < 10)) {
	    printf("WARN: expected=%lu got=%lu\n", lens[n_idx], key_len);
	}
	servers[n_idx]++;
    }

    for (i = 0; i < 5; i++) {
	printf("server%d=%d\n", i + 1, servers[i]);
	if (expected[i] != servers[i]) {
	    printf("FAIL: expected=%d got=%d\n", expected[i], servers[i]);
	}
    }

    chash_free(chash);

    return 0;
}
