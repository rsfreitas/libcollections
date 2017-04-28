
/*
 * Description: Functions to parse a plugin JSON API to internal structures
 *              understood by the library. All internal JSON access to plugin
 *              informations must be here.
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

#include <string.h>

#include "collections.h"
#include "plugin.h"

#define PLUGIN_API                      "API"
#define FUNCTION_NAME                   "name"
#define FUNCTION_RETURN_TYPE            "return_type"
#define FUNCTION_ARGUMENTS              "arguments"
#define FUNCTION_VARIADIC_ARGUMENTS     "varargs"
#define ARGUMENT_NAME                   "name"
#define ARGUMENT_TYPE                   "type"

static enum cl_type cvt_str_to_cv(const char *rv)
{
    if (strcmp(rv, "char") == 0)
        return CL_CHAR;
    else if (strcmp(rv, "uchar") == 0)
        return CL_UCHAR;
    else if (strcmp(rv, "int") == 0)
        return CL_INT;
    else if (strcmp(rv, "uint") == 0)
        return CL_UINT;
    else if (strcmp(rv, "sint") == 0)
        return CL_SINT;
    else if (strcmp(rv, "usint") == 0)
        return CL_USINT;
    else if (strcmp(rv, "float") == 0)
        return CL_FLOAT;
    else if (strcmp(rv, "double") == 0)
        return CL_DOUBLE;
    else if (strcmp(rv, "long") == 0)
        return CL_LONG;
    else if (strcmp(rv, "ulong") == 0)
        return CL_ULONG;
    else if (strcmp(rv, "llong") == 0)
        return CL_LLONG;
    else if (strcmp(rv, "ullong") == 0)
        return CL_ULLONG;
    else if (strcmp(rv, "pointer") == 0)
        return CL_POINTER;
    else if (strcmp(rv, "string") == 0)
        return CL_STRING;
    else if (strcmp(rv, "cstring") == 0)
        return CL_CSTRING;
    else if (strcmp(rv, "boolean") == 0)
        return CL_BOOLEAN;

    return CL_VOID;
}

/*
 * FIXME: We need to create better error codes, to tell the user that he may
 *        have an error in its JSON API.
 */
cl_json_t *api_load(const char *api_data)
{
    cl_json_t *api;
    cl_string_t *s = cl_string_create("%s", api_data);

    api = cl_json_parse(s);
    cl_string_destroy(s);

    if (NULL == api)
        return NULL;

    return api;
}

void api_unload(cl_json_t *api)
{
    cl_json_delete(api);
}

/*
 * Makes a translation between a JSON representing arguments from a function
 * to a 'struct cplugin_fdata_s'.
 */
static struct cplugin_fdata_s *api_parse_function_args(cl_json_t *args)
{
    struct cplugin_fdata_s *alist = NULL, *p = NULL;
    int i, t_args = 0;
    cl_json_t *a, *q;
    cl_string_t *jname, *jtype;

    t_args = cl_json_get_array_size(args);

    for (i = 0; i < t_args; i++) {
        a = cl_json_get_array_item(args, i);

        /* name */
        q = cl_json_get_object_item(a, ARGUMENT_NAME);
        jname = cl_json_get_object_value(q);

        /* type */
        q = cl_json_get_object_item(a, ARGUMENT_TYPE);
        jtype = cl_json_get_object_value(q);

        p = new_cplugin_fdata_s(cl_string_valueof(jname),
                                cvt_str_to_cv(cl_string_valueof(jtype)));

        if (NULL == p)
            goto error_block;

        alist = cl_dll_unshift(alist, p);
    }

    return alist;

error_block:
    if (args != NULL)
        cl_dll_free(alist, destroy_cplugin_fdata_s);

    return NULL;
}

static int find_pointer_argument(void *a, void *b __attribute__((unused)))
{
    struct cplugin_fdata_s *arg = (struct cplugin_fdata_s *)a;

    if (arg->type == CL_POINTER)
        return 1;

    return 0;
}

static enum cl_plugin_arg_mode get_arg_mode(struct cplugin_fdata_s *args)
{
    struct cplugin_fdata_s *ptr = NULL;
    enum cl_plugin_arg_mode arg_mode = CL_PLUGIN_ARGS_VOID;
    bool ptr_argument = false, other_arguments = false;
    int total = 0;

    /*
     * TODO: Block if we have more than one pointer argument.
     */

    total = cl_dll_size(args);
    ptr = cl_dll_map(args, find_pointer_argument, NULL);

    if (ptr != NULL)
        ptr_argument = true;

    if (((ptr_argument == true) && (total > 1)) ||
        ((ptr_argument == false) && (total >= 1)))
    {
        other_arguments = true;
    }

    if (ptr_argument)
        arg_mode |= CL_PLUGIN_ARGS_POINTER;

    if (other_arguments)
        arg_mode |= CL_PLUGIN_ARGS_COMMON;

    return arg_mode;
}

/*
 * Translate a JSON API, created by a plugin, into a 'struct cplugin_function_s',
 * so that we can manipulated it internally.
 */
struct cplugin_function_s *api_parse(cl_plugin_info_t *info)
{
    cl_json_t *functions, *f, *q, *api;
    struct cplugin_function_s *flist = NULL, *p = NULL;
    struct cplugin_fdata_s *args = NULL;
    int i, t_functions = 0;
    enum cl_type return_value;
    cl_string_t *jname, *jrv;
    enum cl_plugin_arg_mode arg_mode = CL_PLUGIN_ARGS_VOID;

    api = info_get_api(info);
    functions = cl_json_get_object_item(api, PLUGIN_API);
    t_functions = cl_json_get_array_size(functions);

    for (i = 0; i < t_functions; i++) {
        args = NULL;
        f = cl_json_get_array_item(functions, i);

        /* name */
        q = cl_json_get_object_item(f, FUNCTION_NAME);
        jname = cl_json_get_object_value(q);

        /* function return type */
        q = cl_json_get_object_item(f, FUNCTION_RETURN_TYPE);
        jrv = cl_json_get_object_value(q);
        return_value = cvt_str_to_cv(cl_string_valueof(jrv));

        /* function arguments */
        q = cl_json_get_object_item(f, FUNCTION_ARGUMENTS);

        if (q != NULL) {
            args = api_parse_function_args(q);

            if (NULL == args)
                goto error_block_args;

            arg_mode = get_arg_mode(args);
        }

        p = new_cplugin_function_s(cl_string_valueof(jname), return_value,
                                   arg_mode, args);

        if (NULL == p)
            goto error_block_functions;

        flist = cl_dll_unshift(flist, p);
    }

    return flist;

error_block_args:
    if (args != NULL)
        cl_dll_free(args, destroy_cplugin_fdata_s);

error_block_functions:
    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    return NULL;
}

cl_string_t *api_to_cstring(cl_json_t *api)
{
    return cl_json_to_cstring(api, true);
}

cl_string_list_t *api_functions(const cl_plugin_info_t *info)
{
    cl_json_t *f, *o, *p;
    int i, t = 0;
    cl_string_list_t *list = NULL;

    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    f = cl_json_get_object_item(info_get_api(info), PLUGIN_API);

    if (NULL == f) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    t = cl_json_get_array_size(f);
    list = cl_string_list_create();

    for (i = 0; i < t; i++) {
        o = cl_json_get_array_item(f, i);

        if (NULL == o) {
            cset_errno(CL_OBJECT_NOT_FOUND);
            return NULL;
        }

        p = cl_json_get_object_item(o, FUNCTION_NAME);
        cl_string_list_add(list, cl_json_get_object_value(p));
    }

    return list;
}

static cl_json_t *get_function_object(cl_json_t *api, const char *function_name)
{
    cl_json_t *f, *o, *p;
    int i, t = 0;
    cl_string_t *name;

    f = cl_json_get_object_item(api, PLUGIN_API);
    t = cl_json_get_array_size(f);

    for (i = 0; i < t; i++) {
        o = cl_json_get_array_item(f, i);
        p = cl_json_get_object_item(o, FUNCTION_NAME);
        name = cl_json_get_object_value(p);

        if (!strcmp(cl_string_valueof(name), function_name))
            return o;
    }

    return NULL;
}

cl_string_list_t *api_function_arguments(const cl_plugin_info_t *info,
    const char *function_name)
{
    cl_json_t *foo, *args, *a, *p;
    int i, t = 0;
    cl_string_list_t *list = NULL;

    foo = get_function_object(info_get_api(info), function_name);
    args = cl_json_get_object_item(foo, FUNCTION_ARGUMENTS);

    if (NULL == args)
        return NULL;

    t = cl_json_get_array_size(args);
    list = cl_string_list_create();

    for (i = 0; i < t; i++) {
        a = cl_json_get_array_item(args, i);
        p = cl_json_get_object_item(a, ARGUMENT_NAME);
        cl_string_list_add(list, cl_json_get_object_value(p));
    }

    return list;
}

enum cl_type api_function_return_type(const cl_plugin_info_t *info,
    const char *function_name)
{
    cl_json_t *foo, *p;
    cl_string_t *v;

    foo = get_function_object(info_get_api(info), function_name);
    p = cl_json_get_object_item(foo, FUNCTION_RETURN_TYPE);

    if (NULL == p) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    v = cl_json_get_object_value(p);

    return cvt_str_to_cv(cl_string_valueof(v));
}

enum cl_plugin_arg_mode api_function_arg_mode(const cl_plugin_info_t *info,
    const char *function_name)
{
    cl_json_t *foo, *args, *p, *type;
    cl_string_t *value;
    enum cl_plugin_arg_mode mode = CL_PLUGIN_ARGS_VOID;
    bool ptr_argument = false, other_arguments = false;
    int i, total;

    foo = get_function_object(info_get_api(info), function_name);
    args = cl_json_get_object_item(foo, FUNCTION_ARGUMENTS);

    if (NULL == args)
        return CL_PLUGIN_ARGS_VOID;

    total = cl_json_get_array_size(args);

    for (i = 0; i < total; i++) {
        p = cl_json_get_array_item(args, i);
        type = cl_json_get_object_item(p, ARGUMENT_TYPE);
        value = cl_json_get_object_value(type);

        if (strcmp(cl_string_valueof(value), "pointer") == 0) {
            ptr_argument = true;
            break;
        }
    }

    if (((ptr_argument == true) && (total > 1)) ||
        ((ptr_argument == false) && (total >= 1)))
    {
        other_arguments = true;
    }

    if (ptr_argument)
        mode |= CL_PLUGIN_ARGS_POINTER;

    if (other_arguments)
        mode |= CL_PLUGIN_ARGS_COMMON;

    return mode;
}

enum cl_type api_function_arg_type(const cl_plugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    cl_json_t *foo, *args, *a, *p;
    int i, t = 0;
    cl_string_t *data;
    enum cl_type type = CL_VOID;

    if (api_function_arg_mode(info, function_name) == CL_PLUGIN_ARGS_VOID)
        return -1;

    foo = get_function_object(info_get_api(info), function_name);
    args = cl_json_get_object_item(foo, FUNCTION_ARGUMENTS);
    t = cl_json_get_array_size(args);

    for (i = 0; i < t; i++) {
        a = cl_json_get_array_item(args, i);
        p = cl_json_get_object_item(a, ARGUMENT_NAME);
        data = cl_json_get_object_value(p);

        if (strcmp(cl_string_valueof(data), argument_name))
            continue;

        p = cl_json_get_object_item(a, ARGUMENT_TYPE);
        data = cl_json_get_object_value(p);
        type = cvt_str_to_cv(cl_string_valueof(data));

        break;
    }

    return type;
}

