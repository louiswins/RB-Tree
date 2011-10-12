CFLAGS += -Wall -pedantic

rbtree: main.o rbtree.o

main.o: rbtree.h
rbtree.o: rbtree.h
