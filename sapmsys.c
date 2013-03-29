
/*******************************************************************************
 *
 *      sapmsys.c
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2013-03-29 15:14:20
 *
 *      COPYRIGHT (C) 2006~2012, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/

#include <Python.h>

#if 0
static PyObject*  
spam_system(PyObject* self, PyObject *args) {  
    const char *command;  
    int sts;  
    if (!PyArg_ParseTuple(args, "s", &command))  
        return NULL;  
    sts = system(command);  
    return Py_BuildValue("i", sts);  
}

static PyMethodDef libspam_methods[] = {
     {"spam_system", spam_system, METH_VARARGS, "Description"},
     {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initlibgongmingpyext() {     
    Py_InitModule3("libmypyext", libspam_methods, "A Python Spam Extension ");  
}  

#endif
