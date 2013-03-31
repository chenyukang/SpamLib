#include <Python.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"

const  char* default_conf = "sensitive_words.conf";
static struct AC_Dict* dict = NULL;
static PyObject* WordFilter_Error = NULL;

static PyObject*  
filter_check(PyObject* self, PyObject* args) {  
    const char* src;
    char* buf;
    PyObject* res;
    
    if(dict == NULL) {
        PyErr_SetString(WordFilter_Error, "create dict failed");
        Py_INCREF(Py_None);
        return Py_None;
    }
    if (!PyArg_ParseTuple(args, "s", &src)) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    assert(dict);
    buf = strdup(src);
    AC_Shield_Word(dict, buf);
    res = Py_BuildValue("s", buf);
    free(buf);
    return res;
}

static PyObject*
filter_init(PyObject* self, PyObject* args) {
    const char* filename;
    struct AC_Dict* back = dict;
    if(!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    dict = AC_New_Dict(filename);
    if(dict == 0) {
        PyErr_SetString(WordFilter_Error, "create dict failed");
        Py_INCREF(Py_None);
        dict = back;
        return Py_None;
    }
    AC_Destory_Dict(back);
    return Py_BuildValue("i", 1);
}

static PyMethodDef libwordfilter_methods[] =  {  
    {"check", filter_check, METH_VARARGS, "word filter check"},
    {"init",  filter_init,  METH_VARARGS, "word filter init with config"},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initlibWordFilter(void) {
    PyObject* m;
    dict = AC_New_Dict(default_conf);
    if(dict == NULL) {
        return;
    }
    m = Py_InitModule3("libWordFilter", libwordfilter_methods, "A Python word filter Extension ");
    WordFilter_Error = PyErr_NewException((char*)"wordfilter.error", NULL, NULL);
    Py_INCREF(WordFilter_Error);
    PyModule_AddObject(m, "error", WordFilter_Error);
}
