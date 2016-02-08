
/*
 * Description: Functions to parse a plugin JSON API to internal structures
 *              understood by the library.
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

#include "collections.h"
#include "plugin.h"

#define PLUGIN_API                      "API"
#define FUNCTION_NAME                   "name"
#define FUNCTION_RETURN_TYPE            "return_type"
#define FUNCTION_ARGUMENTS              "args"
#define FUNCTION_VARIADIC_ARGUMENTS     "varargs"
#define ARGUMENT_NAME                   "name"
#define ARGUMENT_TYPE                   "type"

cjson_t *api_load(const char *api_data)
{
    cjson_t *api;
    cstring_t *s = cstring_new("%s", api_data);

    api = cjson_parse(s);
    cstring_free(s);

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
        q = cjson_get_object_item(a, "name");
        jname = cjson_get_object_value(q);

        /* type */
        q = cjson_get_object_item(a, "type");
        jtype = cjson_get_object_value(q);

        p = new_cplugin_fdata_s(cstring_valueof(jname),
                                cstring_value_as_int(jtype), 0);

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
struct cplugin_function_s *api_parse(cjson_t *api)
{
    cjson_t *functions, *f, *q;
    struct cplugin_function_s *flist = NULL, *p = NULL;
    struct cplugin_fdata_s *args = NULL;
    int i, t_functions = 0;
    enum cl_type return_value;
    enum cplugin_arg type_of_args;
    cstring_t *jname, *jrv;

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

/* TODO: Don't use the LIBEXPORT here. */
cstring_list_t LIBEXPORT *cplugin_get_functions(cplugin_info_t *info)
{
    cjson_t *f, *o, *p;
    int i, t = 0;
    cstring_list_t *list = NULL;

    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    f = cjson_get_object_item(info->api, PLUGIN_API);

    if (NULL == f) {
        /* TODO */
        return NULL;
    }

    t = cjson_get_array_size(f);
    list = cstring_list_new();

    for (i = 0; i < t; i++) {
        o = cjson_get_array_item(f, i);

        if (NULL == o) {
            /* TODO */
            return NULL;
        }

        p = cjson_get_object_item(o, FUNCTION_NAME);
        cstring_list_add(list, cjson_get_object_value(p));
    }

    return list;
}

/* TODO: Don't use the LIBEXPORT here. */
cstring_list_t LIBEXPORT *cplugin_get_function_args(cplugin_info_t *info,
    const char *function_name)
{
    cjson_t *foo, *args, *a, *p;
    int i, t = 0;
    cstring_list_t *list = NULL;
    cstring_t *data, *s;

    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    foo = get_function_object(info->api, function_name);
    args = cjson_get_object_item(foo, "arguments");

    if (NULL == args)
        return NULL;

    t = cjson_get_array_size(args);
    list = cstring_list_new();

    for (i = 0; i < t; i++) {
        a = cjson_get_array_item(args, i);
        p = cjson_get_object_item(a, "name");
        data = cjson_get_object_value(p);
        s = cstring_new("%s,", cstring_valueof(data));
        p = cjson_get_object_item(a, "type");
        data = cjson_get_object_value(p);
        cstring_cat(s, "%s", cstring_valueof(data));
        cstring_list_add(list, s);
        cstring_free(s);
    }

    return list;
}

