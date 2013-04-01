CC=gcc 
CFLAGS=$(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

all: ac_dict.so iteractive_ac test_ac
.PHONY : all

ac_dict.so : ac.c unicode.c pybind.c
	$(CC) $(CFLAGS) -o $@ -shared $^ $(LDFLAGS)


iteractive_ac: ac.c unicode.c ac_iteractive.c 
	$(CC) $^ -o $@

test_ac: ac.c unicode.c ac_test.c 
	$(CC) $^ -o $@

clean: 
	rm -rf *.so iteractive_ac test_ac
