#include "rbtree.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	rb_tree root = rb_create();
	int i;

	for (i=1; i<=20; ++i) {
		rb_insert(&root, i);
	}

	return 0;
}
