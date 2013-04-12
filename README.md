A Sensitive Word Filter library implemented in C, using Aho-Corasick automation, binding for Python 


build:

 make all;

use in python:
 
 import ac_dict
 
 d = ac_dict.AC_Dict("sensitive_words.conf")
 
 res = d.check("something")


use in C:
 see ac.h
