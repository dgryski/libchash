#include <stdio.h>
#include <string.h>

#include "chash.h"

int main()
{

    char *keys[] =
	{ "server1", "server2", "server3", "server4", "server5" };

    int servers[5];

    for (int i = 0; i < 5; i++) {
	servers[i] = 0;
    }

    struct chash_t *chash = chash_create(keys, 5, 160);

    char line[100];
    for (int i = 0; i < 100000; i++) {
	int l = snprintf(line, sizeof(line), "foo%d\n", i);
	char *k = chash_lookup(chash, line, l);
	int b = k[6] - '1';
	servers[b]++;
    }

    int expected[] = {
	19236,
	21802,
	21468,
	17602,
	19892,
    };

    for (int i = 0; i < 5; i++) {
	printf("server%d=%d\n", i + 1, servers[i]);
	if (expected[i] != servers[i]) {
	    printf("FAIL: expected=%d got=%d\n", expected[i], servers[i]);
	}
    }

    chash_free(chash);

    return 0;
}
