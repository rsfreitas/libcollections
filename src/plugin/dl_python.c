
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

void py_library_init(void)
{
    setenv("PYTHONPATH", "/usr/local/lib", 1);
    Py_Initialize();
}

void py_library_uninit(void)
{
    Py_Finalize();
}

/*
 * Removes the file name extension, as well as all path, leaving only
 * its name.
 */
static char *py_strip_filename(const char *pathname)
{
    char *ext = NULL, *n = NULL, *tmp = NULL, *bname = NULL;

    tmp = strdup(pathname);
    bname = basename(tmp);
    ext = strrchr(bname, '.');

    if (NULL == ext)
        n = strdup(bname);
    else {
        n = calloc(1, strlen(bname) - strlen(ext) + 1);

        if (NULL == n) {
            cset_errno(CL_NO_MEM);
            return NULL;
        }

        strncpy(n, bname, strlen(bname) - strlen(ext));
    }

    free(tmp);

    return n;
}

/*
 * Loads information from within 'cplugin_entry_s' class.
 */
cplugin_info_t *py_load_info(void *ptr)
{
    struct py_pl_info {
        char *fname;
        char *data;
    };

    PyObject *dict = (PyObject *)ptr;
    PyObject *class = NULL, *instance = NULL, *result = NULL;
    unsigned int i = 0, t = 0;
    struct py_pl_info pyinfo[] = {
        { "get_name",           NULL },
        { "get_version",        NULL },
        { "get_creator",        NULL },
        { "get_description",    NULL },
        { "get_api",            NULL }
    };

    class = PyDict_GetItemString(dict, "cplugin_entry_s");

    if (NULL == class) {
        cset_errno(CL_ENTRY_SYMBOL_NOT_FOUND);
        return NULL;
    }

    if (!PyCallable_Check(class)) {
        cset_errno(CL_NO_PLUGIN_INFO);
        return NULL;
    } else
        instance = PyObject_CallObject(class, NULL);

    if (NULL == instance) {
        cset_errno(CL_NO_PLUGIN_INFO);
        return NULL;
    }

    /* Call methods from the class so we can get more informations. */
    t = sizeof(pyinfo) / sizeof(pyinfo[0]);

    for (i = 0; i < t; i++) {
        result = PyObject_CallMethod(instance, pyinfo[i].fname, NULL);

        if (NULL == result) {
            cset_errno(CL_NO_PLUGIN_INFO);
            return NULL;
        }

        pyinfo[i].data = PyString_AS_STRING(result);
    }

    return info_create_from_data(pyinfo[0].data, pyinfo[1].data, pyinfo[2].data,
                                 pyinfo[3].data, pyinfo[4].data);
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

int py_load_functions(struct cplugin_function_s *flist, void *handle)
{
    if (cdll_map(flist, py_load_function, handle) != NULL) {
        cset_errno(CL_FUNCTION_SYMBOL_NOT_FOUND);
        return -1;
    }

    return 0;
}

void *py_open(const char *pathname)
{
    PyObject *handle = NULL, *pname = NULL, *module = NULL;
    char *tmp = NULL;

    /*
     * Case it exists, removes file name extension and path. They are
     * unnecessary to Python API.
     */
    tmp = py_strip_filename(pathname);

    if (NULL == tmp)
        return NULL;

    pname = PyString_FromString(tmp);
    free(tmp);

    if (NULL == pname) {
        cset_errno(CL_CVT_DATA_FAILED);
        return NULL;
    }

    module = PyImport_Import(pname);
    PyErr_Print();

    if (NULL == module) {
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

int py_close(void *ptr)
{
    PyObject *handle = (PyObject *)ptr;

    Py_DECREF(handle);

    return 0;
}

void py_call(struct cplugin_function_s *foo, uint32_t caller_id,
    struct cplugin_s *cpl)
{
    PyObject *pvalue, *capsule_of_cpl = NULL, *capsule_of_args = NULL;

    /*
     * Encapsulates 'cplugin_t' and 'foo->args' so we can pass them as a
     * specific Python object, so we can "travel" them between C codes.
     */

    if (foo->type_of_args != CPLUGIN_NO_ARGS)
        capsule_of_args = PyCapsule_New(foo->args, PYARGS, NULL);

    if (foo->return_value != CL_VOID)
        capsule_of_cpl = PyCapsule_New(cpl, PYCPLUGIN_T, NULL);

    if (foo->return_value == CL_VOID) {
        if (foo->type_of_args == CPLUGIN_NO_ARGS)
            pvalue = Py_BuildValue("()");
        else
            pvalue = Py_BuildValue("(O)", capsule_of_args);
    } else {
        if (foo->type_of_args == CPLUGIN_NO_ARGS)
            pvalue = Py_BuildValue("(iO)", caller_id, capsule_of_cpl);
        else
            pvalue = Py_BuildValue("(iOO)", caller_id, capsule_of_cpl,
                                   capsule_of_args);
    }

    PyObject_CallObject(foo->symbol, pvalue);
}

cplugin_internal_data_t *py_plugin_startup(void *handle)
{
    PyObject *dict, *foo, *pvalue;

    /* TODO: MEmory leak warning */
    dict = (PyObject *)handle;
    foo = PyDict_GetItemString(dict, (char *)"module_init");

    /* Optional function */
    if (PyCallable_Check(foo)) {
        pvalue = Py_BuildValue("()");
        PyObject_CallObject(foo, pvalue);
    }

    /*
     * The plugin startup function returns nothing. This way, we return @handle
     * itself, preventing later errors.
     */
    return handle;
}

int py_plugin_shutdown(cplugin_internal_data_t *pl_idata __attribute__((unused)),
    void *handle)
{
    PyObject *dict, *foo, *pvalue;

    /* TODO: Memory leak warning */
    dict = (PyObject *)handle;
    foo = PyDict_GetItemString(dict, (char *)"module_uninit");

    /* Optional function */
    if (PyCallable_Check(foo)) {
        pvalue = Py_BuildValue("()");
        PyObject_CallObject(foo, pvalue);
    }

    return 0;
}
