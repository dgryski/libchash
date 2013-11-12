/* chash.h */
#ifndef _CHASH_H_
#define _CHASH_H_

struct chash_t;

struct chash_t *chash_create(const char **node_names, size_t * name_lens,
			     size_t num_names, size_t replicas);

void chash_lookup(struct chash_t *chash, const char *key, size_t len,
		  const char **node_name, size_t * name_len);

void chash_free(struct chash_t *chash);

#endif /* _CHASH_H_ */
