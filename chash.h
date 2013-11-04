/* chash.h */

struct chash_t;

struct chash_t *chash_create(char **keys, size_t nkeys, int replicas);
char *chash_lookup(struct chash_t *chash, char *key, size_t len);
void chash_free(struct chash_t *chash);
