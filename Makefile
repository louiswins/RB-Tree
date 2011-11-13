ZIPFILE = P2-Wilson-Louis.zip
INZIP = main.c RBtree.c RBtree.h RBtree_priv.h README.txt Makefile
CFLAGS += -Wall -pedantic
LDFLAGS += -s

OBJECTS = main.o RBtree.o

all: run

run: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

main.o: RBtree.h
RBtree.o: RBtree.h RBtree_priv.h

clean:
	-rm run $(OBJECTS) 

$(ZIPFILE): $(INZIP)
	zip $(ZIPFILE) $(INZIP)

zip: $(ZIPFILE)
