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

/* Count the number of pairs inside this map */
size_t hash_count(struct hashmap* map);

/* flatten the map to an array of elements, order not guaranteed
 * 'count' is used to store the number of pairs inside the map
 * */
void** hash_flat(struct hashmap* map, size_t element_size, size_t* count);

#endif /* HASHMAP_H */
