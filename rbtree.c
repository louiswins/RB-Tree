#include "rbtree.h"
#include <stdlib.h>
#ifdef DEBUG
#	include <stdio.h>
#	define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#	define eprintf(...) ((void)0)
#endif

typedef struct rb_node {
	int key;
	struct rb_node *parent;
	struct rb_node *left,
		       *right;
	enum { RED, BLACK } color;
} *rb_node;
struct rb_tree {
	rb_node root;
	rb_node nil;
};

static rb_node rb_new_node(rb_tree tree, int data);
static void fix(rb_tree root, rb_node n);
static void rotate(rb_tree tree, rb_node root, int go_left);
#ifdef DEBUG
static char col(rb_node node) {
	return (node->color==BLACK) ? 'b' : 'r';
}
#endif



rb_tree rb_create() {
	rb_tree ret;
	if ((ret = malloc(sizeof(*ret))) == NULL) {
		return NULL;
	}
	/* We can't use rb_new_node() because it wants to set some of the values
	 * to tree->nil. We don't care what the values are except for color, so
	 * leave them at undefined. */
	ret->nil = malloc(sizeof(*ret->nil));
	ret->nil->color = BLACK;
	ret->root = ret->nil;
	return ret;
}

void rb_insert(rb_tree tree, int data) {
	rb_node newparent = tree->nil;
	rb_node root = tree->root;
	rb_node newnode = rb_new_node(tree, data);
	while (root != tree->nil) {
		newparent = root;
		if (data < root->key) {
			root = root->left;
		} else {
			root = root->right;
		}
	}
	eprintf("> Inserting node %d(%c) below %d(%c)\n", newnode->key,
			col(newnode), newparent->key, col(newparent));
	newnode->parent = newparent;
	/* If we inserted a new root into the tree */
	if (newparent == tree->nil) {
		tree->root = newnode;
	}
	if (data < newparent->key) {
		newparent->left = newnode;
	} else {
		newparent->right = newnode;
	}
	fix(tree, newnode);
}

int node_exists(rb_tree haystack, int needle) {
	rb_node root = haystack->root;
	while (root != haystack->nil) {
		eprintf(">> Passing through %d(%c)\n", root->key,
				col(root));
		if (root->key == needle) {
			return 1;
		} else if (needle < root->key) {
			root = root->left;
		} else {
			root = root->right;
		}
	}
	return 0;
}



static rb_node rb_new_node(rb_tree tree, int data) {
	rb_node ret;
	if ((ret = malloc(sizeof(*ret))) == NULL)
		return NULL;
	ret->key = data;
	ret->parent = tree->nil;
	ret->left = tree->nil;
	ret->right = tree->nil;
	ret->color = RED;
	return ret;
}
static rb_node get_uncle(rb_tree tree, rb_node n) {
	rb_node gp;
	if (n->parent == tree->nil || n->parent->parent == tree->nil) {
		return tree->nil;
	}
	gp = n->parent->parent;
	return (gp->left == n->parent) ? gp->right : gp->left;
}
static void fix(rb_tree tree, rb_node n) {
	rb_node gp = n->parent->parent,
		uncle = get_uncle(tree, n);

	/* Case 1: uncle is colored red */
	while (n->parent->color == RED && uncle->color == RED) {
		/* If gp were null, then n->parent would be the root node (or
		 * tree->nil), and would have to have been black. */
		eprintf(">> Case 1: %d(%c), with uncle %d(%c)\n", n->key,
				col(n), uncle->key, col(uncle));
		gp->color = RED;
		uncle->color = BLACK;
		n->parent->color = BLACK;
		n = gp;
		gp = n->parent->parent;
		uncle = get_uncle(tree, n);
	}
	
	eprintf(">> Case 1 taken care of, on %d(%c) with parent %d(%c)\n",
			n->key, col(n), n->parent->key, col(n->parent));
	if (n->parent->color == BLACK) {
		if (n == tree->root) n->color = BLACK;
		return;
	}

	/* Case 2: node is "close" to uncle */
	if ((n->parent->left == n) == (gp->left == uncle)) {
		rb_node newroot = n->parent;
		eprintf(">> Case 2: %d(%c), with uncle %d(%c)\n", n->key,
				col(n), uncle->key, col(uncle));
		rotate(tree, newroot, newroot->right == n);
		n = newroot;
	} /* Fall through to case 3 */
	eprintf(">> Case 3: %d(%c), with uncle %d(%c)\n", n->key, col(n),
			uncle->key, col(uncle));
	n->parent->color = BLACK;
	gp->color = RED;
	rotate(tree, gp, gp->left == uncle);
	/* If root node was changed, update the pointer */
#ifdef DEBUG
	{
		rb_node realroot = n;
		while (realroot->parent != tree->nil)
			realroot = realroot->parent;
		eprintf(">> n->parent = %d(%c); root = %d(%c); realroot = %d(%c)\n",
				n->parent->key, col(n->parent),
				tree->root->key, col(tree->root),
				realroot->key, col(realroot));
	}
#endif
	tree->root->color = BLACK;
}


static void rotate(rb_tree tree, rb_node root, int go_left) {
	rb_node newroot = (go_left) ? root->right : root->left;

	if (go_left) {
		root->right = newroot->left;
		root->right->parent = root;
		newroot->left = root;
	} else {
		root->left = newroot->right;
		root->left->parent = root;
		newroot->right = root;
	}
	newroot->parent = root->parent;
	root->parent = newroot;
	if (newroot->parent == tree->nil) {
		tree->root = newroot;
	} else if (newroot->parent->left == root) {
		newroot->parent->left = newroot;
	} else {
		newroot->parent->right = newroot;
	}
}
