#include "RBtree.h"
#include "RBtree_priv.h"

#include <stdlib.h>
#ifdef DEBUG
#	include <stdio.h>
#	define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#	define eprintf(...) ((void)0)
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
	ret->nil->color = 'b';
	ret->root = ret->nil;
	return ret;
}

int rb_insert(rb_tree tree, int data) {
	rb_node newparent = tree->nil;
	rb_node root = tree->root;
	rb_node newnode;
	while (root != tree->nil) {
		newparent = root;
		if (data < root->key) {
			root = root->lchild;
		} else if (data > root->key) {
			root = root->rchild;
		} else {
			return 0;
		}
	}
	newnode = rb_new_node(tree, data);
	eprintf("> Inserting node %d(%c) below %d(%c)\n", newnode->key,
			newnode->color, newparent->key, newparen->color);
	newnode->parent = newparent;
	/* If we inserted a new root into the tree */
	if (newparent == tree->nil) {
		tree->root = newnode;
	}
	if (data < newparent->key) {
		newparent->lchild = newnode;
	} else {
		newparent->rchild = newnode;
	}
	insert_fix(tree, newnode);
	return 1;
}

int rb_node_exists(rb_tree haystack, int needle) {
	return get_node_by_key(haystack, needle) != haystack->nil;
}

int rb_delete(rb_tree tree, int key) {
	rb_node dead = get_node_by_key(tree, key);
	rb_node newpos = tree->nil;
	if (dead->lchild == tree->nil || dead->rchild == tree->nil) {
		newpos = dead;
	} else {
		newpos = successor(tree, dead);
	}
	return 0;
	/* XXX */
}







static void delete_fix(rb_tree root, rb_node n) {}
static rb_node successor(rb_tree tree, rb_node node) {
	if (node->rchild != tree->nil) {
		node = node->rchild;
		while (node->lchild != tree->nil)
			node = node->lchild;
		return node;
	} else {
		rb_node ret = node->parent;
		while (ret != tree->nil && ret->rchild == node) {
			node = ret;
			ret = ret->parent;
		}
		return ret;
	}
}
static rb_node get_node_by_key(rb_tree haystack, int needle) {
	rb_node root = haystack->root;
	while (root != haystack->nil) {
		eprintf(">> Passing through %d(%c)\n", root->key,
				root->color);
		if (root->key == needle) {
			return root;
		} else if (needle < root->key) {
			root = root->lchild;
		} else {
			root = root->rchild;
		}
	}
	return haystack->nil;
}

static rb_node rb_new_node(rb_tree tree, int data) {
	rb_node ret;
	if ((ret = malloc(sizeof(*ret))) == NULL)
		return NULL;
	ret->key = data;
	ret->parent = tree->nil;
	ret->lchild = tree->nil;
	ret->rchild = tree->nil;
	ret->color = 'r';
	return ret;
}
static void rb_free_node(rb_node node) {
	free(node);
}
static rb_node get_uncle(rb_tree tree, rb_node n) {
	rb_node gp;
	if (n->parent == tree->nil || n->parent->parent == tree->nil) {
		return tree->nil;
	}
	gp = n->parent->parent;
	return (gp->lchild == n->parent) ? gp->rchild : gp->lchild;
}
static void insert_fix(rb_tree tree, rb_node n) {
	rb_node gp = n->parent->parent,
		uncle = get_uncle(tree, n);

	/* Case 1: uncle is colored red */
	while (n->parent->color == 'r' && uncle->color == 'r') {
		/* If gp were null, then n->parent would be the root node (or
		 * tree->nil), and would have to have been black. */
		eprintf(">> Case 1: %d(%c), with uncle %d(%c)\n", n->key,
				n->color, uncle->key, uncle->color);
		gp->color = 'r';
		uncle->color = 'b';
		n->parent->color = 'b';
		n = gp;
		gp = n->parent->parent;
		uncle = get_uncle(tree, n);
	}
	
	eprintf(">> Case 1 taken care of, on %d(%c) with parent %d(%c)\n",
			n->key, n->color, n->parent->key, n->parent->color);
	if (n->parent->color == 'b') {
		if (n == tree->root) n->color = 'b';
		return;
	}

	/* Case 2: node is "close" to uncle */
	if ((n->parent->lchild == n) == (gp->lchild == uncle)) {
		rb_node newroot = n->parent;
		eprintf(">> Case 2: %d(%c), with uncle %d(%c)\n", n->key,
				n->color, uncle->key, uncle->color);
		rotate(tree, newroot, newroot->rchild == n);
		n = newroot;
	} /* Fall through to case 3 */
	eprintf(">> Case 3: %d(%c), with uncle %d(%c)\n", n->key, n->color,
			uncle->key, uncle->color);
	n->parent->color = 'b';
	gp->color = 'r';
	rotate(tree, gp, gp->lchild == uncle);
	/* If root node was changed, update the pointer */
#ifdef DEBUG
	{
		rb_node realroot = n;
		while (realroot->parent != tree->nil)
			realroot = realroot->parent;
		eprintf(">> n->parent = %d(%c); root = %d(%c); realroot = %d(%c)\n",
				n->parent->key, n->parent->color,
				tree->root->key, tree->root->color,
				realroot->key, realroot->color);
	}
#endif
	tree->root->color = 'b';
}


static void rotate(rb_tree tree, rb_node root, int go_left) {
	rb_node newroot = (go_left) ? root->rchild : root->lchild;

	if (go_left) {
		root->rchild = newroot->lchild;
		root->rchild->parent = root;
		newroot->lchild = root;
	} else {
		root->lchild = newroot->rchild;
		root->lchild->parent = root;
		newroot->rchild = root;
	}
	newroot->parent = root->parent;
	root->parent = newroot;
	if (newroot->parent == tree->nil) {
		tree->root = newroot;
	} else if (newroot->parent->lchild == root) {
		newroot->parent->lchild = newroot;
	} else {
		newroot->parent->rchild = newroot;
	}
}
