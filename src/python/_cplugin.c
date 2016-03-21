
/*
 * Description: Python API to be used by a python extension.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:34:01 BRST 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

/* TODO: finish argument different values support */

#include <Python.h>

#include "collections.h"

/* Keys to access PyObject encapsulated info */
#define PYARGS                              "pyargs"
#define PYCPLUGIN_T                         "pycplugin_t"

static PyObject *argument_object(cplugin_arg_t *acpl, const char *argument_name)
{
    cvalue_t *cplv = NULL;
    PyObject *v = NULL;

    cplv = cplugin_argument(acpl, argument_name);

    if (NULL == cplv)
        return NULL; /* FIXME */

    switch (cvalue_type(cplv)) {
        case CL_VOID:
            break;

        case CL_CHAR:
            break;

        case CL_UCHAR:
            break;

        case CL_INT:
            v = Py_BuildValue("i", CVALUE_INT(cplv));
            break;

        case CL_UINT:
            break;

        case CL_SINT:
            break;

        case CL_USINT:
            break;

        case CL_FLOAT:
            break;

        case CL_DOUBLE:
            break;

        case CL_LONG:
            break;

        case CL_ULONG:
            break;

        case CL_LLONG:
            break;

        case CL_ULLONG:
            break;

        case CL_POINTER:
            break;

        case CL_STRING:
            break;

        case CL_BOOLEAN:
            break;
    }

    return v;
}

static PyObject *argument(PyObject *self, PyObject *args)
{
    PyObject *pcpl;
    cplugin_arg_t *acpl;
    const char *aname;

    if (!PyArg_ParseTuple(args, "Os", &pcpl, &aname))
        return NULL;

    acpl = (cplugin_arg_t *)PyCapsule_GetPointer(pcpl, PYARGS);

    return argument_object(acpl, aname);
}

static PyObject *arg_count(PyObject *self, PyObject *args)
{
    PyObject *pcpl;
    cplugin_arg_t *acpl;

    if (!PyArg_ParseTuple(args, "O", &pcpl))
        return NULL;

    acpl = (cplugin_arg_t *)PyCapsule_GetPointer(pcpl, PYARGS);

    return Py_BuildValue("i", cplugin_arg_count(acpl));
}

static int set_real_return_value(cplugin_t *cpl, uint32_t caller_id,
    const char *function_name, enum cl_type type, const char *value)
{
    int iv;
    char cv;
    float fv;

    switch (type) {
        case CL_INT:
            iv = strtol(value, NULL, 10);
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_INT, iv);

        case CL_CHAR:
            cv = value[0];
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_CHAR, cv);

        /* TODO: String */
        case CL_FLOAT:
            fv = strtof(value, NULL);
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_FLOAT, fv);

        default:
            return -1;
    }
}

static PyObject *set_return_value(PyObject *self, PyObject *args)
{
    PyObject *pcpl;
    cplugin_t *cpl;
    uint32_t caller_id;
    char *function_name = NULL, *value = NULL;
    int value_type;

    if (!PyArg_ParseTuple(args, "iOsis", &caller_id, &pcpl, &function_name,
                          &value_type, &value))
    {
        return NULL;
    }

    cpl = (cplugin_t *)PyCapsule_GetPointer(pcpl, PYCPLUGIN_T);

    return Py_BuildValue("i", set_real_return_value(cpl, caller_id,
                                                    function_name, value_type,
                                                    value));
}

static char module_docstring[] =
    "Extension used for internal manipulation of python plugin data "
    "through libcollections library.";

static PyMethodDef module_methods[] = {
    { "argument",           argument,           METH_VARARGS, ""   },
    { "arg_count",          arg_count,          METH_VARARGS, ""   },
    { "set_return_value",   set_return_value,   METH_VARARGS, ""   },
    { NULL,                 NULL,               0,            NULL }
};

PyMODINIT_FUNC init_cplugin(void)
{
    PyObject *m;

    m = Py_InitModule3("_cplugin", module_methods, module_docstring);

    if (NULL == m)
        return;
}

