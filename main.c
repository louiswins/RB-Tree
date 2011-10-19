#include "RBtree.h"
#include <stdio.h>

#define MAX 8

int main(int argc, char *argv[]) {
	rb_tree root = rb_create();
	int i;

	for (i=1; i<=MAX; ++i) {
		if (!RBinsert(root, i)) {
			fprintf(stderr, "Error! %i already in the tree.\n", i);
		}
	}

	RBwrite(root);

	for (i = 1; i <= MAX; i += 2) {
		if (!RBdelete(root, i)) {
			fprintf(stderr, "Error! %i not found!\n", i);
		}
	}

	RBwrite(root);

	return 0;
}
