/* chash.h */

struct chash_t;

/* node names are required to be null-terminated strings */
struct chash_t *chash_create(const char **node_names, size_t num_names,
			     size_t replicas);

/* returned node name will always be a null-terminated string */
const char *chash_lookup(struct chash_t *chash, const char *key, size_t len);

void chash_free(struct chash_t *chash);
