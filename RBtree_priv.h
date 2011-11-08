#ifndef RBTREE_PRIV_H
#define RBTREE_PRIV_H

#include <stdio.h>
#define RADIUS 15
#define PADDING 10
#define MAXWIDTH 1000

typedef struct rb_node {
	int key;
	struct rb_node *parent;
	struct rb_node *lchild,
		       *rchild;
	char color;
} *rb_node;
struct rb_tree {
	rb_node root;
	rb_node nil;
};

/* Our pool of nodes for faster allocation */
static rb_node rb_mem_pool = NULL;


/* Helper routine: frees a subtree rooted at specified node. */
static void rb_delete_subtree(rb_tree tree, rb_node node);
/* Creates a new node, taking from the memory pool if available. */
static rb_node rb_new_node(rb_tree tree, int data);
/* Frees a node to the memory pool. */
static void rb_free_node(rb_node node);

/* Corrects for properties violated on a single insertion. */
static void rb_insert_fix(rb_tree tree, rb_node n);
/* Helper routine: returns the uncle of a given node. */
static rb_node rb_get_uncle(rb_tree tree, rb_node n);

/* Helper routine: transplants node from into node to's position. */
static void rb_transplant(rb_tree tree, rb_node to, rb_node from);
/* Fixes possible violations from node deletion. */
static void rb_delete_fix(rb_tree tree, rb_node n);

/* Helper routine: write an entire subtree to stdout. */
static void rb_preorder_write(rb_tree tree, rb_node n);
/* Helper routine: read a single node from file fp. */
static rb_node rb_read_node(rb_tree tree, FILE *fp);
/* Reads a tree in preorder format, limited by the maximum value of max. */
static rb_node rb_read_subtree(rb_tree tree, rb_node *next, int max, FILE *fp);

/* Returns a node with the given key. */
static rb_node rb_get_node_by_key(rb_tree haystack, int needle);
/* Rotates a tree around the given root. */
static void rb_rotate(rb_tree tree, rb_node root, int go_left);
/* Returns minimum node in the given subtree. */
static rb_node rb_min(rb_tree tree, rb_node node);

/* Draws a subtree rooted at a given node between the x-coordinates l and r,
 * starting at height h. */
static void rb_draw_subtree(FILE *fp, rb_tree tree, rb_node n, int l,
		int r, int h);
/* Computes height of a tree. */
static int rb_height(rb_tree tree);
/* Computes height of a subtree. */
static int rb_subheight(rb_tree tree, rb_node n);
/* Computes 2^h, twice the maximum width of a tree of height h. */
static int pow2(int h);

#endif /* RBTREE_PRIV_H */
