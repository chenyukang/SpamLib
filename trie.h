
/*******************************************************************************
 *
 *      KINGTRIE_H
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2013-03-29 18:14:46
 *
 *******************************************************************************/

#if !defined(KINGTRIE_H)
#define KINGTRIE_H


#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
    
    struct TrieTreeDict;
    struct TrieTreeNode;

    struct TrieTreeDict* 
    TRIE_New_Dict(const char* path);

    void 
    TRIE_Destroy_Dict(struct TrieTreeDict* dict);

    typedef void (*FUNC_PTR_TRAVERSAL_VISIT)(struct TrieTreeNode *);

    void 
    TRIE_Traversal(struct TrieTreeDict *dict, FUNC_PTR_TRAVERSAL_VISIT visit);

    void 
    TRIE_Print_Word(struct TrieTreeNode *node);

    void
    TRIE_Shield_Words(struct TrieTreeDict* dict, char*  str);

#ifdef __cplusplus
}
#endif

#endif

