#include "RBtree.h"
#include <stdio.h>

#define MAX 50

int main(int argc, char *argv[]) {
	rb_tree root = RBcreate();
	int i;

	for (i=1; i<=MAX; ++i) {
		RBinsert(root, i);
	}

	RBwrite(root);

/*
	for (i=1; i<=MAX; i += 2) {
		RBdelete(root, i);
	}

	RBwrite(root);

	for (i=MAX+1; i<=2*MAX; ++i) {
		RBinsert(root, i);
	}

	RBwrite(root);
*/

	RBdraw(root, "test.svg");

	RBdestroy(root);
	RBcleanup();

	return 0;
}
