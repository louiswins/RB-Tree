#ifndef RBTREE_H
#define RBTREE_H

typedef struct rb_tree *rb_tree;

/* Returns an empty tree */
rb_tree rb_create();
/* Inserts the value data into the tree */
void rb_insert(rb_tree *root, int data);

#endif /* RBTREE_H */
