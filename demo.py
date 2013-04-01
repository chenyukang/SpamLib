#!/usr/bin/env python
#coding=utf-8
import os
import sys
import ac_dict

def check_str(msg):
    d = ac_dict.AC_Dict("sensitive_words.conf")
    print msg, "==>", d.check(msg)
    

if __name__ == '__main__':
    if len(sys.argv) == 2:
        check_str(sys.argv[1])
    else:
        print "argument error"
        sys.exit()
        
