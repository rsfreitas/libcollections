
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
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

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "collections.h"
#include "plugin/plugin.h"

/*
 * -- ARGUMENTS
 */

cobject_t LIBEXPORT *cplugin_argument(const cplugin_arg_t *args,
    const char *arg_name)
{
    struct cplugin_fdata_s *p = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object_with_offset(args, CPLUGIN_ARG,
                                    CPLUGIN_ARG_OBJECT_OFFSET) == false)
    {
        return NULL;
    }

    if (NULL == arg_name) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cdll_map((void *)args, search_cplugin_fdata_s, (char *)arg_name);

    if (NULL == p) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    return cobject_ref(p->value);
}

int LIBEXPORT cplugin_arg_count(const cplugin_arg_t *args)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object_with_offset(args, CPLUGIN_ARG,
                                    CPLUGIN_ARG_OBJECT_OFFSET) == false)
    {
        return -1;
    }

    return cdll_size((void *)args);
}

/*
 * -- RETURN VALUE
 */

int LIBEXPORT cplugin_set_return_value(cplugin_t *cpl, const char *function_name,
    unsigned int caller_id, enum cl_type type, ...)
{
    cplugin_s *pl = (cplugin_s *)cpl;
    struct cplugin_function_s *foo = NULL;
    struct cplugin_fdata_s *return_value = NULL;
    va_list ap;
    void *p;
    int psize;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(cpl, CPLUGIN) == false)
        return -1;

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    foo = cdll_map(pl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    return_value = new_cplugin_fdata_s(NULL, type, caller_id);

    if (NULL == return_value)
        return -1;

    va_start(ap, NULL);

    switch (type) {
        case CL_INT:
            return_value->value = cobject_create(CL_INT, va_arg(ap, int), NULL);
            break;

        case CL_UINT:
            return_value->value = cobject_create(CL_UINT,
                                                 (unsigned int)va_arg(ap, int),
                                                 NULL);

            break;

        case CL_SINT:
            return_value->value = cobject_create(CL_SINT,
                                                 (short int)va_arg(ap, int),
                                                 NULL);
            break;

        case CL_USINT:
            return_value->value = cobject_create(CL_USINT,
                                                 (unsigned short int)va_arg(ap,
                                                                            int),
                                                 NULL);

            break;

        case CL_VOID:
            /* noop */
            break;

        case CL_POINTER:
            p = va_arg(ap, void *);
            psize = va_arg(ap, int);
            return_value->value = cobject_create(CL_POINTER, true, p, psize,
                                                 NULL);

            break;

        case CL_STRING:
            return_value->value = cobject_create(CL_STRING, va_arg(ap, char *),
                                                 NULL);

            break;

        case CL_CSTRING:
            p = va_arg(ap, void *);
            return_value->value = cobject_create(CL_CSTRING, p, NULL);
            break;

        case CL_CHAR:
            return_value->value = cobject_create(CL_CHAR, (char)va_arg(ap, int),
                                                 NULL);

            break;

        case CL_UCHAR:
            return_value->value = cobject_create(CL_UCHAR,
                                                 (unsigned char)va_arg(ap, int),
                                                 NULL);

            break;

        case CL_FLOAT:
            return_value->value = cobject_create(CL_FLOAT, va_arg(ap, double),
                                                 NULL);

            break;

        case CL_DOUBLE:
            return_value->value = cobject_create(CL_DOUBLE, va_arg(ap, double),
                                                 NULL);

            break;

        case CL_BOOLEAN:
            return_value->value = cobject_create(CL_BOOLEAN, va_arg(ap, int),
                                                 NULL);

            break;

        case CL_LONG:
            return_value->value = cobject_create(CL_LONG, va_arg(ap, long),
                                                 NULL);

            break;

        case CL_ULONG:
            return_value->value = cobject_create(CL_ULONG, va_arg(ap, long),
                                                 NULL);

            break;

        case CL_LLONG:
            return_value->value = cobject_create(CL_LLONG, va_arg(ap, long),
                                                 NULL);

            break;

        case CL_ULLONG:
            return_value->value = cobject_create(CL_ULLONG, va_arg(ap, long),
                                                 NULL);

            break;

        default:
            cset_errno(CL_UNSUPPORTED_TYPE);
            destroy_cplugin_fdata_s(return_value);
            return -1;
    }

    va_end(ap);

    /* Add into the return values list */
    pthread_mutex_lock(&foo->m_return_value);
    foo->values = cdll_unshift(foo->values, return_value);
    pthread_mutex_unlock(&foo->m_return_value);

    return 0;
}

/*
 * -- PLUGIN INFORMATION API
 */

cplugin_info_t LIBEXPORT *cplugin_info(const cplugin_t *cpl)
{
    cplugin_s *pl = (cplugin_s *)cpl;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(cpl, CPLUGIN) == false)
        return NULL;

    return info_ref(pl->info);
}

cplugin_info_t LIBEXPORT *cplugin_info_from_file(const char *pathname)
{
    void *handle = NULL;
    cplugin_info_t *info = NULL;
    struct dl_plugin_driver *pdriver = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    pdriver = dl_get_plugin_driver(pathname);

    if (NULL == pdriver) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    handle = dl_open(pdriver, pathname);

    if (NULL == handle)
        return NULL;

    /* Loads plugin info */
    info = dl_load_info(pdriver, handle);

    if (handle != NULL)
        dl_close(pdriver, handle);

    return info;
}

int LIBEXPORT cplugin_info_unref(cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return -1;

    info_unref(info);

    return 0;
}

const char LIBEXPORT *cplugin_name(const cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    return info_get_name(info);
}

const char LIBEXPORT *cplugin_version(const cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    return info_get_version(info);
}

const char LIBEXPORT *cplugin_author(const cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    return info_get_author(info);
}

const char LIBEXPORT *cplugin_description(const cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    return info_get_description(info);
}

cstring_t LIBEXPORT *cplugin_API(const cplugin_info_t *info)
{
    cjson_t *api;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    api = info_get_api(info);

    if (NULL == api)
        return NULL;

    return api_to_cstring(api);
}

cstring_list_t LIBEXPORT *cplugin_functions(const cplugin_info_t *info)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    return api_functions(info);
}

enum cl_type LIBEXPORT cplugin_function_return_type(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return -1;

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_return_type(info, function_name);
}

cstring_list_t LIBEXPORT *cplugin_function_arguments(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return NULL;

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_function_arguments(info, function_name);
}

enum cplugin_arg LIBEXPORT cplugin_function_arg_mode(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return -1;

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_mode(info, function_name);
}

enum cl_type LIBEXPORT cplugin_function_arg_type(const cplugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(info, CPLUGIN_INFO) == false)
        return -1;

    if ((NULL == function_name) || (NULL == argument_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_type(info, function_name, argument_name);
}

/*
 * -- PLUGIN MANIPULATION
 */

cobject_t LIBEXPORT *cplugin_call_ex(int argc, cplugin_t *cpl,
    const char *function_name, ...)
{
    cplugin_s *pl = (cplugin_s *)cpl;
    struct cplugin_function_s *foo = NULL;
    va_list ap;
    int fargc = 0;
    cobject_t *cplv = NULL;
    uint32_t caller_id = 0;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(cpl, CPLUGIN) == false)
        return NULL;

    va_start(ap, NULL);
    argc -= CPLUGIN_CALL_DEF_ARGUMENTS;
    foo = cdll_map(pl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    /* Checks if the arguments are right */
    if (foo->type_of_args == CPLUGIN_ARG_FIXED) {
        fargc = cdll_size(foo->args);

        if ((argc / 2) != fargc) {
            cset_errno(CL_INVALID_VALUE);
            return NULL;
        }
    }

    /* Set up arguments value */
    if (foo->type_of_args != CPLUGIN_NO_ARGS)
        if (adjust_arguments(foo, argc, ap) < 0)
            return NULL;

    /*
     * Creates an identification number to the function.
     *
     * TODO: Implement a way to ensure that this will not repeat for other
     *       calls.
     */
    caller_id = random_caller_id(foo);

    /*
     * If the same function is called at the same time, one may set its
     * return value while the other is still trying to get it or both may
     * try set it at the same time.
     *
     * One way to solve this is to use the random identification number
     * and passed it to the called function.
     */

    /* Call the function */
    dl_call(pl, foo, caller_id);

    if (foo->return_value != CL_VOID)
        cplv = cplugin_get_return_value(pl, function_name, caller_id);

    /* Unload arguments */
    if (foo->type_of_args == CPLUGIN_ARG_VAR) {
        cdll_free(foo->args, destroy_cplugin_fdata_s);
        foo->args = NULL;
    }

    if ((NULL == cplv) && (foo->return_value != CL_VOID))
        return NULL;

    return cplv;
}

cplugin_t LIBEXPORT *cplugin_load(const char *pathname)
{
    struct cplugin_function_s *flist = NULL;
    cplugin_s *cpl = NULL;
    cplugin_info_t *info = NULL;
    void *handle = NULL;
    struct dl_plugin_driver *pdriver = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (access(pathname, 0x00) == -1) {
        cset_errno(CL_FILE_NOT_FOUND);
        return NULL;
    }

    pdriver = dl_get_plugin_driver(pathname);

    if (NULL == pdriver) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    handle = dl_open(pdriver, pathname);

    if (NULL == handle) {
        cset_errno(CL_DLOPEN);
        return NULL;
    }

    /* Loads plugin info */
    info = dl_load_info(pdriver, handle);

    if (NULL == info)
        goto error_block;

    /* Transform the JSON API into a list of functions and arguments */
    flist = api_parse(info);

    if (NULL == flist)
        goto error_block;

    /*
     * Look for the real plugin exported functions (internal API) and point
     * to them so they can be called.
     */
    if (dl_load_functions(pdriver, flist, handle) < 0)
        goto error_block;

    /* Runs the plugin initialization function */
    if (dl_plugin_startup(pdriver, handle, info))
        goto error_block;

    cpl = new_cplugin_s();

    if (NULL == cpl)
        goto error_block;

    cpl->dl = pdriver;
    cpl->handle = handle;
    cpl->functions = flist;
    cpl->info = info;

    return cpl;

error_block:
    if (handle != NULL)
        dl_close(pdriver, handle);

    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    return NULL;
}

int LIBEXPORT cplugin_unload(cplugin_t *cpl)
{
    cplugin_s *pl = (cplugin_s *)cpl;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(cpl, CPLUGIN) == false)
        return -1;

    /* Runs the plugin closing function */
    if (dl_plugin_shutdown(cpl) < 0) {
        cset_errno(CL_PLUGIN_SHUTDOWN);
        return -1;
    }

    if (pl->handle != NULL)
        dl_close(pl->dl, pl->handle);

    if (destroy_cplugin_s(cpl) < 0)
        return -1;

    return 0;
}

void LIBEXPORT cplugin_set_supported_types(enum cplugin_type types)
{
    cerrno_clear();

    if (library_initialized() == false)
        return;

    dl_enable_plugin_types(types);
}

