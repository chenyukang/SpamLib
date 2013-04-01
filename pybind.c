#include <Python.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"
#include "structmember.h"

const  char* default_conf = "sensitive_words.conf";

staticforward PyTypeObject AC_DictType;

typedef struct {
    PyObject_HEAD
    int count;
    struct AC_Dict* ac_dict;
}AC_Dict_Obj;

static PyMemberDef AC_Dict_members[] = {
	{NULL}			                /* sentinel */
};


static PyObject*
AC_Dict_new(PyTypeObject* type,PyObject* args, PyObject* kwds) {
    const char* path;
	AC_Dict_Obj* self = (AC_Dict_Obj*) type->tp_alloc(type, 0);
	if (self != NULL) {
        if(args == Py_None || (!PyArg_ParseTuple(args, "s", &path))) {
            Py_DECREF(self);
            return NULL;
        }
		self->ac_dict = AC_New_Dict(path);
		if (self->ac_dict == NULL) {
            Py_INCREF(Py_None);
			return Py_None;
		}
		self->count = 0;
	}
	return (PyObject*) self;
}

/* Deallocates the memory used in allocating this thing. */
static void
AC_Dict_dealloc(AC_Dict_Obj *self) {
	AC_Destory_Dict(self->ac_dict);
	self->ob_type->tp_free((PyObject*) self);
}

static PyObject*
AC_Dict_check(AC_Dict_Obj* self, PyObject *args) {
    const char* src;
    char* buf;
    PyObject* res;
    if(self->ac_dict != NULL) {
        if(!PyArg_ParseTuple(args, "s", &src)) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        buf = strdup(src);
        int v = AC_Shield_Word(self->ac_dict, buf);

        if(v == -1)
            res = Py_BuildValue("s", src);
        else
            res = Py_BuildValue("s", buf);
        free(buf);
        return res;
    }
    Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef AC_Dict_methods[] = {
	{"check", (PyCFunction) AC_Dict_check, METH_VARARGS, "check a string" },
	{NULL}			/* sentinel */
};


/* Let's define the type table stuff. */
static PyTypeObject AC_DictType = {
	PyObject_HEAD_INIT(NULL)
	0,                                       /*ob_size*/
	"ac_dict.AC_Dict",                       /*tp_name*/
	sizeof(AC_Dict_Obj),                     /*tp_basicsize*/
	0,                                       /*tp_itemsize*/
	(destructor)AC_Dict_dealloc,             /*tp_dealloc*/
	0,                                       /*tp_print*/
	0,                                       /*tp_getattr*/
	0,                                       /*tp_setattr*/
	0,                                       /*tp_compare*/
	0,                                       /*tp_repr*/
	0,                                       /*tp_as_number*/
	0,                                       /*tp_as_sequence*/
	0,                                       /*tp_as_mapping*/
	0,                                       /*tp_hash */
	0,                                       /*tp_call*/
	0,                                       /*tp_str*/
	0,                                       /*tp_getattro*/
	0,                                       /*tp_setattro*/
	0,                                       /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,/*tp_flags*/
	"AC_Dict objects",                       /* tp_doc */
	0,                                       /* tp_traverse */
	0,                                       /* tp_clear */
	0,                                       /* tp_richcompare */
	0,                                       /* tp_weaklistoffset */
	0,                                       /* tp_iter */
	0,                                       /* tp_iternext */
	AC_Dict_methods,                         /* tp_methods */
	AC_Dict_members,                         /* tp_members */
	0,                                       /* tp_getset */
	0,                                       /* tp_base */
	0,                                       /* tp_dict */
	0,                                       /* tp_descr_get */
	0,                                       /* tp_descr_set */
	0,                                       /* tp_dictoffset */
	0,                                       /* tp_init */
	0,                                       /* tp_alloc */
	AC_Dict_new,                             /* tp_new */
};


static PyMethodDef module_methods[] = {
	{NULL, NULL, 0, NULL}		             /* sentinel */
};

#ifndef PyMODINIT_FUNC 		                 /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initac_dict(void) {
	PyObject* m;
	if (PyType_Ready(&AC_DictType) < 0) {
		return;
    }

	m = Py_InitModule3("ac_dict", module_methods, "ACdict module");

	if (m == NULL){
		return;
    }
	Py_INCREF(&AC_DictType);
	PyModule_AddObject(m, "AC_Dict", (PyObject*)&AC_DictType);
}

