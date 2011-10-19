#include "RBtree.h"
#include <stdio.h>

#define MAX 50

static void checktree(rb_tree tree) {
	int i;
	for (i=0; i<=MAX+1; ++i) {
		if (rb_node_exists(tree, i)) {
			printf("%d is in the tree.\n", i);
		} else {
			printf("%d is NOT in the tree.\n", i);
		}
	}
}

int main(int argc, char *argv[]) {
	rb_tree root = rb_create();
	int i;

	for (i=1; i<=MAX; ++i) {
		if (!RBinsert(root, i)) {
			fprintf(stderr, "Error! %i already in the tree.\n", i);
		}
	}

	checktree(root);

	for (i = 1; i <= MAX; i += 2) {
		if (!RBdelete(root, i)) {
			fprintf(stderr, "Error! %i not found!\n", i);
		}
	}

	checktree(root);

	return 0;
}
