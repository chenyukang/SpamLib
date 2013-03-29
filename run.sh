
#*******************************************************************************/
# 
#       RUN_SH
# 
#       @brief
# 
#       @author   Yukang Chen  @date  2013-03-29 15:34:14
# 
#       COPYRIGHT (C) 2006~2012, Nextop INC., ALL RIGHTS RESERVED.
# 
# ******************************************************************************/

#!/bin/sh

gcc -I/usr/include/python2.5 -I/usr/include/python2.5 -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -o libSpamSys.so -export-dybanuc  -shared spamsys.c KingUnicode.c 


