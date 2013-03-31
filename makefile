CC=gcc 
CFLAGS=$(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

all: libWordFilter.so iteractive_ac test_ac
.PHONY : all

libWordFilter.so : ac.c unicode.c pybind.c
	$(CC) $(CFLAGS) -o $@ -shared $^ $(LDFLAGS)


iteractive_ac: ac.c unicode.c ac_iteractive.c 
	$(CC) $^ -o $@

test_ac: ac.c unicode.c ac_test.c 
	$(CC) $^ -o $@

clean: 
	rm -rf libWordFilter.so iteractive_ac test_ac
