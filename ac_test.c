#include "ac.h"
#include "unicode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


//read words from sensitive_words.conf
//since dict is empty, all words should keep same
int emptydict_test() {
    const char* file = "sensitive_words.conf";
    struct AC_Dict* dict = AC_New_Dict(0);
    FILE* fp = fopen(file, "r");
    char line[256];
    char cmp[256];
    int k, len;
    while((fgets(line, sizeof(line), fp) != NULL)) {
        len = strlen(line);
        if(line[len-1] == '\n')
            line[len-1] = '\0';
        strcpy(cmp, line);
        AC_Shield_Word(dict, line);
        if(strcmp(cmp, line) != 0) {
            printf("error: %s(%u) -- %s(%u)\n", cmp,
                   (unsigned int)strlen(cmp), line, (unsigned int)strlen(line));
            assert(0);
        }
    }
    AC_Destory_Dict(dict);
    fclose(fp);
    return 0;
}


//read from sensitive_words.conf,
//all key word should be replaced with *
int all_keyword_test() {
    const char* file = "sensitive_words.conf";
    struct AC_Dict* dict = AC_New_Dict(file);
    FILE* fp = fopen(file, "r");
    char line[256];
    char cmp[256];
    int k, len;
    while((fgets(line, sizeof(line), fp) != NULL)) {
        len = strlen(line);
        if(line[len-1] == '\n')
            line[len-1] = '\0';
        AC_Shield_Word(dict, line);
        len = strlen(line);
        for(k=0; k<len; k++) {
            cmp[k] = '*';
        }
        cmp[len] = '\0';
        if(strcmp(cmp, line) != 0) {
            printf("error: %s(%u) -- %s(%u)\n", cmp,
                   (unsigned int)strlen(cmp), line, (unsigned int)strlen(line));
            assert(0);
        }
    }
    AC_Destory_Dict(dict);
    fclose(fp);
    return 0;
}

//we read all words from sensitive_words.conf, but just
//add to dict in random, assume the words is queried right
//Note: we assume there is no much common character in sensitive_words.conf
//so this test case is same
int random_test() {
    const char* file = "sensitive_words.conf";
    unsigned int len, prev_len;
    int line_cnt = 0;
    char* buf[4000];
    char line[246];
    char cmp[256];
    int i, k;
    
    FILE* fp = fopen(file, "r");
    struct AC_Dict* dict = AC_New_Dict(0);
    memset(buf, 0, sizeof(buf));
    while((fgets(line, sizeof(line), fp) != NULL)) {
        len = strlen(line);
        if(line[len-1] == '\n')
            line[len-1] = '\0';
        buf[line_cnt++] = strdup(line);
    }

    for(i=0; i<line_cnt; i++) {
        if(i%2 == 0) {
            AC_Add_Word(dict, buf[i], (size_t)strlen(buf[i]));
        }
    }

    AC_Build_Automation(dict);
    for(i=0; i<line_cnt; i++) {
        if(i%2 != 0) {
            memset(line, 0, sizeof(line));
            strcpy(line, buf[i]);
            memset(cmp, 0, sizeof(cmp));
            strcpy(cmp, buf[i]);
            AC_Shield_Word(dict, line);
            if(strcmp(line, cmp) != 0) {
                //printf("results: %s cmp: %s\n", line, cmp);
                //must have one or more *
                for(k=0; k<strlen(line); k++) {
                    if(line[k] == '*') break;
                }
                if(k == strlen(line)) {
                    assert(0);
                }
            }
            //assert(strcmp(line, cmp) == 0);
        } else { //for all word added into dict, should be replace with *
            memset(line, 0, sizeof(line));
            strcpy(line, buf[i]);
            prev_len = strlen(line);
            AC_Shield_Word(dict, line);
            assert(strlen(line) <= prev_len);
            for(k=0; k<strlen(line); k++) {
                if(line[k] != '*') {
                    printf("really: %s\n", line);
                    break;
                }
            }
        }
    }
    for(i=0; i<line_cnt; i++)
        free(buf[i]);
    AC_Destory_Dict(dict);
}

int one_char_test() {
    struct AC_Dict* dict = AC_New_Dict(0);
    unsigned int len;
    unsigned int* res;
    char line[256];
    char c;
    
    res = (unsigned int*)malloc(sizeof(unsigned int) * 256);
    for(c='a'; c<='z'; c++) {
        memset(line, 0, sizeof(line));
        line[0] = c;
        AC_Add_Word(dict, line, 1);
    }
    AC_Build_Automation(dict);
    for(c='a'; c<='z'; c++) {
        memset(line, 0, sizeof(line));
        line[0] = c;
        AC_Shield_Word(dict, line);
        assert(strlen(line) == 1);
        assert(line[0] == '*');
    }
    for(c='A'; c<='Z'; c++ ) {
        memset(line, 0, sizeof(line));
        line[0] = c;
        AC_Shield_Word(dict, line);
        assert(strlen(line) == 1);
        assert(line[0] == c);
    }
    AC_Destory_Dict(dict);
    free(res);
    return 0;
}


int main() {
    emptydict_test();
    all_keyword_test();
    one_char_test();
    random_test();
    printf("all tests passed\n");
    return 0;
}
