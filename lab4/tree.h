#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#define BTREE_T 2
#define BTREE_MAX_KEYS (2 * BTREE_T - 1)
#define BTREE_MAX_CHILDREN (2 * BTREE_T)
#define BTREE_KEY_SIZE 7

typedef struct BTreeNode {
    int key_count;
    int leaf;
    char keys[BTREE_MAX_KEYS][BTREE_KEY_SIZE];
    double values[BTREE_MAX_KEYS];
    struct BTreeNode* children[BTREE_MAX_CHILDREN];
} BTreeNode;

typedef struct BTree {
    BTreeNode* root;
} BTree;

typedef enum BTreeStatus {
    BTREE_OK,
    BTREE_INVALID_KEY,
    BTREE_DUPLICATE_KEY,
    BTREE_KEY_NOT_FOUND,
    BTREE_MEMORY_ERROR,
    BTREE_FILE_ERROR,
    BTREE_INVALID_COMMAND
} BTreeStatus;

void init_tree(BTree* tree);
BTreeNode* create_node(int leaf);
void clear_tree(BTree* tree);
void free_tree(BTreeNode* node);

int is_valid_key(const char* key);
int tree_height(BTreeNode* node);
int count_keys(BTreeNode* node);

BTreeStatus insert_tree(BTree* tree, const char* key, double value);
BTreeStatus delete_tree(BTree* tree, const char* key);
BTreeStatus search_tree(BTreeNode* node, const char* key, double* value_out);

void print_tree(BTreeNode* node, int depth, FILE* out);

BTreeStatus process_command_line(BTree* tree, const char* line, FILE* out);
BTreeStatus process_commands_from_file(BTree* tree, const char* input_filename, const char* output_filename);

#endif
