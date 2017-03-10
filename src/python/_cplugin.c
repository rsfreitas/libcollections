
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

/*static PyObject *argument_object(cplugin_arg_t *acpl, const char *argument_name)
{
    cobject_t *cplv = NULL;
    PyObject *v = NULL;
    char *s = NULL;

    cplv = cplugin_argument(acpl, argument_name);

    if (NULL == cplv)
        return Py_BuildValue("s", "null");

    switch (cobject_type(cplv)) {
        case CL_VOID:*/
            /* noop */
/*            break;

        case CL_CHAR:
            v = Py_BuildValue("b", COBJECT_AS_CHAR(cplv));
            break;

        case CL_UCHAR:
            v = Py_BuildValue("B", COBJECT_AS_UCHAR(cplv));
            break;

        case CL_INT:
            v = Py_BuildValue("i", COBJECT_AS_INT(cplv));
            break;

        case CL_UINT:
            v = Py_BuildValue("I", COBJECT_AS_UINT(cplv));
            break;

        case CL_SINT:
            v = Py_BuildValue("h", COBJECT_AS_SINT(cplv));
            break;

        case CL_USINT:
            v = Py_BuildValue("H", COBJECT_AS_USINT(cplv));
            break;

        case CL_FLOAT:
            v = Py_BuildValue("f", COBJECT_AS_FLOAT(cplv));
            break;

        case CL_DOUBLE:
            v = Py_BuildValue("d", COBJECT_AS_DOUBLE(cplv));
            break;

        case CL_LONG:
            v = Py_BuildValue("l", COBJECT_AS_LONG(cplv));
            break;

        case CL_ULONG:
            v = Py_BuildValue("k", COBJECT_AS_ULONG(cplv));
            break;

        case CL_LLONG:
            v = Py_BuildValue("L", COBJECT_AS_LLONG(cplv));
            break;

        case CL_ULLONG:
            v = Py_BuildValue("K", COBJECT_AS_ULLONG(cplv));
            break;

        case CL_POINTER:*/
            /*
             * XXX: Is this the right way?
             */
//            v = COBJECT_AS_POINTER(cplv);

            /*
             * We increase the reference count of this pointer as this may be a
             * python object and we want to live longer.
             */
//            Py_INCREF(v);
//            break;

        /* XXX: We still can't receive arguments of this type. */
/*        case CL_CSTRING:
            v = Py_BuildValue("s", "##unsupported argument type##");
            break;

        case CL_STRING:
            s = COBJECT_AS_STRING(cplv);
            v = Py_BuildValue("s", s);
            free(s);
            break;

        case CL_BOOLEAN:
            v = Py_BuildValue("i", COBJECT_AS_BOOLEAN(cplv));
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
*/
/*static int set_real_return_value(cplugin_t *cpl, uint32_t caller_id,
    const char *function_name, enum cl_type type, const void *value)
{
    cstring_t *s = NULL;
    int ret = -1;
    char *c;

    switch (type) {
        case CL_VOID:*/
            /* noop */
/*            break;

        case CL_CHAR:
            c = (char *)value;
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_CHAR, c[0]);

        case CL_UCHAR:
            c = (char *)value;
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_UCHAR, (unsigned char)c[0]);

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

        case CL_POINTER:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_POINTER, (void *)value, 1);

        case CL_STRING:
            return cplugin_set_return_value(cpl, function_name, caller_id,
                                            CL_STRING, value);

        case CL_CSTRING:
            s = cstring_create("%s", (char *)value);
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
}*/

/*static PyObject *set_return_value(PyObject *self, PyObject *args)
{
    PyObject *pcpl;
    cplugin_t *cpl;
    uint32_t caller_id;
    char *function_name = NULL;
    int value_type;
    void *value = NULL;

    if (!PyArg_ParseTuple(args, "iOsis", &caller_id, &pcpl, &function_name,
                          &value_type, (char *)&value))
    {*/
        /* Try to get the argument as an object. */
/*        if (!PyArg_ParseTuple(args, "iOsiO", &caller_id, &pcpl, &function_name,
                              &value_type, &value))
        {
            return NULL;
        }
*/
        /*
         * Since we just received an object as an argument, we increase its
         * reference count, so we don't lose it.
         */
/*        Py_INCREF(value);
    }

    cpl = (cplugin_t *)PyCapsule_GetPointer(pcpl, PYCPLUGIN_T);

    return Py_BuildValue("i", set_real_return_value(cpl, caller_id,
                                                    function_name, value_type,
                                                    value));
}*/

/*static PyObject *release_argument(PyObject *self, PyObject *args)
{
}*/

static char module_docstring[] =
    "Extension used for internal manipulation of python plugin data "
    "through libcollections library.";

static PyMethodDef module_methods[] = {
//    { "argument",           argument,           METH_VARARGS, ""   },
//    { "arg_count",          arg_count,          METH_VARARGS, ""   },
//    { "set_return_value",   set_return_value,   METH_VARARGS, ""   },
//    { "release_argument",   release_argument,   METH_VARARGS, ""   },
    { NULL,                 NULL,               0,            NULL }
};

PyMODINIT_FUNC init_cplugin(void)
{
    PyObject *m;

    collections_init(NULL);
    m = Py_InitModule3("_cplugin", module_methods, module_docstring);

    if (NULL == m)
        return;

    Py_AtExit(collections_uninit);
}

