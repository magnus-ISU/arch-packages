#include <stdlib.h>

#include "tree.h"
#include "helpers.h"

Binary_Tree_T *new_binary_tree(void *data) {
    Binary_Tree_T *t;
    t = safe_malloc(sizeof(Binary_Tree_T));
    t->data = data;
    t->left = NULL;
    t->right = NULL;
    return t;
}

void cleanup_binary_tree(Binary_Tree_T *t) {
    if (t->left != NULL) {
        cleanup_binary_tree((Binary_Tree_T *) t->left);
    }
    if (t->right != NULL) {
        cleanup_binary_tree((Binary_Tree_T *) t->right);
    }
    free(t->data);
    free(t);
}
