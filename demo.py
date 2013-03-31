import libWordFilter
import os
import sys
import libWordFilter

def check_str(msg):
    #if libWordFilter.filter_init("haha") == None:
    #   print "error happend"
    print libWordFilter.filter_check(msg)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        check_str(sys.argv[1])
    else:
        print "argument error"
        sys.exit()
        
