/* chash.h */

struct chash_t;

struct chash_t *chash_create(char **keys, int nkeys, int replicas);
char *chash_lookup(struct chash_t *chash, char *key, int len);
void chash_free(struct chash_t *chash);
