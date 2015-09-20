/* chash.h */
#ifndef DGRYSKI_LIBCHASH_CHASH_H
#define DGRYSKI_LIBCHASH_CHASH_H

struct chash_t;

struct chash_t *chash_create(const char **node_names, size_t *name_lens,
			     size_t num_names, size_t replicas);

void chash_lookup(struct chash_t *chash, const char *key, size_t len,
		  const char **node_name, size_t *name_len);

void chash_free(struct chash_t *chash);

#endif /* DGRYSKI_LIBCHASH_CHASH_H */
