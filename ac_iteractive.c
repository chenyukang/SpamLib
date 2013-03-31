#include "ac.h"
#include "unicode.h"
#include <stdio.h>
#include <string.h>

int main() {
    struct AC_Dict* dict = AC_New_Dict("sensitive_words.conf");
    char str1[256];
    char str2[256];
    printf("Input all keywords, use \"eof\" to end:\n");
    while(1) {
        scanf("%s", str1);
        if(strcmp(str1, "eof") == 0)
            break;
        AC_Add_Word(dict, str1, (size_t)strlen(str1));
    }
    AC_Build_Automation(dict);
    printf("Input word to query:\n");
    while(1) {
        scanf("%s", str2);
        AC_Shield_Word(dict, str2);
        printf("result: %s\n", str2);
    }
}
