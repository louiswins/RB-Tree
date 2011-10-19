#ifndef RBTREE_H
#define RBTREE_H

typedef struct rb_tree *rb_tree;

/* Returns an empty tree */
rb_tree rb_create();
/* Inserts the value data into the tree */
int RBinsert(rb_tree tree, int data);
/* Finds a certain element */
int rb_node_exists(rb_tree haystack, int needle);
/* Deletes an element with a particular key */
int RBdelete(rb_tree tree, int key);
/* Writes a tree in preorder format to stdout */
void RBwrite(rb_tree tree);

#endif /* RBTREE_H */
