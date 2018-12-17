#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"

struct node {
    size_t key;
    void *value;
    struct node *left;
    struct node *right;
};

struct hashmap *hash_init() {
    struct hashmap *map = calloc(1, sizeof(struct hashmap));
    return map;
}

void free_node(struct node *node) {
    if (node == NULL)
        return;

    free_node(node->left);
    free_node(node->right);
    free(node);
}

static struct node *insert_node(struct node *tree, struct node *n) {
    if (n == NULL) {
        return NULL;
    }
    if (tree == NULL) {
        return n;
    }
    if (n->key < tree->key) {
        tree->left = insert_node(tree->left, n);
        return tree;
    }
    if (n->key > tree->key) {
        tree->right = insert_node(tree->right, n);
        return tree;
    }
    return tree;
}

void hash_free(struct hashmap *map) {
    free_node(map->root);
    free(map);
}

void hash_put(struct hashmap *map, size_t key, void *val) {
    struct node *n = calloc(1, sizeof(struct node));
    n->key = key;
    n->value = val;
    map->root = insert_node(map->root, n);
}

void *node_get(struct node *node, size_t key) {
    if (node == NULL) {
        return NULL;
    }
    if (key == node->key) {
        return node->value;
    }
    if (key < node->key) {
        return node_get(node->left, key);
    }
    return node_get(node->right, key);
}

void *hash_get(struct hashmap *map, size_t key) {
    return node_get(map->root, key);
}

size_t node_count(struct node* node) {
    if(node == NULL)
        return 0;
    return 1 + node_count(node->left) + node_count(node->right);
}

size_t hash_count(struct hashmap* map) {
    return node_count(map->root);
}

void node_flat(struct node* node, void** ptr, size_t* index, size_t element_size) {
    if(node == NULL)
        return;
    ptr[*index] = &node->value;
    *index += element_size;
    node_flat(node->left, ptr, index, element_size);
    node_flat(node->right, ptr, index, element_size);
}

void** hash_flat(struct hashmap* map, size_t element_size, size_t* count) {
    *count = hash_count(map);
    void** result = malloc((*count)*element_size);
    void** ptr = result;
    size_t index = 0x0;
    node_flat(map->root, ptr, &index, element_size);
    return result;
}