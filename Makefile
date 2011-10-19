ZIPFILE = P2-Wilson-Louis.zip
CFLAGS += -Wall -Wno-variadic-macros -pedantic

all: run

run: main.o RBtree.o
	$(CC) $(CFLAGS) $(LDFLAGS) -s -o $@ $^

main.o: RBtree.h
RBtree.o: RBtree.h RBtree_priv.h

clean:
	rm run *.o

zip:
	zip $(ZIPFILE) main.c RBtree.c RBtree.h RBtree_priv.h README.txt Makefile
