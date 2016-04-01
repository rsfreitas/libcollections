
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
cjson_t *api_load(const char *api_data)
{
    cjson_t *api;
    cstring_t *s = cstring_create("%s", api_data);

    api = cjson_parse(s);
    cstring_destroy(s);

    if (NULL == api)
        return NULL;

    return api;
}

void api_unload(cjson_t *api)
{
    cjson_delete(api);
}

/*
 * Makes a translation between a JSON representing arguments from a function
 * to a 'struct cplugin_fdata_s'.
 */
static struct cplugin_fdata_s *api_parse_function_args(cjson_t *args)
{
    struct cplugin_fdata_s *alist = NULL, *p = NULL;
    int i, t_args = 0;
    cjson_t *a, *q;
    cstring_t *jname, *jtype;

    t_args = cjson_get_array_size(args);

    for (i = 0; i < t_args; i++) {
        a = cjson_get_array_item(args, i);

        /* name */
        q = cjson_get_object_item(a, ARGUMENT_NAME);
        jname = cjson_get_object_value(q);

        /* type */
        q = cjson_get_object_item(a, ARGUMENT_TYPE);
        jtype = cjson_get_object_value(q);

        p = new_cplugin_fdata_s(cstring_valueof(jname),
                                cvt_str_to_cv(cstring_valueof(jtype)), 0);

        if (NULL == p)
            goto error_block;

        alist = cdll_unshift(alist, p);
    }

    return alist;

error_block:
    if (args != NULL)
        cdll_free(alist, destroy_cplugin_fdata_s);

    return NULL;
}

/*
 * Translate a JSON API, created by a plugin, into a 'struct cplugin_function_s',
 * so that we can manipulated it internally.
 */
struct cplugin_function_s *api_parse(cplugin_info_t *info)
{
    cjson_t *functions, *f, *q, *api;
    struct cplugin_function_s *flist = NULL, *p = NULL;
    struct cplugin_fdata_s *args = NULL;
    int i, t_functions = 0;
    enum cl_type return_value;
    enum cplugin_arg type_of_args;
    cstring_t *jname, *jrv;

    api = info_get_api(info);
    functions = cjson_get_object_item(api, PLUGIN_API);
    t_functions = cjson_get_array_size(functions);

    for (i = 0; i < t_functions; i++) {
        args = NULL;
        f = cjson_get_array_item(functions, i);

        /* name */
        q = cjson_get_object_item(f, FUNCTION_NAME);
        jname = cjson_get_object_value(q);

        /* function return type */
        q = cjson_get_object_item(f, FUNCTION_RETURN_TYPE);
        jrv = cjson_get_object_value(q);
        return_value = cvt_str_to_cv(cstring_valueof(jrv));

        /* function arguments */
        q = cjson_get_object_item(f, FUNCTION_ARGUMENTS);

        if (NULL == q) {
            q = cjson_get_object_item(f, FUNCTION_VARIADIC_ARGUMENTS);

            if ((q != NULL) && (cjson_get_object_type(q) == CJSON_TRUE))
                type_of_args = CPLUGIN_ARG_VAR;
            else
                type_of_args = CPLUGIN_NO_ARGS;
        } else {
            type_of_args = CPLUGIN_ARG_FIXED;
            args = api_parse_function_args(q);

            if (NULL == args)
                goto error_block_args;
        }

        p = new_cplugin_function_s(cstring_valueof(jname), return_value,
                                   type_of_args, args);

        if (NULL == p)
            goto error_block_functions;

        flist = cdll_unshift(flist, p);
    }

    return flist;

error_block_args:
    if (args != NULL)
        cdll_free(args, destroy_cplugin_fdata_s);

error_block_functions:
    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    return NULL;
}

cstring_t *api_to_cstring(cjson_t *api)
{
    return cjson_to_cstring(api, true);
}

cstring_list_t *api_functions(const cplugin_info_t *info)
{
    cjson_t *f, *o, *p;
    int i, t = 0;
    cstring_list_t *list = NULL;

    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    f = cjson_get_object_item(info_get_api(info), PLUGIN_API);

    if (NULL == f) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    t = cjson_get_array_size(f);
    list = cstring_list_create();

    for (i = 0; i < t; i++) {
        o = cjson_get_array_item(f, i);

        if (NULL == o) {
            cset_errno(CL_OBJECT_NOT_FOUND);
            return NULL;
        }

        p = cjson_get_object_item(o, FUNCTION_NAME);
        cstring_list_add(list, cjson_get_object_value(p));
    }

    return list;
}

static cjson_t *get_function_object(cjson_t *api, const char *function_name)
{
    cjson_t *f, *o, *p;
    int i, t = 0;
    cstring_t *name;

    f = cjson_get_object_item(api, PLUGIN_API);
    t = cjson_get_array_size(f);

    for (i = 0; i < t; i++) {
        o = cjson_get_array_item(f, i);
        p = cjson_get_object_item(o, FUNCTION_NAME);
        name = cjson_get_object_value(p);

        if (!strcmp(cstring_valueof(name), function_name))
            return o;
    }

    return NULL;
}
cstring_list_t *api_function_arguments(const cplugin_info_t *info,
    const char *function_name)
{
    cjson_t *foo, *args, *a, *p;
    int i, t = 0;
    cstring_list_t *list = NULL;

    foo = get_function_object(info_get_api(info), function_name);
    args = cjson_get_object_item(foo, FUNCTION_ARGUMENTS);

    if (NULL == args)
        return NULL;

    t = cjson_get_array_size(args);
    list = cstring_list_create();

    for (i = 0; i < t; i++) {
        a = cjson_get_array_item(args, i);
        p = cjson_get_object_item(a, ARGUMENT_NAME);
        cstring_list_add(list, cjson_get_object_value(p));
    }

    return list;
}

enum cl_type api_function_return_type(const cplugin_info_t *info,
    const char *function_name)
{
    cjson_t *foo, *p;
    cstring_t *v;

    foo = get_function_object(info_get_api(info), function_name);
    p = cjson_get_object_item(foo, FUNCTION_RETURN_TYPE);

    if (NULL == p) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    v = cjson_get_object_value(p);

    return cvt_str_to_cv(cstring_valueof(v));
}

enum cplugin_arg api_function_arg_mode(const cplugin_info_t *info,
    const char *function_name)
{
    cjson_t *foo, *p;
    enum cplugin_arg t = CPLUGIN_ARG_FIXED;

    foo = get_function_object(info_get_api(info), function_name);

    if (cjson_get_object_item(foo, FUNCTION_ARGUMENTS) != NULL)
        return t;
    else {
        p = cjson_get_object_item(foo, FUNCTION_VARIADIC_ARGUMENTS);

        if ((p != NULL) && (cjson_get_object_type(p) == CJSON_TRUE))
            t = CPLUGIN_ARG_VAR;
        else
            t = CPLUGIN_NO_ARGS;
    }

    return t;
}

enum cl_type api_function_arg_type(const cplugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    cjson_t *foo, *args, *a, *p;
    int i, t = 0;
    cstring_t *data;
    enum cl_type type = CL_VOID;

    if (api_function_arg_mode(info, function_name) != CPLUGIN_ARG_FIXED)
        return -1;

    foo = get_function_object(info_get_api(info), function_name);
    args = cjson_get_object_item(foo, FUNCTION_ARGUMENTS);
    t = cjson_get_array_size(args);

    for (i = 0; i < t; i++) {
        a = cjson_get_array_item(args, i);
        p = cjson_get_object_item(a, ARGUMENT_NAME);
        data = cjson_get_object_value(p);

        if (strcmp(cstring_valueof(data), argument_name))
            continue;

        p = cjson_get_object_item(a, ARGUMENT_TYPE);
        data = cjson_get_object_value(p);
        type = cvt_str_to_cv(cstring_valueof(data));

        break;
    }

    return type;
}

