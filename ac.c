#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ac.h"
#include "unicode.h"

#define ROOT_CHAR    (0xFFFFFFFF)
#define AC_CHILD_NUM (100)
#define SHIEL_REPLACE_WORD (0x0000002A)
#define USE_AUTOMATION 1

struct AC_Node* AC_New_Node(unsigned int value) {
    struct AC_Node* node = (struct AC_Node*)malloc(sizeof(struct AC_Node));
    assert(node);
    memset(node, 0, sizeof(struct AC_Node));
    node->value = value;
    node->count = 0;
    node->size = AC_CHILD_NUM;
    node->children = (struct AC_Node**)malloc(sizeof(struct AC_Node*) * AC_CHILD_NUM);
    memset(node->children, 0, sizeof(struct AC_Node*) * AC_CHILD_NUM);
    return node;
}

//keep children in order, so query is more fast
static int AC_Add_SubNode(struct AC_Node* parent,
                          struct AC_Node* child) {
    assert(parent && child);
    assert(parent->count <= parent->size);
    size_t size = 0;
    size_t i, k;
    if(parent->count == parent->size) {
        size = parent->size + AC_CHILD_NUM;
        parent->children = (struct AC_Node**)realloc(parent->children, size * sizeof(struct AC_Node*));
        parent->size = size;
        if(parent->children == NULL) {
            fprintf(stderr, "[AC_Dict]: realloc failed");
            return -1;
        }
    }
    //find the pos to insert
    for(i=0; i<parent->count; i++) {
        if(parent->children[i]->value > child->value)
            break;
    }
    if(i == parent->count)  //append to last pos
        parent->children[parent->count++] = child;
    else {
        for(k=parent->count; k>i; k--){
            parent->children[k] = parent->children[k-1];
        }
        parent->children[i] = child;
        parent->count++;
    }
    child->depth = parent->depth+1;
    return 0;
}

//Binary search
struct AC_Node* AC_has_child(struct AC_Node* node, unsigned int val) {
    int mid, left, right;
    struct AC_Node** p = node->children;
    if(node->count == 0) return NULL;
    left = 0, right = node->count-1;
    while(left <= right) {
        mid = (left + right)>>1;
        if(p[mid]->value == val) return p[mid];
        else if(p[mid]->value < val) left = mid+1;
        else if(p[mid]->value > val) right = mid-1;
    }
    return NULL;
}

void AC_verify(struct AC_Node* node) {
    if(node->count != 0) {
        int k;
        for(k=0; k<node->count-1; k++) {
            if(!(node->children[k]->value < node->children[k+1]->value)) {
                printf(" %d <- %d\n", node->children[k]->value,
                       node->children[k+1]->value);
            }
            AC_verify(node->children[k]);
        }
    }
}

int AC_Add_Word(struct AC_Dict* dict, char* word, size_t len) {
    assert(dict && word);
    struct AC_Node* node = dict->root;
    struct AC_Node* child = NULL;
    struct AC_Node* new = NULL;
    int i, k;
    unsigned int ch;
    size_t unicode_len;
    unsigned int unicode_buf[256];
    unicode_len = UTF8toUnicode(word, len, unicode_buf, 256);
    for(i=0; i<unicode_len; i++) {
        assert(node);
        ch = unicode_buf[i];
        for(k=0; k<node->count; k++) {
            if( ch == node->children[k]->value ) break;
        }
        if((child = AC_has_child(node, ch)) == NULL) { //no found , add child
            new = AC_New_Node(ch);
            AC_Add_SubNode(node, new);
            node = new;
        } else {
            node = child;
        }
    }
    node->flag++; //a word node 
    return 0;
}

void AC_Build_Automation(struct AC_Dict* dict) {
    struct AC_Node* root = dict->root;
    struct AC_Node* node;
    struct AC_Node* pfail;
    struct AC_Node* child;
    struct AC_Node* Q[50000]; //NOTE size
    unsigned int v;
    int head, tail, i;
    
    root->fail = NULL;
    memset(Q, 0, sizeof(Q[0]) * 50000);
    head = tail = 0;
    Q[head++] = root;
    
    while(tail != head) {
        node = Q[tail++];
        for(i=0; i<node->count; i++) {
            if(node == root)
                node->children[i]->fail = root;
            else {
                v = node->children[i]->value;
                pfail = node->fail;
                while(pfail != NULL) {
                    if((child = AC_has_child(pfail, v)) != NULL)  {
                        node->children[i]->fail = child;
                        break;
                    }
                    pfail = pfail->fail;
                }
                if(pfail == NULL)
                    node->children[i]->fail = root;
            }
            Q[head++] = node->children[i];
        }
    }
}

static struct AC_Dict* AC_New_Empty_Dict(void) {
    struct AC_Dict* dict = (struct AC_Dict*)malloc(sizeof(struct AC_Dict));
    memset(dict, 0, sizeof(struct AC_Dict));
    dict->root = (struct AC_Node*)AC_New_Node(ROOT_CHAR);
    return dict;
}

struct AC_Dict* AC_New_Dict(const char* path) {
    char                    utf8_line[256];
    size_t                  utf8_length;
    struct AC_Dict* dict = NULL;

    size_t  path_length = 0;
    FILE* file;

    dict = AC_New_Empty_Dict();
    if(path == 0) { //mainly for testing
        return dict;
    }

    dict->dict_path = strdup(path);
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

    //add sensitive words into dict tree
    while((fgets(utf8_line, sizeof(utf8_line), file) != NULL)) {
        utf8_length = strlen(utf8_line);
        if(utf8_line > 0 && utf8_line[utf8_length-1] == '\n')
            utf8_length--;
         AC_Add_Word(dict, utf8_line, utf8_length);
    }
    fclose(file);
#ifdef USE_AUTOMATION
    AC_Build_Automation(dict);
#endif
    AC_verify(dict->root);
    printf("build finished\n");
    return dict;
}

#ifdef USE_AUTOMATION
void AC_Shield_Word(struct AC_Dict* dict, char* str) {
    size_t src_len, unicode_len;
    unsigned int* unicode_buf;
    unsigned int i = 0, k;
    unsigned int val;
    struct AC_Node* node;
    struct AC_Node* next;
    assert(str && dict);

    src_len = strlen(str) + 1;
    unicode_buf = (unsigned int*)malloc(src_len * sizeof(unsigned int));
    unicode_len = UTF8toUnicode(str, src_len, unicode_buf, src_len);
    node = dict->root;
    while( i < unicode_len ) {
        val = unicode_buf[i];
        while(node != dict->root && (next = AC_has_child(node, val)) == NULL)
            node = node->fail;
        next = AC_has_child(node, val);
        node = (next == NULL)? dict->root : next;
        if(node->flag != 0) {
            for(k=i-node->depth+1; k<=i; k++) {
                 unicode_buf[k] = SHIEL_REPLACE_WORD;
            }
        }
        i++;
    }
    UnicodetoUTF8(unicode_buf, unicode_len, str, src_len);
    free(unicode_buf);
}

#else
void AC_Shield_Word(struct AC_Dict* dict, char* str) {
    size_t src_len, unicode_len;
    unsigned int* unicode_buf;
    unsigned int i = 0, j, k;
    int is_matched = 0;
    struct AC_Node* node;
    struct AC_Node* next;
    assert(str && dict);

    src_len = strlen(str) + 1;
    unicode_buf = (unsigned int*)malloc(src_len * sizeof(unsigned int));
    unicode_len = UTF8toUnicode(str, src_len, unicode_buf, src_len);
    while( i < unicode_len ) {
        node = dict->root;
        is_matched = 0;
        j = 0;
        while((next = AC_has_child(node, unicode_buf[i+j])) &&
              (i + j < unicode_len)) {
            j++;
            if(next->flag != 0) {
                for(k=0; k<j; k++) 
                    unicode_buf[i+k] = SHIEL_REPLACE_WORD;
                is_matched = 1;
                break;
            }
            node = next;
        }
        if(is_matched)
            i += j;
        else
            i++;
    }
    UnicodetoUTF8(unicode_buf, unicode_len, str, src_len);
    free(unicode_buf);
}
#endif

static void AC_Destory_Node(struct AC_Node* node) {
    int k;
    for(k=0; k<node->count; k++) {
        if(node->children[k]) {
            AC_Destory_Node(node->children[k]);
        }
    }
    free(node);
}


void AC_Destory_Dict(struct AC_Dict* dict) {
    AC_Destory_Node(dict->root);
    if(dict->dict_path)
        free(dict->dict_path);
    free(dict);
}

