#include <stdio.h>
#include <string.h>

#include "chash.h"

int main()
{
    const char *keys[] =
	{ "server1", "server2", "server3", "server4", "server5" };
    size_t lens[] = { 8, 8, 8, 8, 8 };

    int expected[] = {
	19236,
	21802,
	21468,
	17602,
	19892,
    };

    int servers[5];
    int i, l, b;
    const char *k;
    size_t k_len;
    struct chash_t *chash;
    char line[100];
    unsigned int warns = 0;

    for (i = 0; i < 5; i++) {
	servers[i] = 0;
    }

    chash = chash_create(keys, lens, 5, 160);

    for (i = 0; i < 100000; i++) {
	l = snprintf(line, sizeof(line), "foo%d\n", i);
	chash_lookup(chash, line, l, &k, &k_len);
	b = k[6] - '1';
	if ((k_len != lens[b]) && (warns++ < 10)) {
	    printf("WARN: expected=%u got=%u\n", lens[b], k_len);
	}
	servers[b]++;
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
