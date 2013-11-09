/* chash.h */

struct chash_t;

/* keys are required to be null-terminated strings */
struct chash_t *chash_create(const char **keys, size_t nkeys, size_t replicas);

/* returned key will always be a null-terminated string */
const char *chash_lookup(struct chash_t *chash, const char *key, size_t len);

void chash_free(struct chash_t *chash);
