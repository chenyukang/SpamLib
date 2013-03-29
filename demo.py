import libSpamSys
import os
import sys


def check_str(msg):
    print libSpamSys.spam_check(msg)

if __name__ == '__main__':
    if len(sys.argv) == 2:
        check_str(sys.argv[1])
    else:
        print "argument error"
        sys.exit()
        
