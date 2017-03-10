
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

/* Structure to save custom plugin informations. */
struct py_info {
    char    *startup_name;
    char    *shutdown_name;
};

static void set_custom_plugin_info(cplugin_info_t *info, const char *startup,
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
    printf("%s: %s\n", __FUNCTION__, foo->name);

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
    cjson_t *paths = NULL, *root, *p;
    int i, t;
    char *tmp;
    cstring_t *s;

    root = library_configuration();

    if (NULL == root)
        return;

    paths = cjson_get_object_item(root, "plugin_path");

    if (NULL == paths)
        return;

    t = cjson_get_array_size(paths);
    PyRun_SimpleString("import sys");

    for (i = 0; i < t; i++) {
        p = cjson_get_array_item(paths, i);
        s = cjson_get_object_value(p);

        if (asprintf(&tmp, "sys.path.append(%s)", cstring_valueof(s)) == -1)
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
    printf("%s\n", __FUNCTION__);
    PyEval_AcquireLock();
    /* FIXME: We're having a segfault here on multi-thread applications */
//    Py_Finalize();
}

/*
 * Loads information from within 'CpluginMainEntry' class.
 */
cplugin_info_t *py_load_info(void *data __attribute__((unused)), void *ptr)
{
    PyObject *dict = (PyObject *)ptr;
    PyObject *class = NULL, *instance = NULL, *result = NULL;
    unsigned int i = 0, t = 0;
    cplugin_info_t *info = NULL;
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
        { "get_api",            NULL },
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

    /* Call methods from the class so we can get more informations. */
    t = sizeof(pyinfo) / sizeof(pyinfo[0]);

    for (i = 0; i < t; i++) {
        result = PyObject_CallMethod(instance, pyinfo[i].name, NULL);

        if (NULL == result)
            goto end_block;

        pyinfo[i].return_value = PyString_AS_STRING(result);
        Py_DECREF(result);
    }

    info = info_create_from_data(pyinfo[0].return_value,
                                 pyinfo[1].return_value,
                                 pyinfo[2].return_value,
                                 pyinfo[3].return_value,
                                 pyinfo[4].return_value);

    if (info != NULL)
        set_custom_plugin_info(info, pyinfo[5].return_value,
                               pyinfo[6].return_value);

    Py_DECREF(instance);

//    for (i = 0; i < t; i++)
//        free(pyinfo[i].return_value);

end_block:
    PyGILState_Release(gstate);
    return info;
}

int py_load_functions(void *data __attribute__((unused)),
    struct cplugin_function_s *flist, void *handle)
{
    if (cdll_map(flist, py_load_function, handle) != NULL)
        return -1;

    return 0;
}

void py_unload_functions(void *data __attribute__((unused)),
    struct cplugin_function_s *flist, void *handle __attribute__((unused)))
{
    cdll_map(flist, py_unload_function, NULL);
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

cobject_t *py_call(void *data __attribute__((unused)),
    struct cplugin_function_s *foo, cplugin_t *cpl __attribute__((unused)),
    struct function_argument *args)
{
    PyObject *pvalue, *capsule_of_args = NULL, *pret;
    PyGILState_STATE gstate;
    cobject_t *ret;

    gstate = PyGILState_Ensure();

    /*
     * Encapsulates 'cplugin_t' and 'foo->args' so we can pass them as a
     * specific Python object, so we can "travel" them between C codes.
     */

    if (foo->arg_mode != CPLUGIN_ARGS_VOID)
        capsule_of_args = PyCapsule_New(foo->args, PYARGS, NULL);

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_VOID:
            pvalue = Py_BuildValue("()");
            break;

        case CPLUGIN_ARGS_ONLY:
            pvalue = Py_BuildValue("(s)", args->jargs);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            //pvalue = Py_BuildValue("(O)", capsule_of_args);
            break;

        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            //pvalue = Py_BuildValue("(sO)", args->jargs, capsule_of_args);
            break;
    }

    pret = PyObject_CallObject(foo->symbol, pvalue);
        PyErr_Print();

    printf("%s: %d\n", __FUNCTION__, pret == NULL);
    ret = cobject_create_empty(foo->return_value);

    switch (foo->return_value) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            cobject_set_char(ret, (char)PyInt_AsLong(pret));
            break;

        case CL_UCHAR:
            cobject_set_uchar(ret, (unsigned char)PyInt_AsLong(pret));
            break;

        case CL_INT:
            cobject_set_int(ret, (int)PyInt_AsLong(pret));
            break;

        case CL_UINT:
            cobject_set_uint(ret, (unsigned int)PyInt_AsLong(pret));
            break;

        case CL_SINT:
            cobject_set_sint(ret, (short int)PyInt_AsLong(pret));
            break;

        case CL_USINT:
            cobject_set_usint(ret, (unsigned short int)PyInt_AsLong(pret));
            break;

        case CL_FLOAT:
            cobject_set_float(ret, (float)PyFloat_AsDouble(pret));
            break;

        case CL_DOUBLE:
            cobject_set_double(ret, PyFloat_AsDouble(pret));
            break;

        case CL_LONG:
            cobject_set_long(ret, PyInt_AsLong(pret));
            break;

        case CL_ULONG:
            cobject_set_ulong(ret, (unsigned long)PyInt_AsLong(pret));
            break;

        case CL_LLONG:
            cobject_set_llong(ret, (long long)PyLong_AsLongLong(pret));
            break;

        case CL_ULLONG:
            cobject_set_ullong(ret, (unsigned long long)PyLong_AsLongLong(pret));
            break;

        case CL_POINTER:
        case CL_CSTRING:     /* collections strings */
            /* Not supported yet */
            break;

        case CL_STRING:      /* 'char *' strings */
            cobject_set_string(ret, PyString_AsString(pret));
            break;

        case CL_BOOLEAN:
            cobject_set_boolean(ret, (char)PyInt_AsLong(pret));
            break;
    }

//    Py_DECREF(pret);
    Py_DECREF(pvalue);

    PyGILState_Release(gstate);

    return ret;
}

int py_plugin_startup(void *data __attribute__((unused)), void *handle,
    cplugin_info_t *info)
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
    cplugin_info_t *info)
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

bool py_plugin_test(const cstring_t *mime)
{
    const char *recognized_mime[] = {
        "text/x-python",
        "text/plain",
        "application/octet-stream"
    };
    int i = 0, t;
    cstring_t *p;

    t = sizeof(recognized_mime) / sizeof(recognized_mime[0]);

    for (i = 0; i < t; i++) {
        p = cstring_create("%s", recognized_mime[i]);

        if (cstring_cmp(mime, p) == 0) {
            cstring_unref(p);
            return true;
        }

        cstring_unref(p);
    }

    return false;
}

