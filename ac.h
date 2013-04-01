#ifndef _AC_H_
#define _AC_H_
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif
    
struct AC_Node {
    struct AC_Node* fail;
    struct AC_Node** children;
    size_t size;
    unsigned int count;
    unsigned int value;
    unsigned int flag;
    unsigned int depth;
};
    
struct AC_Dict {
    char* dict_path;
    struct AC_Node* root;
};

/* -------------------- API --------------------*/
struct AC_Dict* AC_New_Dict(const char* path);
void   AC_Destory_Dict(struct AC_Dict* dict);
int    AC_Init_Dict(struct AC_Dict* dict);
int    AC_Shield_Word(struct AC_Dict* dict, char* str);
/* -------------------- API -------------------- */

/*  for Test  */
void   AC_Build_Automation(struct AC_Dict* dict);
int    AC_Add_Word(struct AC_Dict* dict, char* word, size_t len);

#ifdef __cplusplus
}
#endif

#endif
