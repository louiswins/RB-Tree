#ifndef RBTREE_H
#define RBTREE_H

#define RBREADFILE "RBinput.txt"

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
/* Reads a tree from file RBREADFILE.
 * DOES NOT CHECK to see if this tree is formatted correctly
 * with the blacks and the reds. */
rb_tree RBread();

#endif /* RBTREE_H */
