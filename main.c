#include "RBtree.h"
#include <stdio.h>
#include <ctype.h>

#define READFILE "RBinput.txt"
#define DRAWING "RBdrawing.svg"

void help() {
	printf(
"Louis Wilson's CSE310 Project #2\n"
"Commands:\n"
"\tC   - Create empty tree\n"
"\tR   - Read tree from %s\n"
"\tW   - Write tree to screen in preorder format\n"
"\tI n - Insert node with key `n' into tree\n"
"\tD n - Delete node with key `n' from tree\n"
"\tT   - draw Tree in %s\n"
"\tH   - Help\n"
"\tQ   - Quit (you may also use Ctrl-D)\n",
READFILE, DRAWING);
}

int main(int argc, char *argv[]) {
	rb_tree tree = NULL,
		tmp = NULL;
	int cmd = 0;

	help();
	while (cmd != EOF) {
		int arg;
		printf("> ");
		fflush(stdout);
		while (isspace((cmd = getchar()))) {
			if (cmd == '\n') {
				ungetc(cmd, stdin);
				break;
			}
		}
		switch(cmd) {
			case 'C':
			case 'c':
				if (tree != NULL) {
					printf("Tree already exists - clearing.\n");
					RBdestroy(tree);
				}
				tree = RBcreate();
				break;
			case 'R':
			case 'r':
				tmp = RBread(READFILE);
				if (tmp != NULL) {
					if (tree != NULL) {
						printf("Non-empty tree - overwriting.\n");
						RBdestroy(tree);
					}
					tree = tmp;
				}
				break;
			case 'W':
			case 'w':
				if (tree == NULL) {
					fprintf(stderr, "Error: no tree loaded, cannot write.\n");
				} else {
					RBwrite(tree);
				}
				break;
			case 'I':
			case 'i':
				if (tree == NULL) {
					printf("No tree loaded - creating empty one.\n");
					tree = RBcreate();
				}
				if (scanf("%d", &arg) != 1) {
					fprintf(stderr, "Error: must specify integer key to insert.\n");
				} else {
					RBinsert(tree, arg);
				}
				break;
			case 'D':
			case 'd':
				if (tree == NULL) {
					fprintf(stderr, "Error: no tree loaded, cannot delete.\n");
				} else {
					if (scanf("%d", &arg) != 1) {
						fprintf(stderr, "Error: must specify integer key to delete.\n");
					} else {
						RBdelete(tree, arg);
					}
				} 
				break;
			case 'T':
			case 't':
				if (tree == NULL) {
					fprintf(stderr, "Error: no tree loaded, cannot draw.\n");
				} else {
					RBdraw(tree, DRAWING);
				}
				break;
			case 'H':
			case 'h':
				help();
				break;
			case 'Q':
			case 'q':
			case EOF:
				cmd = EOF;
				break;
			case '\n':
				break;
			default:
				fprintf(stderr, "Error: unknown command `%c'.\n", cmd);
				help();
				break;
		}
		if (cmd != EOF) {
			/* Delete the rest of the line. */
			while ((cmd = getchar()) != '\n');
		}
	}


	if (tree != NULL) {
		RBdestroy(tree);
		RBcleanup();
	}

	return 0;
}
