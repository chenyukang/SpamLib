
A Sensitive Word Filter library implemented in C, using Aho-Corasick automation, binding for Python

## Usage

build:

```shell
make all;
```


Using this library in Python:

```python
 import ac_dict
 
 d = ac_dict.AC_Dict("sensitive_words.conf")
 
 res = d.check("something")
```

How to use in C?

 see ac.h


## License

[The FreeBSD Copyright](http://www.freebsd.org/copyright/freebsd-license.html)
