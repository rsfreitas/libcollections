
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:34:01 BRST 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * FIXME: Lots of memory leak reported by valgrind.
 */

#include <Python.h>

#include <libgen.h>

#include "collections.h"
#include "plugin.h"

/* Structure to save custom plugin information. */
struct py_info {
    char    *startup_name;
    char    *shutdown_name;
};

static void set_custom_plugin_info(cl_plugin_info_t *info, const char *startup,
    const char *shutdown)
{
    struct py_info *p;

    p = calloc(1, sizeof(struct py_info));

    if (NULL == p)
        return;

    p->startup_name = strdup(startup);
    p->shutdown_name = strdup(shutdown);

    info_set_custom_data(info, p);
}

static void release_custom_plugin_info(struct py_info *info)
{
    if (NULL == info)
        return;

    free(info->shutdown_name);
    free(info->startup_name);
    free(info);
}

static int py_load_function(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    PyObject *dict = (PyObject *)b;

    foo->symbol = PyDict_GetItemString(dict, (char *)foo->name);

    if (PyCallable_Check(foo->symbol))
        return 0;

    return -1;
}

static int py_unload_function(void *a, void *b __attribute__((unused)))
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;

    Py_DECREF(foo->symbol);

    return 0;
}

/*
 *
 * Plugin Driver API
 *
 */

static void add_preconfigured_paths(void)
{
    cl_json_t *paths = NULL, *root, *p;
    int i, t;
    char *tmp;
    cl_string_t *s;

    root = library_configuration();

    if (NULL == root)
        return;

    paths = cl_json_get_object_item(root, "plugin_path");

    if (NULL == paths)
        return;

    t = cl_json_get_array_size(paths);
    PyRun_SimpleString("import sys");

    for (i = 0; i < t; i++) {
        p = cl_json_get_array_item(paths, i);
        s = cl_json_get_object_value(p);

        if (asprintf(&tmp, "sys.path.append(%s)", cl_string_valueof(s)) == -1)
            return;

        PyRun_SimpleString(tmp);
        free(tmp);
    }
}

void *py_library_init(void)
{
    setenv("PYTHONPATH", "/usr/local/lib", 1);
    Py_Initialize();
    PyEval_InitThreads();
    PyEval_ReleaseLock();
    add_preconfigured_paths();

    return NULL;
}

void py_library_uninit(void *data __attribute__((unused)))
{
    PyEval_AcquireLock();
    /* FIXME: We're having a segfault here on multi-thread applications */
//    Py_Finalize();
}

/*
 * Loads information from within 'CpluginMainEntry' class.
 */
cl_plugin_info_t *py_load_info(void *data __attribute__((unused)), void *ptr)
{
    PyObject *dict = (PyObject *)ptr;
    PyObject *class = NULL, *instance = NULL, *result = NULL;
    unsigned int i = 0, t = 0;
    cl_plugin_info_t *info = NULL;
    PyGILState_STATE gstate;
    struct plugin_internal_function pyinfo[] = {
        /*
         * These names are based upon the method names from the CpluginEntryAPI
         * class, inside the cplugin.py file.
         */
        { "get_name",           NULL },
        { "get_version",        NULL },
        { "get_author",         NULL },
        { "get_description",    NULL },
        { "get_startup",        NULL },
        { "get_shutdown",       NULL }
    };

    gstate = PyGILState_Ensure();
    class = PyDict_GetItemString(dict, "CpluginMainEntry");

    if (NULL == class)
        return NULL;

    if (!PyCallable_Check(class))
        return NULL;
    else
        instance = PyObject_CallObject(class, NULL);

    if (NULL == instance)
        return NULL;

    /* Call methods from the class so we can get more information. */
    t = sizeof(pyinfo) / sizeof(pyinfo[0]);

    for (i = 0; i < t; i++) {
        result = PyObject_CallMethod(instance, pyinfo[i].name, NULL);

        if (NULL == result)
            goto end_block;

        pyinfo[i].return_value = strdup(PyString_AS_STRING(result));
        Py_DECREF(result);
    }

    info = info_create_from_data(pyinfo[0].return_value,
                                 pyinfo[1].return_value,
                                 pyinfo[2].return_value,
                                 pyinfo[3].return_value);

    if (info != NULL)
        set_custom_plugin_info(info, pyinfo[5].return_value,
                               pyinfo[6].return_value);

    Py_DECREF(instance);

    for (i = 0; i < t; i++)
        free(pyinfo[i].return_value);

end_block:
    PyGILState_Release(gstate);
    return info;
}

int py_load_functions(void *data __attribute__((unused)),
    struct cplugin_function_s *flist, void *handle)
{
    if (cl_dll_map(flist, py_load_function, handle) != NULL)
        return -1;

    return 0;
}

void py_unload_functions(void *data __attribute__((unused)),
    struct cplugin_function_s *flist, void *handle __attribute__((unused)))
{
    cl_dll_map(flist, py_unload_function, NULL);
}

void *py_open(void *data __attribute__((unused)), const char *pathname)
{
    PyObject *handle = NULL, *pname = NULL, *module = NULL;
    char *tmp = NULL;

    /*
     * Case it exists, removes file name extension and path. They are
     * unnecessary to Python API.
     */
    tmp = strip_filename(pathname);

    if (NULL == tmp)
        return NULL;

    pname = PyString_FromString(tmp);
    free(tmp);

    if (NULL == pname) {
        cset_errno(CL_CVT_DATA_FAILED);
        return NULL;
    }

    module = PyImport_Import(pname);

    if (NULL == module) {
        PyErr_Print();
        cset_errno(CL_PY_IMPORT_FAILED);
        return NULL;
    }

    handle = PyModule_GetDict(module);

    if (NULL == handle) {
        cset_errno(CL_PY_GET_DICT_FAILED);
        return NULL;
    }

    Py_DECREF(pname);
    Py_DECREF(module);

    return handle;
}

int py_close(void *data __attribute__((unused)), void *ptr)
{
    PyObject *handle = (PyObject *)ptr;

    Py_DECREF(handle);

    return 0;
}

cl_object_t *py_call(void *data __attribute__((unused)),
    struct cplugin_function_s *foo, cl_plugin_t *cpl __attribute__((unused)),
    struct function_argument *args)
{
    PyObject *pvalue, *pret, *ptr_arg = NULL;
    PyGILState_STATE gstate;
    cl_object_t *ret;
    cl_string_t *s;
    char *tmp;

    gstate = PyGILState_Ensure();

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        ptr_arg = args->ptr;
        Py_INCREF(ptr_arg);
        pvalue = Py_BuildValue("(sO)", args->jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        pvalue = Py_BuildValue("(s)", args->jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        ptr_arg = args->ptr;
        Py_INCREF(ptr_arg);
        pvalue = Py_BuildValue("(O)", ptr_arg);
    } else {
        pvalue = Py_BuildValue("()");
    }

    pret = PyObject_CallObject(foo->symbol, pvalue);

    if (NULL == pret)
        goto end_block;

    ret = cl_object_create_empty(foo->return_value);

    switch (foo->return_value) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            tmp = PyString_AsString(pret);
            cl_object_set_char(ret, tmp[0]);
            break;

        case CL_UCHAR:
            cl_object_set_uchar(ret, (unsigned char)PyInt_AsLong(pret));
            break;

        case CL_INT:
            cl_object_set_int(ret, (int)PyInt_AsLong(pret));
            break;

        case CL_UINT:
            cl_object_set_uint(ret, (unsigned int)PyInt_AsLong(pret));
            break;

        case CL_SINT:
            cl_object_set_sint(ret, (short int)PyInt_AsLong(pret));
            break;

        case CL_USINT:
            cl_object_set_usint(ret, (unsigned short int)PyInt_AsLong(pret));
            break;

        case CL_FLOAT:
            cl_object_set_float(ret, (float)PyFloat_AsDouble(pret));
            break;

        case CL_DOUBLE:
            cl_object_set_double(ret, PyFloat_AsDouble(pret));
            break;

        case CL_LONG:
            cl_object_set_long(ret, PyInt_AsLong(pret));
            break;

        case CL_ULONG:
            cl_object_set_ulong(ret, (unsigned long)PyInt_AsLong(pret));
            break;

        case CL_LLONG:
            cl_object_set_llong(ret, (long long)PyLong_AsLongLong(pret));
            break;

        case CL_ULLONG:
            cl_object_set_ullong(ret,
                                 (unsigned long long)PyLong_AsLongLong(pret));

            break;

        case CL_POINTER:
            cl_object_set_pointer(ret, false, pret, -1, NULL);

            /*
             * Since we just received an object as an argument, we increase its
             * reference count, so we don't lose it.
             */
            Py_INCREF(pret);
            break;

        case CL_STRING:
            cl_object_set_string(ret, PyString_AsString(pret));
            break;

        case CL_BOOLEAN:
            cl_object_set_boolean(ret, (char)PyInt_AsLong(pret));
            break;

        case CL_CSTRING: /* collections strings */
            s = cl_string_create("%s", PyString_AsString(pret));
            cl_object_set_cstring(ret, s);
            cl_string_unref(s);
            break;
    }

    Py_DECREF(pret);

end_block:
    Py_DECREF(pvalue);

    if (ptr_arg != NULL)
        Py_DECREF(ptr_arg);

    PyGILState_Release(gstate);

    return ret;
}

int py_plugin_startup(void *data __attribute__((unused)), void *handle,
    cl_plugin_info_t *info)
{
    PyObject *dict, *foo, *pvalue, *pret;
    struct py_info *plinfo = NULL;
    int ret = -1;
    PyGILState_STATE gstate;

    gstate = PyGILState_Ensure();
    plinfo = (struct py_info *)info_get_custom_data(info);

    if (NULL == plinfo)
        return -1;

    dict = (PyObject *)handle;
    foo = PyDict_GetItemString(dict, (char *)plinfo->startup_name);

    if (PyCallable_Check(foo)) {
        pvalue = Py_BuildValue("()");
        pret = PyObject_CallObject(foo, pvalue);

        if (pret != NULL)
           ret = (int)PyInt_AsLong(pret);

        Py_DECREF(pret);
        Py_DECREF(pvalue);
    }

    PyGILState_Release(gstate);

    return ret;
}

int py_plugin_shutdown(void *data __attribute__((unused)), void *handle,
    cl_plugin_info_t *info)
{
    PyObject *dict, *foo, *pvalue, *pret;
    struct py_info *plinfo = NULL;
    PyGILState_STATE gstate;

    gstate = PyGILState_Ensure();
    plinfo = (struct py_info *)info_get_custom_data(info);

    if (NULL == plinfo)
        return -1;

    dict = (PyObject *)handle;
    foo = PyDict_GetItemString(dict, (char *)plinfo->shutdown_name);

    if (PyCallable_Check(foo)) {
        pvalue = Py_BuildValue("()");
        pret = PyObject_CallObject(foo, pvalue);
        Py_DECREF(pret);
        Py_DECREF(pvalue);
    }

    release_custom_plugin_info(plinfo);
    PyGILState_Release(gstate);

    return 0;
}

bool py_plugin_test(const cl_string_t *mime)
{
    const char *recognized_mime[] = {
        "text/x-python",
        "text/plain",
        "application/octet-stream"
    };
    int i = 0, t;
    cl_string_t *p;

    t = sizeof(recognized_mime) / sizeof(recognized_mime[0]);

    for (i = 0; i < t; i++) {
        p = cl_string_create("%s", recognized_mime[i]);

        if (cl_string_cmp(mime, p) == 0) {
            cl_string_unref(p);
            return true;
        }

        cl_string_unref(p);
    }

    return false;
}

