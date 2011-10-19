#include "RBtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

static unsigned time_seed() {
	time_t now = time(0);
	unsigned char *p = (unsigned char *)&now;
	unsigned seed = 0;
	size_t i;

	for (i=0; i < sizeof(now); ++i)
		seed = seed * (UCHAR_MAX + 2U) + p[i];
	return seed;
}


#define HOWMANY 30
#define MAX 99

int main(int argc, char *argv[]) {
	rb_tree root = RBcreate();
	int i;

	srand(time_seed());

	for (i=0; i<HOWMANY; ++i) {
		int r = ((double)rand() * MAX) / (RAND_MAX + 1.0);
		printf("Inserting %d into tree.\n", r);
		if (!RBinsert(root, r)) {
			i -= 1;
		} else {
			/* demonstrate that it doesn't segfault */
			RBinsert(root, r);
		}
	}

	RBwrite(root);

	RBdraw(root, "test.svg");	

	RBdestroy(root);
	RBcleanup();

	return 0;
}
