
#*******************************************************************************/
# 
#       RUN_SH
# 
#       @brief
# 
#       @author   Yukang Chen  @date  2013-03-29 15:34:14
# 
# 
# ******************************************************************************/

#!/bin/sh

gcc -I/usr/include/python2.5 -I/usr/include/python2.5 -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 \
 -Wall -Wstrict-prototypes -o libSpamSys.so  -shared spamsys.c unicode.c 



