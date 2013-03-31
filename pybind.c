#include <Python.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"

static struct AC_Dict* dict = NULL;

static PyObject*  
filter_check(PyObject* self, PyObject* args) {  
    const char* src;
    if (!PyArg_ParseTuple(args, "s", &src))  
        return NULL;
    printf("now got: %s\n", src);
    if(dict == NULL) {
        dict = AC_New_Dict("sensitive_words.conf");
    }
    assert(dict);
    char* words = strdup(src);
    AC_Shield_Word(dict, words);
    printf("result: %s\n", words);
    return Py_BuildValue("s", words);
}

static PyObject*
filter_init(PyObject* self, PyObject* args) {
    const char* filename;
    if(!PyArg_ParseTuple(args, "s", &filename))
        return NULL;
    if(dict != NULL) {
        AC_Destory_Dict(dict);
    }
    dict = AC_New_Dict(filename);
    return Py_BuildValue("i", 1);
}

static PyMethodDef libwordfilter_methods[] =  {  
    {"filter_check", filter_check, METH_VARARGS, "word filter check"},
    {"filter_init", filter_init, METH_VARARGS, "word filter init with config"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initlibWordFilter(void) {
    Py_InitModule3("libWordFilter", libwordfilter_methods, "A Python word filter Extension ");  
}  
