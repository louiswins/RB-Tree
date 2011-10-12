#include "rbtree.h"
#include <stdio.h>

#define MAX 8

int main(int argc, char *argv[]) {
	rb_tree root = rb_create();
	int i;

	for (i=1; i<=MAX; ++i) {
		rb_insert(root, i);
	}

	for (i=0; i<=MAX+1; ++i) {
		if (node_exists(root, i)) {
			printf("%d is in the tree.\n", i);
		} else {
			printf(">> %d is NOT in the tree.\n", i);
		}
	}

	return 0;
}
