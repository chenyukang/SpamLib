#ifndef _AC_H_
#define _AC_H_
#include <stdlib.h>

struct AC_Node;
struct AC_Dict;

/* -------------------- API --------------------*/
struct AC_Dict* AC_New_Dict(const char* path);
void   AC_Destory_Dict(struct AC_Dict* dict);
void   AC_Shield_Word(struct AC_Dict* dict, char* str);
/* -------------------- API -------------------- */

/*  Test Uitl */
void   AC_Build_Automation(struct AC_Dict* dict);
int    AC_Add_Word(struct AC_Dict* dict, unsigned int* word, size_t len);
struct AC_Dict* AC_New_Empty_Dict();

#endif
