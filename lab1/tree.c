#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} typedef Node;

Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));

    if (!node) {
        printf("Memory Error");
        return NULL;
    }

    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

Node* findMin(Node* root) {
    if (!root) {
        printf("Empty Tree");
        return NULL;
    }

    Node* current = root;
    while (current->left) {
        current = current->left;
    }
    return current;
}

void deleteNode(Node** root, int data) {
    if (*root == NULL) {
        printf("Element not found!\n");
        return;
    }

    if (data < (*root)->data) {
        deleteNode(&(*root)->left, data);
    }
    else if (data > (*root)->data) {
        deleteNode(&(*root)->right, data);
    }
    else {
        Node* temp = NULL;

        if ((*root)->left == NULL) {
            temp = *root;
            *root = (*root)->right;
            free(temp);
        }

        else if ((*root)->right == NULL) {
            temp = *root;
            *root = (*root)->left;
            free(temp);
        }

        else {
            Node* minParent = *root;
            Node* minNode = (*root)->right;

            while (minNode->left != NULL) {
                minParent = minNode;
                minNode = minNode->left;
            }

            (*root)->data = minNode->data;

            if (minParent->left == minNode) {
                minParent->left = minNode->right;
            } else {
                minParent->right = minNode->right;
            }

            free(minNode);
        }
    }
}

void insert(Node** root, int data) {
    if (*root == NULL) {
        *root = createNode(data);
        return;
    }

    if (data <= (*root)->data) {
        insert(&(*root)->left, data);
    }
    else {
        insert(&(*root)->right, data);
    }
}

void printTree(Node* root, int depth) {
    if (!root) return;

    for (int i = 0; i < depth; i++) printf("  ");
    printf("%d\n", root->data);

    printTree(root->left, depth + 1);
    printTree(root->right, depth + 1);
}

bool isMirrorStruct(Node* node_left, Node* node_right) {
    if (node_left == NULL && node_right == NULL) return true;
    if (node_left == NULL || node_right == NULL) return false;

    return isMirrorStruct(node_left->left, node_right->right) &&
           isMirrorStruct(node_left->right, node_right->left);
}

bool isSymmetricStruct(Node* root) {
    if (root == NULL) return true;
    return isMirrorStruct(root->left, root->right);
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}