CC=gcc
CFLAGS=$(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

libWordFilter.so : trie.c unicode.c pybind.c ac.o
	$(CC) $(CFLAGS) -o libWordFilter.so -shared trie.c unicode.c pybind.c $(LDFLAGS)

ac.o: ac.c
	$(CC) -c ac.c -o ac.o
clean: 
	rm -rf libWordFilter.so
