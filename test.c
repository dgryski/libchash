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

    while (fgets(line, sizeof(line), stdin)) {
	int l = strlen(line);
	line[l - 1] = 0;
	char *k = chash_lookup(chash, line, l - 1);
	int b = k[6] - '1';
	servers[b]++;
    }

    for (int i = 0; i < 5; i++) {
	printf("servers%d=%d\n", i + 1, servers[i]);
    }

    chash_free(chash);
}
