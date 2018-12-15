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
