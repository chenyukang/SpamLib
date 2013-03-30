CC=gcc
CFLAGS=$(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

libWordFilter.so : trie.c unicode.c pybind.c
	$(CC) $(CFLAGS) -o libWordFilter.so -shared trie.c unicode.c pybind.c $(LDFLAGS)

clean: 
	rm -rf libWordFilter.so
