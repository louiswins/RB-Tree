#include "rbtree.h"
#include <stdlib.h>

struct rb_tree {
	int data;
	struct rb_tree *parent;
	struct rb_tree *left,
		       *right;
	enum { RED, BLACK } color;
};

static rb_tree rb_new_node(int data);
static void fix(rb_tree root);
static void rotate(rb_tree root, int go_left);





rb_tree rb_create() {
	return NULL;
}

void rb_insert(rb_tree *root, int data) {
	rb_tree rootnode = *root;
	if (rootnode == NULL) {
		rb_tree tmp = rb_new_node(data);
		tmp->parent = rootnode;
		*root = tmp;
		fix(*root);
	} else if (data < rootnode->data) {
		rb_insert(&rootnode->left, data);
	} else {
		rb_insert(&rootnode->right, data);
	}
}

static rb_tree rb_new_node(int data) {
	rb_tree ret;
	if ((ret = malloc(sizeof(*ret))) == NULL)
		return NULL;
	ret->data = data;
	ret->parent = NULL;
	ret->left = NULL;
	ret->right = NULL;
	ret->color = RED;
	return ret;
}

static void fix(rb_tree root) {
	rb_tree grandparent,
		uncle;
	int is_left_child,
	    uncle_is_left_child;
	/* Preserve property 1 */
	if (root->parent == NULL) {
		root->color = BLACK;
		return;
	}
	/* Not violating any property */
	if (root->parent->color == BLACK) return;

	if (root->parent->left == root) {
		is_left_child = 1;
	} else {
		is_left_child = 0;
	}
	grandparent = root->parent->parent;
	if (grandparent->left == root->parent) {
		uncle = grandparent->right;
		uncle_is_left_child = 0;
	} else {
		uncle = grandparent->left;
		uncle_is_left_child = 1;
	}

	/* Case 1 */
	if (uncle->color == RED) {
		grandparent->color = RED;
		uncle->color = BLACK;
		root->parent->color = BLACK;
		fix(grandparent);
		return;
	}

	/* Case 2: node is "close" to uncle */
	if (is_left_child == uncle_is_left_child) {
		rb_tree newroot = root->parent;
		rotate(newroot, !is_left_child);
		root = newroot;
	} /* Fall through to case 3 */
	root->parent->color = BLACK;
	grandparent->color = RED;
	rotate(grandparent, uncle_is_left_child);
}


static void rotate(rb_tree root, int go_left) {
	rb_tree newroot = (go_left) ? root->right : root->left;
	rb_tree beta = (go_left) ? newroot->left : newroot->right;
	rb_tree parent = root->parent;

	if (go_left) {
		newroot->left = root;
		root->right = beta;
	} else {
		newroot->right = root;
		root->left = beta;
	}
	root->parent = newroot;
	beta->parent = root;
	newroot->parent = parent;
	if (parent != NULL) {
		if (parent->left == root) {
			parent->left = newroot;
		} else {
			parent->right = newroot;
		}
	}
}
