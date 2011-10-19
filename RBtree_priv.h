#ifndef RBTREE_PRIV_H
#define RBTREE_PRIV_H

#include <stdlib.h>

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

/* Our heap of nodes for faster allocation */
static rb_node rb_mem_pool = NULL;


static void rb_delete_subtree(rb_tree tree, rb_node node);
static rb_node rb_new_node(rb_tree tree, int data);
static void rb_free_node(rb_node node);

static rb_node rb_unsafe_insert(rb_tree tree, rb_node n);
static void rb_insert_fix(rb_tree tree, rb_node n);
static rb_node rb_get_uncle(rb_tree tree, rb_node n);

static void rb_transplant(rb_tree tree, rb_node to, rb_node from);
static void rb_delete_fix(rb_tree tree, rb_node n);

static void rb_preorder_write(rb_tree tree, rb_node n);

static rb_node rb_get_node_by_key(rb_tree haystack, int needle);
static void rb_rotate(rb_tree tree, rb_node root, int go_left);
static rb_node rb_min(rb_tree tree, rb_node node);

#endif /* RBTREE_PRIV_H */
