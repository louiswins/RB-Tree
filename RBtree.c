#include "RBtree.h"
#include "RBtree_priv.h"
#include <stdlib.h>
#include <stdio.h>


/******************************************************************************
 * Section 1: Creation and Deallocation
 ******************************************************************************/
/* Creates an empty Red-Black tree. */
rb_tree RBcreate() {
	rb_tree ret;
	if ((ret = malloc(sizeof(*ret))) == NULL) {
		fprintf(stderr, "Error: out of memory.\n");
		return NULL;
	}
	/* We can't use rb_new_node() because it wants to set some of the values
	 * to tree->nil. We don't care what the values are except for color, so
	 * leave them at undefined. */
	if ((ret->nil = malloc(sizeof(*ret->nil))) == NULL) {
		fprintf(stderr, "Error: out of memory.\n");
		free(ret);
		return NULL;
	}
	ret->nil->color = 'b';
	ret->nil->lchild = ret->nil;
	ret->nil->rchild = ret->nil;
	ret->nil->parent = ret->nil;
	ret->root = ret->nil;
	return ret;
}
/* Frees an entire tree. */
void RBdestroy(rb_tree tree) {
	rb_delete_subtree(tree, tree->root);
	rb_free_node(tree->nil);
	free(tree);
}
/* Helper routine: frees a subtree rooted at specified node. */
static void rb_delete_subtree(rb_tree tree, rb_node node) {
	if (node == tree->nil) return;
	rb_delete_subtree(tree, node->lchild);
	rb_delete_subtree(tree, node->rchild);
	rb_free_node(node);
}
/* Creates a new node, taking from the memory pool if available. */
static rb_node rb_new_node(rb_tree tree, int data) {
	rb_node ret;
	if (rb_mem_pool != NULL) {
		ret = rb_mem_pool;
		rb_mem_pool = ret->parent;
	} else {
		if ((ret = malloc(sizeof(*ret))) == NULL) {
			fprintf(stderr, "Error: out of memory.\n");
			return NULL;
		}
	}
	ret->key = data;
	ret->parent = tree->nil;
	ret->lchild = tree->nil;
	ret->rchild = tree->nil;
	ret->color = 'r';
	return ret;
}
/* Frees a node to the memory pool. */
static void rb_free_node(rb_node node) {
	node->parent = rb_mem_pool;
	rb_mem_pool = node;
}
/* Frees entire memory pool. */
void RBcleanup() {
	while (rb_mem_pool != NULL) {
		rb_node cur = rb_mem_pool;
		rb_mem_pool = cur->parent;
		free(cur);
	}
}







/******************************************************************************
 * Section 2: Insertion
 ******************************************************************************/
/* Inserts an element with specified key into tree. */
int RBinsert(rb_tree tree, int key) {
	rb_node newnode = rb_new_node(tree, key);
	if (newnode == NULL) {
		return 0;
	}
	if (!rb_unsafe_insert(tree, newnode)) {
		rb_free_node(newnode);
		fprintf(stderr, "Error: node %i already in the tree.\n", key);
		return 0;
	}
	rb_insert_fix(tree, newnode);
	return 1;
}
/* Helper routine: acts a binary tree insertion. DOES NOT PRESERVE RED-BLACK
 * PROPERTIES. */
static int rb_unsafe_insert(rb_tree tree, rb_node n) {
	rb_node newparent = tree->nil;
	rb_node root = tree->root;
	while (root != tree->nil) {
		newparent = root;
		if (n->key < root->key) {
			root = root->lchild;
		} else if (n->key > root->key) {
			root = root->rchild;
		} else {
			return 0;
		}
	}
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
	return 1;
}
/* Corrects for properties violated on a single insertion. */
static void rb_insert_fix(rb_tree tree, rb_node n) {
	rb_node gp = n->parent->parent,
		uncle = rb_get_uncle(tree, n);

	/* Case 1: uncle is colored red */
	while (n->parent->color == 'r' && uncle->color == 'r') {
		gp->color = 'r';
		uncle->color = 'b';
		n->parent->color = 'b';
		n = gp;
		gp = n->parent->parent;
		uncle = rb_get_uncle(tree, n);
	}
	
	if (n->parent->color == 'b') {
		if (n == tree->root) n->color = 'b';
		return;
	}

	/* Case 2: node is "close" to uncle */
	if ((n->parent->lchild == n) == (gp->lchild == uncle)) {
		rb_node newroot = n->parent;
		rb_rotate(tree, newroot, newroot->rchild == n);
		n = newroot;
	} /* Fall through to case 3 */
	n->parent->color = 'b';
	gp->color = 'r';
	rb_rotate(tree, gp, gp->lchild == uncle);
	tree->root->color = 'b';
}
/* Helper routine: returns the uncle of a given node. */
static rb_node rb_get_uncle(rb_tree tree, rb_node n) {
	rb_node gp;
	if (n->parent == tree->nil || n->parent->parent == tree->nil) {
		return tree->nil;
	}
	gp = n->parent->parent;
	return (gp->lchild == n->parent) ? gp->rchild : gp->lchild;
}






/******************************************************************************
 * Section 3: Deletion
 ******************************************************************************/
/* Deletes an element with a particular key. */
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

	if (dead->lchild == tree->nil) {
		fixit = dead->rchild;
		rb_transplant(tree, dead, fixit);
	} else if (dead->rchild == tree->nil) {
		fixit = dead->lchild;
		rb_transplant(tree, dead, fixit);
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
		rb_delete_fix(tree, fixit);
	}
	return 1;
}
/* Helper routine: transplants node from into node to's position. */
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
/* Fixes possible violations from node deletion. */
static void rb_delete_fix(rb_tree tree, rb_node n) {
	while (n != tree->root && n->color == 'b') {
		int is_left = n == n->parent->lchild;
		rb_node sibling = (is_left) ? n->parent->rchild : n->parent->lchild;
		/* Case 1: sibling red */
		if (sibling->color == 'r') {
			sibling->color = 'b';
			sibling->parent->color = 'r';
			rb_rotate(tree, sibling->parent, is_left);
			sibling = (is_left) ? n->parent->rchild : n->parent->rchild;
		}
		/* Case 2: sibling black, both sibling's children black */
		if (sibling->lchild->color == 'b' && sibling->rchild->color == 'b') {
			sibling->color = 'r';
			n = n->parent;
		} else {
			/* Case 3: sibling black, "far" child black */
			if (( is_left && sibling->rchild->color == 'b') ||
			    (!is_left && sibling->lchild->color == 'b')) {
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








/******************************************************************************
 * Section 4: I/O
 ******************************************************************************/
/* Writes a tree to stdout in preorder format. */
void RBwrite(rb_tree tree) {
	if (tree->root == tree->nil) {
		fprintf(stderr, "Error: empty tree\n");
		return;
	}
	/* Special case to account for missing semicolon */
	printf("%c, %d", tree->root->color, tree->root->key);
	rb_preorder_write(tree, tree->root->lchild);
	rb_preorder_write(tree, tree->root->rchild);
	putchar('\n');
}
/* Helper routine: write an entire subtree to stdout. */
static void rb_preorder_write(rb_tree tree, rb_node n) {
	if (n == tree->nil) return;
	printf("; %c, %d", n->color, n->key);
	rb_preorder_write(tree, n->lchild);
	rb_preorder_write(tree, n->rchild);
}
/* Reads a tree in preorder format from RBREADFILE. */
rb_tree RBread(char *fname) {
	rb_tree ret;
	FILE *infp = fopen(fname, "r");
	if (infp == NULL) {
		fprintf(stderr, "Error: couldn't read file %s.\n", fname);
		return NULL;
	}
	ret = RBcreate();
	if (ret == NULL) {
		fclose(infp);
		return NULL;
	}
	do {
		char col;
		int data;
		if (fscanf(infp, " %c, %d ", &col, &data) != 2) {
			fprintf(stderr, "File format error: continuing.\n");
 		} else if (col != 'b' && col != 'r') {
			fprintf(stderr, "Invalid node color `%c': skipping.\n", col);
		} else {
			rb_node n = rb_new_node(ret, data);
			if (n == NULL) {
				break;
			}
			n->color = col;
			rb_unsafe_insert(ret, n);
		}
	/* skip over semicolon; if it isn't there, we know that input has ended. */
	} while (getc(infp) == ';');
	fclose(infp);
	return ret;
}









/******************************************************************************
 * Section 5: General helper methods
 ******************************************************************************/
/* Returns a node with the given key. */
static rb_node rb_get_node_by_key(rb_tree haystack, int needle) {
	rb_node root = haystack->root;
	while (root != haystack->nil) {
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
/* Rotates a tree around the given root. */
static void rb_rotate(rb_tree tree, rb_node root, int go_left) {
	rb_node newroot = (go_left) ? root->rchild : root->lchild;

	if (go_left) {
		root->rchild = newroot->lchild;
		/* We CANNOT CHANGE NIL'S PARENT because it totally messes up
		 * rb_delete_fix, which relies on the node to be fixed's parent
		 * stay constant. */
		if (root->rchild != tree->nil) {
			root->rchild->parent = root;
		}
		newroot->lchild = root;
	} else {
		root->lchild = newroot->rchild;
		/* See above. */
		if (root->lchild != tree->nil) {
			root->lchild->parent = root;
		}
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
/* Returns minimum node in the given subtree. */
static rb_node rb_min(rb_tree tree, rb_node node) {
	while (node->lchild != tree->nil)
		node = node->lchild;
	return node;
}




/******************************************************************************
 * Section 6: SVG
 ******************************************************************************/
/* Draws an SVG picture of the tree in the specified file. */
void RBdraw(rb_tree tree, char *fname) {
	FILE *fp;
	int height = rb_height(tree);
	int width;
	if (height == 0) return;
	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "Error: couldn't open %s for writing.\n", fname);
		return;
	}
	width = pow2(height) * (RADIUS + PADDING);
	if (width > MAXWIDTH) width = MAXWIDTH;
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
		"<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%dpx\" height=\"%dpx\" "
		"style=\"background-color:white\">\n",
		width, height * (2*RADIUS + PADDING) + PADDING);
	rb_draw_subtree(fp, tree, tree->root, 0, width - RADIUS - PADDING, RADIUS+PADDING);
	fputs("</svg>\n", fp);
	fclose(fp);
}
/* Draws a subtree rooted at a given node between the x-coordinates l and r,
 * starting at height h. */
static void rb_draw_subtree(FILE *fp, rb_tree tree, rb_node n, int l, int r, int h) {
	char *col = (n->color == 'b') ? "black" : "red";
	int mid = (l+r)/2;
	int nexth = h + 2*RADIUS + PADDING;
	if (n->lchild != tree->nil) {
		fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
			"style=\"stroke:black;stroke-width:1\"/>\n",
			mid, h, (l+mid)/2, nexth);
		rb_draw_subtree(fp, tree, n->lchild, l, mid, nexth);
	}
	if (n->rchild != tree->nil) {
		fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
			"style=\"stroke:black;stroke-width:1\"/>\n",
			mid, h, (mid+r)/2, nexth);
		rb_draw_subtree(fp, tree, n->rchild, mid, r, nexth);
	}
	fprintf(fp, "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" stroke=\"black\" "
		"stroke-width=\"1\" fill=\"%s\"/>\n", (l+r)/2, h, RADIUS, col);
	fprintf(fp, "<text x=\"%d\" y=\"%d\" fill=\"white\" text-anchor=\"middle\" "
		"dy=\"0.5ex\">%d</text>\n", (l+r)/2, h, n->key);
}
/* Computes height of a tree. */
static int rb_height(rb_tree tree) {
	return rb_subheight(tree, tree->root);
}
/* Computes height of a subtree. */
static int rb_subheight(rb_tree tree, rb_node n) {
	int l, r;
	if (n == tree->nil) return 0;
	l = rb_subheight(tree, n->lchild);
	r = rb_subheight(tree, n->rchild);
	return 1 + ((l > r) ? l : r);
}
/* Computes 2^h, twice the maximum width of a tree of height h. */
static int pow2(int h) {
	int ret = 1, base = 2;
	while (h) {
		if (h & 1)
			ret *= base;
		h >>= 1;
		base *= base;
	}
	return ret;
}
