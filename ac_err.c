#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ac.h"
#include "unicode.h"

#define ROOT_CHAR    (0xFFFFFFFF)
#define AC_CHILD_NUM (100)
#define SHIEL_REPLACE_WORD (0x0000002A)

struct AC_Node {
    struct AC_Node* fail;
    struct AC_Node* parent;
    struct AC_Node* children[256];
    unsigned int value;
    unsigned int flag;

};
    
struct AC_Dict {
    char* dict_path;
    struct AC_Node* root;
};

struct AC_Node* AC_New_Node(unsigned int value) {
    struct AC_Node* node = (struct AC_Node*)malloc(sizeof(struct AC_Node));
    assert(node);
    memset(node, 0, sizeof(struct AC_Node));
    node->value = value;
    node->flag = 0;
    memset(node->children, 0, sizeof(struct AC_Node*) * AC_CHILD_NUM);
    return node;
}


#if 0
static int AC_Add_SubNode(struct AC_Node* parent,
                          struct AC_Node* child) {
    assert(parent && child);
    assert(parent->count <= parent->size);
    size_t size = 0;
    
    if(parent->count == parent->size) {
        size = parent->size + AC_CHILD_NUM;
        parent->children = (struct AC_Node**)realloc(parent->children, size * sizeof(struct AC_Node*));
        parent->size = size;
        if(parent->children == NULL) {
            fprintf(stderr, "[AC_Dict]: realloc failed");
            return -1;
        }
    }
    assert(parent->count < parent->size);
    parent->children[parent->count++] = child;
    //printf("count: %d\n", parent->count);
    child->parent = parent;
    return 0;
}
#endif

static void AC_put_bits(unsigned int* word, unsigned int* vals, size_t len) {
    int i, j, k;
    k=0;
    for(i=0; i<len; i++) {
        for(j=0; j<4; j++)
            vals[k++] = (word[i] >> (8*j)) & (0x0000000F);
    }
}

int AC_Add_Word(struct AC_Dict* dict, unsigned int* word, size_t len) {
    assert(dict && word);
    struct AC_Node* node = dict->root;
    struct AC_Node* new = NULL;
    int i, j, k;
    unsigned int ch;
    unsigned int cnt = len * 4;
    unsigned int* vals = (unsigned int*)malloc(cnt * sizeof(unsigned int));

    AC_put_bits(word, vals, len);
    for(i=0; i<cnt; i++) {
        assert(node);
        ch = vals[i];
        printf("ch: %d\n", ch);
        if(node->children[ch] == NULL) {
            node->children[ch] = AC_New_Node(ch);
        }
        node = node->children[ch];
        if(i == cnt-1) {
            printf("now i: %d ch:%d\n", i, ch);
            node->flag = 1;
        }
    }
    //node->flag=1; //a word flag
    free(vals);
    return 0;
}

struct AC_Dict* AC_New_Dict(const char* path) {
    char                    utf8_line[256];
    unsigned int            unicode_line[256];
    size_t                  utf8_length;
    size_t                  unicode_length;
    struct AC_Dict* dict = NULL;

    size_t  path_length = 0;
    FILE* file;
    assert(path);

    path_length = strlen(path);
    if(path_length > 256) {
        fprintf(stderr, "[AC_Dict] : path %s is too long", path);
        return NULL;
    }
    file = fopen(path, "r");
    if(file == NULL) {
        fprintf(stderr, "[AC_Dict]: path %s can not opend", path);
        return NULL;
    }
    dict = (struct AC_Dict*)malloc(sizeof(struct AC_Dict));
    memset(dict, 0, sizeof(struct AC_Dict));
    dict->dict_path = strdup(path);
    dict->root = (struct AC_Node*)AC_New_Node(ROOT_CHAR);

    //add sensitive words into dict tree
    while((fgets(utf8_line, sizeof(utf8_line), file) != NULL)) {
        utf8_length = strlen(utf8_line);
        if(utf8_line > 0 && utf8_line[utf8_length-1] == '\n')
            utf8_length--;
        unicode_length = UTF8toUnicode(utf8_line, utf8_length, unicode_line, 256);
        AC_Add_Word(dict, unicode_line, unicode_length);
    }
    fclose(file);
    printf("build finished\n");
    return dict;
}

static void AC_Destory_Node(struct AC_Node* node) {
    int k;
    for(k=0; k<256; k++) {
        if(node->children[k]) {
            AC_Destory_Node(node->children[k]);
        }
    }
    free(node);
}

void AC_Destory_Dict(struct AC_Dict* dict) {
    AC_Destory_Node(dict->root);
    free(dict->dict_path);
    free(dict);
}

void AC_Shield_Word(struct AC_Dict* dict, char* str) {
    size_t src_len, unicode_len;
    unsigned int* unicode_buf;
    unsigned int i = 0, j, k;
    unsigned int bits_cnt;
    int head , tail;
    int is_matched = 0;
    struct AC_Node* node;
    assert(str && dict);

    src_len = strlen(str) + 1;
    unicode_buf = (unsigned int*)malloc(src_len * sizeof(unsigned int));
    unicode_len = UTF8toUnicode(str, src_len, unicode_buf, src_len);
    bits_cnt = unicode_len * 4;
    unsigned int* vals = (unsigned int*)malloc(sizeof(unsigned int) * bits_cnt);
    AC_put_bits(unicode_buf, vals, unicode_len);
    head = tail = -1;
    while( i < bits_cnt ) {
        node = dict->root;
        is_matched = 0;
        j = 0;
        head = i/4;
        while((node = node->children[vals[i+j]]) &&
              (i + j < bits_cnt)) {
            j++;
            tail = (i+j)/4;
            printf("i=%d i+j=%d\n",i, i+j);
            printf("vals:");
            for(k=0; k<i+j; k++) {
                printf(" %d", vals[k]);
            }
            printf("\n");
            if(node->flag == 1) {
                printf("head: %d tail: %d\n", head, tail);
                for(k=head; k<tail; k++)
                    unicode_buf[k] = SHIEL_REPLACE_WORD;
                is_matched = 1;
                break;
            }
        }
        if(is_matched)
            i += j;
        else
            i++;
    }
    UnicodetoUTF8(unicode_buf, unicode_len, str, src_len);
    free(vals);
    free(unicode_buf);
}

int main() {
    struct AC_Dict* dict = AC_New_Dict("sensitive_words.conf");
    char str[100];
    while(gets(str)) {
        printf("gots: %s\n", str);
        AC_Shield_Word(dict, str);
        printf("result: %s\n", str);
    }
}
