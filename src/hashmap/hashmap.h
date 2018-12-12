#ifndef HASHMAP_H
#define HASHMAP_H

struct hashmap {
  struct node* root;
};

/* initialize a hashmap */
struct hashmap* hash_init();

/* free a hashmap and all the stored values */
void hash_free(struct hashmap* map);

/* insert the couple (key, value) */
void hash_put(struct hashmap* map, size_t key, void* val);

/* return the value associated with key */
void* hash_get(struct hashmap* map, size_t key);

#endif /* HASHMAP_H */
