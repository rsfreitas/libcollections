
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

#include <Python.h>

#include "collections.h"

static PyObject *argument_object(cplugin_arg_t *acpl, const char *argument_name)
{
    cvalue_t *cplv = NULL;
    PyObject *v = NULL;
    char *s = NULL;

    cplv = cplugin_argument(acpl, argument_name);

    if (NULL == cplv)
        return Py_BuildValue("s", "null");

    switch (cvalue_type(cplv)) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            v = Py_BuildValue("b", CVALUE_AS_CHAR(cplv));
            break;

        case CL_UCHAR:
            v = Py_BuildValue("B", CVALUE_AS_UCHAR(cplv));
            break;

        case CL_INT:
            v = Py_BuildValue("i", CVALUE_AS_INT(cplv));
            break;

        case CL_UINT:
            v = Py_BuildValue("I", CVALUE_AS_UINT(cplv));
            break;

        case CL_SINT:
            v = Py_BuildValue("h", CVALUE_AS_SINT(cplv));
            break;

        case CL_USINT:
            v = Py_BuildValue("H", CVALUE_AS_USINT(cplv));
            break;

        case CL_FLOAT:
            v = Py_BuildValue("f", CVALUE_AS_FLOAT(cplv));
            break;

        case CL_DOUBLE:
            v = Py_BuildValue("d", CVALUE_AS_DOUBLE(cplv));
            break;

        case CL_LONG:
            v = Py_BuildValue("l", CVALUE_AS_LONG(cplv));
            break;

        case CL_ULONG:
            v = Py_BuildValue("k", CVALUE_AS_ULONG(cplv));
            break;

        case CL_LLONG:
            v = Py_BuildValue("L", CVALUE_AS_LLONG(cplv));
            break;

        case CL_ULLONG:
            v = Py_BuildValue("K", CVALUE_AS_ULLONG(cplv));
            break;

        /* XXX: We still can't receive arguments of these types. */
        case CL_POINTER:
        case CL_CSTRING:
            v = Py_BuildValue("s", "##unsupported argument type##");
            break;

        case CL_STRING:
            s = CVALUE_AS_STRING(cplv);
            v = Py_BuildValue("s", s);
            break;

        case CL_BOOLEAN:
            v = Py_BuildValue("i", CVALUE_AS_BOOLEAN(cplv));
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
    cstring_t *s = NULL;
    int ret = -1;

    switch (type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_CHAR, value[0]);

        case CL_UCHAR:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_UCHAR, (unsigned char)value[0]);

        case CL_INT:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_INT, strtol(value, NULL, 10));

        case CL_UINT:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_UINT, strtoul(value, NULL, 10));

        case CL_SINT:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_SINT,
                                            (short int)strtol(value, NULL, 10));

        case CL_USINT:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_USINT,
                                            (unsigned short int)strtoul(value,
                                                                        NULL,
                                                                        10));

        case CL_FLOAT:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_FLOAT, strtof(value, NULL));

        case CL_DOUBLE:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_DOUBLE, strtod(value, NULL));

        case CL_LONG:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_LONG, strtol(value, NULL, 10));

        case CL_ULONG:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_ULONG, strtoul(value, NULL, 10));

        case CL_LLONG:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_LLONG, strtoll(value, NULL, 10));

        case CL_ULLONG:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_ULLONG,
                                            strtoull(value, NULL, 10));

        /* unsupported */
        case CL_POINTER:
            break;

        case CL_STRING:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_STRING, value);

        case CL_CSTRING:
            s = cstring_create("%s", value);
            ret = cplugin_set_return_value(cpl, function_name, caller_id,
                                           CL_CSTRING, s);

            cstring_unref(s);
            return ret;

        case CL_BOOLEAN:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_BOOLEAN,
                                            strtol(value, NULL, 10));
    }

    return -1;
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

