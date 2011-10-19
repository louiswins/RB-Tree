#ifndef RBTREE_H
#define RBTREE_H

#define RBREADFILE "RBinput.txt"

typedef struct rb_tree *rb_tree;

/* Creates an empty Red-Black tree. */
rb_tree RBcreate();
/* Frees an entire tree. */
void RBdestroy(rb_tree tree);
/* Cleans up. Call this when you won't be using any more Red-Black trees. */
void RBcleanup();

/* Inserts an element with specified key into tree. */
int RBinsert(rb_tree tree, int key);

/* Deletes an element with a particular key. */
int RBdelete(rb_tree tree, int key);

/* Writes a tree to stdout in preorder format. */
void RBwrite(rb_tree tree);
/* Reads a tree in preorder format from file RBREADFILE.
 * DOES NOT CHECK to see if this tree is formatted correctly to preserve
 * red-black properties. */
rb_tree RBread();

void RBdraw(rb_tree tree, char *fname);

#endif /* RBTREE_H */
