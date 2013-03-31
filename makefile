CC=gcc
CFLAGS=$(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

libWordFilter.so : ac.c unicode.c pybind.c
	$(CC) $(CFLAGS) -o libWordFilter.so -shared $^ $(LDFLAGS)


iteractive_ac: ac.c unicode.c ac_iteractive.c 
	$(CC)  $^ -o $@

test_ac: ac.c unicode.c ac_test.c 
	$(CC) $^ -o $@

all: libWordFilter.so iteractive_ac test_ac
.PHONY : all

clean: 
	rm -rf libWordFilter.so iteractive_ac test_ac
