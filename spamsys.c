
/*******************************************************************************
 *
 *      sapmsys.c
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2013-03-29 15:14:20
 *
 *
 *******************************************************************************/

#include <Python.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"
#include "unicode.h"

/** 
 *   NOTICE:
 *   Trie tree width may be not enough while tree is used for Chinese dictionary,
 *   you should optimise this count by your actual usages. 
 **/
#define TRIE_TREE_WIDTH (100)
#define TRIE_TREE_DICT_PATH_LENGTH (256)
#define TRIE_TREE_ROOT_CHAR (0xFFFFFFFF)
#define TRIE_TREE_SHIELD_REPLACE_WORD (0x0000002A)

struct TrieTreeDict
{
    struct TrieTreeNode*    root_node;
    unsigned int            node_count;
    unsigned int            dict_size;
    FILE*                   dict_file;
    char                    dict_path[TRIE_TREE_DICT_PATH_LENGTH];
};

struct TrieTreeNode
{
    unsigned int        character;  /* dict characters discribed with Unicode */
    unsigned int        size;
    unsigned int        count;
    unsigned int        is_last_char;
    struct TrieTreeNode*  parent;
    struct TrieTreeNode** nexts;
};

struct TrieTreeNode* TRIE_New_Node(unsigned int ch);

int TRIE_Add_SubNode(struct TrieTreeNode* current,
                     struct TrieTreeNode* subnode);

int TRIE_Add_DictWord(struct TrieTreeDict* dict,
                      unsigned int* word, unsigned int wordlength);

struct TrieTreeDict* TRIE_New_Dict(const char* dict_path)
{
    struct TrieTreeDict*    dict = NULL;
    struct TrieTreeNode*    root = NULL;
    size_t                  path_length;
    FILE*                   file;
    char                    utf8_line[256];
    unsigned int            unicode_line[256];
    unsigned int            utf8_length;
    unsigned int            unicode_length;

    assert(dict_path);
    if (dict_path == NULL) return NULL;

    dict = (struct TrieTreeDict*)malloc(sizeof(struct TrieTreeDict));
    if (dict == NULL) 
    {
        fputs("[Trie]New dict failed, cannot malloc dict!\n", stderr);
        return NULL;
    }
    
    memset(dict, 0, sizeof(struct TrieTreeDict));

    path_length = strlen(dict_path);
    if (path_length >= TRIE_TREE_DICT_PATH_LENGTH) return NULL;

    file = fopen(dict_path, "r"); 
    if (file == NULL)
    {
        fputs("[Trie]Init dict failed, cannot open dict file!\n", stderr);
        return NULL;
    }

    strcpy(dict->dict_path, dict_path);
    dict->dict_file = file;

    // add root node into tree
    root = TRIE_New_Node(TRIE_TREE_ROOT_CHAR);
    dict->root_node = root;

    // add dict word nodes into tree
    while (fgets(utf8_line, sizeof(utf8_line), file) != NULL)
    {
        utf8_length = strlen(utf8_line);

        // excepts the new-line character
        if (utf8_length > 0 && utf8_line[utf8_length - 1] == '\n')
            utf8_length--;

        unicode_length = UTF8toUnicode(utf8_line, utf8_length, unicode_line, 256);
        
        TRIE_Add_DictWord(dict, unicode_line, unicode_length);
    }

    return dict;
}
    
struct TrieTreeNode* TRIE_New_Node(unsigned int ch)
{
    void *n;
    struct TrieTreeNode *p;
        
    p = malloc(sizeof(struct TrieTreeNode));

    if (p == NULL)
    {
        fputs("[Trie]New trie-tree node failed, cannot malloc tree node!\n", stderr);
        return NULL;
    }

    n = malloc(sizeof(struct TrieTreeNode*) * TRIE_TREE_WIDTH);
    if (n == NULL)
    {
        fputs("[Trie]New trie-tree node failed, cannot malloc nexts-field!\n", stderr);
        free(p);
        return NULL;
    }

    memset(p, 0, sizeof(struct TrieTreeNode));
    p->character = ch;
    p->size = TRIE_TREE_WIDTH;
    p->count = 0;
    p->nexts = n;

    return p;
}

int TRIE_Add_DictWord(struct TrieTreeDict* dict,
                      unsigned int* word, unsigned int wordlength)
{
    struct TrieTreeNode* node;
    struct TrieTreeNode* t;
    unsigned int i, j;
    unsigned int ch;

    assert(dict && word);

    t = dict->root_node;
    
    for (i = 0; i < wordlength; ++i)
    {
        assert(t);
        ch = word[i];
        
        for (j = 0; j < t->count; ++j)
        {
            if (ch == t->nexts[j]->character)
                break;
        }

        // add new sub node
        if (j == t->count)
        {
            node = TRIE_New_Node(ch);
            if (node == NULL)
            {
                return -1;
            }

            if (TRIE_Add_SubNode(t, node))
            {
                return -1;
            }

            t = node;
        }
        else
        {
            t = t->nexts[j];
        }

        if (i == wordlength - 1)
            t->is_last_char = 1;
    }

    return 0;
}

int TRIE_Add_SubNode(struct TrieTreeNode* current,
                     struct TrieTreeNode* subnode)
{
    unsigned int resize;
    void *newnexts;
    assert(current && subnode);
    assert(current->count <= current->size);

    if (current->count == current->size)
    {
        //extends memory
        resize = current->size + TRIE_TREE_WIDTH;
        newnexts = realloc(current->nexts, resize * sizeof(struct TrieTreeNode*));
        if (newnexts == NULL)
        {
            fputs("[Trie]Extend trie-tree node failed, cannot realloc tree node!\n", stderr);
            fputs("[Trie]Add subnode failed, cannot extend trie-tree node width!\n", stderr);
            return -1;
        }
        current->size  = resize;
        current->nexts = (struct TrieTreeNode **)newnexts;
    }

    assert(current->count < current->size);

    current->nexts[current->count] = subnode;
    current->count++;
    subnode->parent = current;

    return 0;
}

void TRIE_PostOrder_Traversal(struct TrieTreeNode *node,
                              FUNC_PTR_TRAVERSAL_VISIT visit)
{
    unsigned int i;

    if (node == NULL) return;

    for (i = 0; i < node->count; ++i)
        TRIE_PostOrder_Traversal(node->nexts[i], visit);

    visit(node);
}

void TRIE_Print_Word(struct TrieTreeNode *node)
{
    char unicode_print[256] = {0};
    char unicode_tempword[256];
    char utf8_print[256] = {0};
    char utf8_tempword[256];
    char conv_2_utf8[256];
    struct TrieTreeNode* p;
    unsigned int utf8_length;
    assert(node);

    // root node
    if (node->parent == NULL)
    {
        return;
    }

    if (node->is_last_char == 1)
    {
        printf("[Trie][Node] ");

        sprintf(unicode_print, "%08x ", node->character);

        utf8_length = UnicodetoUTF8(&node->character, 1, conv_2_utf8, 256);
        conv_2_utf8[utf8_length] = '\0';
        sprintf(utf8_print, "%s", conv_2_utf8);

        p = node->parent;
        while (p && p->parent)
        {
            sprintf(unicode_tempword, "%08x ", p->character);
            strcat(unicode_tempword, unicode_print);
            strcpy(unicode_print, unicode_tempword);

            utf8_length = UnicodetoUTF8(&p->character, 1, conv_2_utf8, 256);
            conv_2_utf8[utf8_length] = '\0';
            sprintf(utf8_tempword, "%s", conv_2_utf8);
            strcat(utf8_tempword, utf8_print);
            strcpy(utf8_print, utf8_tempword);

            p = p->parent;
        }

        printf("[utf8|%s] [unicode|%s] \n", utf8_print, unicode_print);
    }
}

void TRIE_Traversal(struct TrieTreeDict* dict,
                    FUNC_PTR_TRAVERSAL_VISIT visit)
{
    TRIE_PostOrder_Traversal(dict->root_node, visit);
}

void TRIE_Destroy_Node(struct TrieTreeNode* node)
{
    assert(node && node->nexts);
    free(node->nexts);
    free(node);
}

void TRIE_Destroy_Dict(struct TrieTreeDict* dict)
{
    TRIE_PostOrder_Traversal(dict->root_node, TRIE_Destroy_Node);
    free(dict);
}

struct TrieTreeNode* TRIE_NodehasNextChar(struct TrieTreeNode* node,
                                          int ch)
{
    unsigned int i;
    assert(node);
    
    for (i = 0; i < node->count; ++i)
    {
        if (node->nexts[i]->character == ch)
            return node->nexts[i];
    }

    return NULL;
}

void TRIE_Shield_Words(struct TrieTreeDict* dict,
                       char*  src_string)
{
    unsigned int* unicode_buffer;
    unsigned int  src_length;
    unsigned int  buffer_length;
    unsigned int  i = 0;
    unsigned int  j, k;
    unsigned int  is_matched;
    struct TrieTreeNode* t = NULL;
    struct TrieTreeNode* n = NULL;

    assert(dict && src_string);

    // include '\0' character to shield, so return buffer will become a string automaticly
    src_length = strlen(src_string) + 1;

    unicode_buffer = malloc(src_length * sizeof(int));
    if (unicode_buffer == NULL)
    {
        fputs("[Trie] Shield words failed, can not malloc unicode buffer!\n", stderr);
        return;
    }

    buffer_length = UTF8toUnicode(src_string, src_length, unicode_buffer, src_length);

    while (i < buffer_length)
    {
        // start new match
        t = dict->root_node;
        assert(t);
        is_matched = 0;

        j = 0;
        while ( (n = TRIE_NodehasNextChar(t, unicode_buffer[i+j]))
                && (i + j < buffer_length) )
        {
            j++;

            // has matched a word
            if (n->is_last_char)
            {
                for (k = 0; k < j; ++k)
                    unicode_buffer[i+k] = TRIE_TREE_SHIELD_REPLACE_WORD;
                
                is_matched = 1;
                break;
            }

            t = n;
        }                

        if (is_matched)
        {
            i += j;
        }
        else
        {
            i++;
        }
    } 

    UnicodetoUTF8(unicode_buffer, buffer_length, src_string, src_length);
    
    free(unicode_buffer);
}


static struct TrieTreeDict* dict = NULL;

static PyObject*  
spam_check(PyObject* self, PyObject* args) {  
    const char* src;
    if (!PyArg_ParseTuple(args, "s", &src))  
        return NULL;
    if(dict == NULL) {
        dict = TRIE_New_Dict("sensitive_words.conf");
    }
    assert(dict);
    char* words = strdup(src);
    TRIE_Shield_Words(dict, words);
    printf("result: %s\n", words);
    return Py_BuildValue("s", words);
}

static PyObject*
spam_init(PyObject* self, PyObject* args) {
    const char* filename;
    if(!PyArg_ParseTuple(args, "s", &filename))
        return NULL;
    dict = TRIE_New_Dict(filename);
    return Py_BuildValue("i", 1);
}

static PyMethodDef libspam_methods[] =  {  
    {"spam_check", spam_check, METH_VARARGS, "spam check"},
    {"spam_init", spam_init, METH_VARARGS, "spam init"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initlibSpamSys(void) {
    Py_InitModule3("libSpamSys", libspam_methods, "A Python Spam Extension ");  
}  
