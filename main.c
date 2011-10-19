#include "RBtree.h"
#include <stdio.h>

#define MAX 50

int main(int argc, char *argv[]) {
	rb_tree root = rb_create();
	int i;

	for (i=1; i<=MAX; ++i) {
		if (!rb_insert(root, i)) {
			fprintf(stderr, "Error! %i already in the tree.\n",
					i);
		}
	}

	for (i=0; i<=MAX+1; ++i) {
		if (rb_node_exists(root, i)) {
			printf("%d is in the tree.\n", i);
		} else {
			printf(">> %d is NOT in the tree.\n", i);
		}
	}

	return 0;
}
