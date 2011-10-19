#include "RBtree.h"
#include "RBtree_priv.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef DEBUG
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

int RBinsert(rb_tree tree, int data) {
	rb_node newnode = rb_new_node(tree, data);
	if (newnode == NULL) {
		fprintf(stderr, "Error: out of memory.\n");
		return 0;
	}
	newnode = rb_unsafe_insert(tree, newnode);
	if (newnode == NULL) {
		rb_free_node(newnode);
		fprintf(stderr, "Error: node %i already in the tree.\n", data);
		return 0;
	}
	rb_insert_fix(tree, newnode);
	return 1;
}

int rb_node_exists(rb_tree haystack, int needle) {
	return rb_get_node_by_key(haystack, needle) != haystack->nil;
}

int RBdelete(rb_tree tree, int key) {
	rb_node dead = rb_get_node_by_key(tree, key);
	rb_node replacewith = dead;
	rb_node fixit;
	char orig_col = replacewith->color;
	/* Node does not exist, so we cannot delete it */
	if (dead == tree->nil) {
		fprintf(stderr, "Error: node %i does not exist.\n", key);
		return 0;
	}
	eprintf("> Deleting node %d(%c)\n", dead->key, dead->color);

	if (dead->lchild == tree->nil) {
		fixit = dead->rchild;
		rb_transplant(tree, dead, dead->rchild);
	} else if (dead->rchild == tree->nil) {
		fixit = dead->lchild;
		rb_transplant(tree, dead, dead->lchild);
	} else {
		replacewith = rb_min(tree, dead->rchild);
		orig_col = replacewith->color;
		fixit = replacewith->rchild;
		if (replacewith->parent == dead) {
			fixit->parent = replacewith;
		} else {
			rb_transplant(tree, replacewith, replacewith->rchild);
			replacewith->rchild = dead->rchild;
			replacewith->rchild->parent = replacewith;
		}
		rb_transplant(tree, dead, replacewith);
		replacewith->lchild = dead->lchild;
		replacewith->lchild->parent = replacewith;
		replacewith->color = dead->color;
	}
	rb_free_node(dead);
	/* If the color of replacewith was black, we have a violation of
	 * property 5, and possibly 4 as well. */
	if (orig_col == 'b') {
		eprintf(">> Fixing tree at node %d(%c)\n", fixit->key, fixit->color);
		rb_delete_fix(tree, fixit);
	}
	return 1;
}

void RBwrite(rb_tree tree) {
	rb_node prev = rb_preorder_write(tree, tree->root, NULL);
	if (prev != NULL)
		printf("%c, %d\n", prev->color, prev->key);
}

rb_tree RBread() {
	rb_tree ret;
	FILE *infp = fopen(RBREADFILE, "r");
	if (infp == NULL) {
		fprintf(stderr, "Error: couldn't read file %s.\n", RBREADFILE);
		return NULL;
	}
	ret = rb_create();
	if (ret == NULL) {
		fclose(infp);
		fprintf(stderr, "Error: out of memory.\n");
		return NULL;
	}
	do {
		char col;
		int data;
		if (fscanf(infp, " %c, %d", &col, &data) != 2 || (col != 'b' && col != 'r')) {
			fprintf(stderr, "File format error: continuing...\n");
		} else {
			rb_node n = rb_new_node(ret, data);
			if (n == NULL) {
				fprintf(stderr, "Error: out of memory.\n");
				break;
			}
			n->color = col;
			rb_unsafe_insert(ret, n);
		}
	/* skip over semicolon; if it isn't there, we know that input has ended. */
	} while (getchar() == ';');
	fclose(infp);
	return ret;
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



static rb_node rb_unsafe_insert(rb_tree tree, rb_node n) {
	rb_node newparent = tree->nil;
	rb_node root = tree->root;
	while (root != tree->nil) {
		newparent = root;
		if (n->key < root->key) {
			root = root->lchild;
		} else if (n->key > root->key) {
			root = root->rchild;
		} else {
			return NULL;
		}
	}
	eprintf("> Inserting node %d(%c) below %d(%c)\n", n->key,
			n->color, newparent->key, newparent->color);
	n->parent = newparent;
	/* If we inserted a new root into the tree */
	if (newparent == tree->nil) {
		tree->root = n;
	}
	if (n->key < newparent->key) {
		newparent->lchild = n;
	} else {
		newparent->rchild = n;
	}
	return n;
}

static void rb_insert_fix(rb_tree tree, rb_node n) {
	rb_node gp = n->parent->parent,
		uncle = rb_get_uncle(tree, n);

	/* Case 1: uncle is colored red */
	while (n->parent->color == 'r' && uncle->color == 'r') {
		/* If gp were null, then n->parent would be the root node (or
		 * tree->nil), and would have to have been black. */
		eprintf(">> Insertion case 1: %d(%c), with uncle %d(%c)\n", n->key,
				n->color, uncle->key, uncle->color);
		gp->color = 'r';
		uncle->color = 'b';
		n->parent->color = 'b';
		n = gp;
		gp = n->parent->parent;
		uncle = rb_get_uncle(tree, n);
	}
	
	eprintf(">> Insertion case 1 taken care of, on %d(%c) with parent %d(%c)\n",
			n->key, n->color, n->parent->key, n->parent->color);
	if (n->parent->color == 'b') {
		if (n == tree->root) n->color = 'b';
		return;
	}

	/* Case 2: node is "close" to uncle */
	if ((n->parent->lchild == n) == (gp->lchild == uncle)) {
		rb_node newroot = n->parent;
		eprintf(">> Insertion case 2: %d(%c), with uncle %d(%c)\n", n->key,
				n->color, uncle->key, uncle->color);
		rb_rotate(tree, newroot, newroot->rchild == n);
		n = newroot;
	} /* Fall through to case 3 */
	eprintf(">> Insertion case 3: %d(%c), with uncle %d(%c)\n", n->key, n->color,
			uncle->key, uncle->color);
	n->parent->color = 'b';
	gp->color = 'r';
	rb_rotate(tree, gp, gp->lchild == uncle);
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

static void rb_delete_fix(rb_tree tree, rb_node n) {
	while (n != tree->root && n->color == 'b') {
		int is_left = n == n->parent->lchild;
		rb_node sibling = (is_left) ? n->parent->rchild : n->parent->lchild;
		/* Case 1: sibling red */
		if (sibling->color == 'r') {
			eprintf(">> Deletion case 1: %d(%c), with sibling %d(%c)\n",
					n->key, n->color, sibling->key, sibling->color);
			sibling->color = 'b';
			sibling->parent->color = 'r';
			rb_rotate(tree, sibling->parent, is_left);
			sibling = (is_left) ? n->parent->rchild : n->parent->rchild;
		}
		/* Case 2: sibling black, both sibling's children black */
		if (sibling->lchild->color == 'b' && sibling->rchild->color == 'b') {
			eprintf(">> Deletion case 2: %d(%c), with sibling %d(%c) "
					"(sibling's kids %d(%c), %d(%c))\n",
					n->key, n->color, sibling->key, sibling->color,
					sibling->lchild->key, sibling->lchild->color,
					sibling->rchild->key, sibling->rchild->color);
			/* Push the "extra black" up the tree */
			sibling->color = 'r';
			n = n->parent;
		} else {
			/* Case 3: sibling black, "far" child black */
			if ((is_left && sibling->rchild->color == 'b') ||
					(!is_left && sibling->lchild->color == 'b')) {
				eprintf(">> Deletion case 3: %d(%c), with sibling %d(%c) "
					"(sibling's kids %d(%c), %d(%c))\n",
					n->key, n->color, sibling->key, sibling->color,
					sibling->lchild->key, sibling->lchild->color,
					sibling->rchild->key, sibling->rchild->color);
				if (is_left) {
					sibling->lchild->color = 'b';
				} else {
					sibling->rchild->color = 'b';
				}
				sibling->color = 'r';
				rb_rotate(tree, sibling, !is_left);
				sibling = (is_left) ? n->parent->rchild : n->parent->lchild;
			} /* Fall through to case 4 */
			/* Case 4: sibling black, "far" child red */
			eprintf(">> Deletion case 4: %d(%c), with sibling %d(%c) "
					"(sibling's kids %d(%c), %d(%c))\n",
					n->key, n->color, sibling->key, sibling->color,
					sibling->lchild->key, sibling->lchild->color,
					sibling->rchild->key, sibling->rchild->color);
			sibling->color = n->parent->color;
			n->parent->color = 'b';
			if (is_left) {
				sibling->rchild->color = 'b';
			} else {
				sibling->lchild->color = 'b';
			}
			rb_rotate(tree, n->parent, is_left);
			/* We're done, so set n to the root node */
			n = tree->root;
		}
	}
	n->color = 'b';
}

static rb_node rb_get_node_by_key(rb_tree haystack, int needle) {
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

static rb_node rb_preorder_write(rb_tree tree, rb_node n, rb_node prev) {
	if (n == tree->nil) return prev;
	if (prev != NULL)
		printf("%c, %d; ", prev->color, prev->key);
	prev = rb_preorder_write(tree, n->lchild, n);
	return rb_preorder_write(tree, n->rchild, prev);
}



static rb_node rb_get_uncle(rb_tree tree, rb_node n) {
	rb_node gp;
	if (n->parent == tree->nil || n->parent->parent == tree->nil) {
		return tree->nil;
	}
	gp = n->parent->parent;
	return (gp->lchild == n->parent) ? gp->rchild : gp->lchild;
}

static void rb_rotate(rb_tree tree, rb_node root, int go_left) {
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

static rb_node rb_min(rb_tree tree, rb_node node) {
	while (node->lchild != tree->nil)
		node = node->lchild;
	return node;
}

static void rb_transplant(rb_tree tree, rb_node to, rb_node from) {
	if (to->parent == tree->nil) {
		tree->root = from;
	} else if (to == to->parent->lchild) {
		to->parent->lchild = from;
	} else {
		to->parent->rchild = from;
	}
	from->parent = to->parent;
}
