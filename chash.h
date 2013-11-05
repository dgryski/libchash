/* chash.h */

struct chash_t;

struct chash_t *chash_create(const char **keys, size_t nkeys, size_t replicas);
const char *chash_lookup(struct chash_t *chash, const char *key, size_t len);
void chash_free(struct chash_t *chash);
