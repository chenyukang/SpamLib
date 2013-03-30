#include "ac.h"
#include "unicode.h"
#include <stdio.h>
#include <string.h>

int main() {
    struct AC_Dict* dict = AC_New_Empty_Dict();
    char str1[100];
    char str2[100];
    strcpy(str1, "泽");
    strcpy(str2, "江泽民");
    unsigned int length;
    unsigned int buf[256];
    length = UTF8toUnicode(str1, strlen(str1), buf, 256);
    AC_Add_Word(dict, buf, length);
    length = UTF8toUnicode(str1, strlen(str1), buf, 256);
    AC_Add_Word(dict, buf, length);
    AC_Build_Automation(dict);
    char str[100];
    while(gets(str)) {
        printf("gots: %s\n", str);
        AC_Shield_Word(dict, str);
        printf("result: %s\n", str);
    }
}
