
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

cvalue_t LIBEXPORT *cplugin_argument(const cplugin_arg_t *args,
    const char *arg_name)
{
    struct cplugin_fdata_s *p = NULL;

    cerrno_clear();

    if ((NULL == args) || (NULL == arg_name)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cdll_map((void *)args, search_cplugin_fdata_s, (char *)arg_name);

    if (NULL == p) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    return cvalue_ref(p->value);
}

int LIBEXPORT cplugin_arg_count(const cplugin_arg_t *args)
{
    cerrno_clear();

    if (NULL == args) {
        cset_errno(CL_NULL_ARG);
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
    struct cplugin_s *pl = (struct cplugin_s *)cpl;
    struct cplugin_function_s *foo = NULL;
    struct cplugin_fdata_s *return_value = NULL;
    va_list ap;
    void *p;
    int psize;

    cerrno_clear();

    if ((NULL == cpl) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    foo = cdll_map(pl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
        return -1;
    }

    return_value = new_cplugin_fdata_s(NULL, type, caller_id);

    if (NULL == return_value)
        return -1;

    va_start(ap, NULL);

    switch (type) {
        case CL_INT:
            return_value->value = cvalue_new(CL_INT, va_arg(ap, int), NULL);
            break;

        case CL_UINT:
            return_value->value = cvalue_new(CL_UINT,
                                             (unsigned int)va_arg(ap, int),
                                             NULL);

            break;

        case CL_VOID:
            /* noop */
            break;

        case CL_POINTER:
            p = va_arg(ap, void *);
            psize = va_arg(ap, int);
            return_value->value = cvalue_new(CL_POINTER, true, p, psize, NULL);
            break;

        case CL_CHAR:
            return_value->value = cvalue_new(CL_CHAR, (char)va_arg(ap, int),
                                             NULL);

            break;

        case CL_FLOAT:
            return_value->value = cvalue_new(CL_FLOAT, va_arg(ap, double), NULL);
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
    struct cplugin_s *pl = (struct cplugin_s *)cpl;

    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_ref(pl->info);
}

cplugin_info_t LIBEXPORT *cplugin_info_from_file(const char *pathname)
{
    void *handle = NULL;
    cplugin_info_t *info = NULL;
    enum cplugin_plugin_type plugin_type;

    cerrno_clear();

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    plugin_type = dl_get_plugin_type(pathname);

    if (plugin_type == CPLUGIN_UNKNOWN) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    handle = dl_open(pathname, plugin_type);

    if (NULL == handle) {
        cset_errno(CL_DLOPEN);
        return NULL;
    }

    /* Loads plugin info */
    info = dl_load_info(handle, plugin_type);

    if (handle != NULL)
        dl_close(handle, plugin_type);

    return info;
}

int LIBEXPORT cplugin_info_unref(cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    info_unref(info);

    return 0;
}

const char LIBEXPORT *cplugin_name(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_name(info);
}

const char LIBEXPORT *cplugin_version(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_version(info);
}

const char LIBEXPORT *cplugin_author(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_author(info);
}

const char LIBEXPORT *cplugin_description(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_description(info);
}

cstring_t LIBEXPORT *cplugin_API(const cplugin_info_t *info)
{
    cjson_t *api;

    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    api = info_get_api(info);

    if (NULL == api)
        return NULL;

    return api_to_string(api);
}

cstring_list_t LIBEXPORT *cplugin_functions(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_functions(info);
}

enum cl_type LIBEXPORT cplugin_function_return_type(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_return_type(info, function_name);
}

cstring_list_t LIBEXPORT *cplugin_function_arguments(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_function_arguments(info, function_name);
}

enum cplugin_arg LIBEXPORT cplugin_function_arg_mode(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_mode(info, function_name);
}

enum cl_type LIBEXPORT cplugin_function_arg_type(const cplugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name) || (NULL == argument_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_type(info, function_name, argument_name);
}

/*
 * -- PLUGIN MANIPULATION
 */

cvalue_t LIBEXPORT *cplugin_call_ex(int argc, cplugin_t *cpl,
    const char *function_name, ...)
{
    struct cplugin_s *pl = (struct cplugin_s *)cpl;
    struct cplugin_function_s *foo = NULL;
    va_list ap;
    int fargc = 0;
    cvalue_t *cplv = NULL;
    uint32_t caller_id = 0;

    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    va_start(ap, NULL);
    argc -= CPLUGIN_CALL_DEF_ARGUMENTS;
    foo = cdll_map(pl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
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
    dl_call(foo, caller_id, pl);

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
    struct cplugin_s *cpl = NULL;
    cplugin_info_t *info = NULL;
    void *handle = NULL;
    cplugin_internal_data_t *plugin_idata = NULL;
    enum cplugin_plugin_type plugin_type;

    cerrno_clear();

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (access(pathname, 0x00) == -1) {
        cset_errno(CL_FILE_NOT_FOUND);
        return NULL;
    }

    plugin_type = dl_get_plugin_type(pathname);

    if (plugin_type == CPLUGIN_UNKNOWN) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    handle = dl_open(pathname, plugin_type);

    if (NULL == handle) {
        cset_errno(CL_DLOPEN);
        return NULL;
    }

    /* Loads plugin info */
    info = dl_load_info(handle, plugin_type);

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
    if (dl_load_functions(flist, handle, plugin_type) < 0)
        goto error_block;

    /* Runs the plugin initialization function */
    plugin_idata = dl_plugin_startup(handle, plugin_type);

    if (NULL == plugin_idata) {
        cset_errno(CL_PLUGIN_STARTUP);
        goto error_block;
    } else {
        /*
         * If the plugin does not have an initialization function makes
         * sure that the @idata is set to NULL.
         */
        if (plugin_idata == handle)
            plugin_idata = NULL;
    }

    cpl = new_cplugin_s();

    if (NULL == cpl)
        goto error_block;

    cpl->type = plugin_type;
    cpl->handle = handle;
    cpl->functions = flist;
    cpl->info = info;
    cpl->idata = plugin_idata;

    return cpl;

error_block:
    if (handle != NULL)
        dl_close(handle, plugin_type);

    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    return NULL;
}

int LIBEXPORT cplugin_unload(cplugin_t *cpl)
{
    struct cplugin_s *pl = (struct cplugin_s *)cpl;

    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    /* Runs the plugin closing function */
    if (dl_plugin_shutdown(cpl) < 0) {
        cset_errno(CL_PLUGIN_SHUTDOWN);
        return -1;
    }

    if (pl->handle != NULL)
        dl_close(pl->handle, pl->type);

    if (destroy_cplugin_s(cpl) < 0)
        return -1;

    return 0;
}

cplugin_internal_data_t LIBEXPORT *cplugin_get_startup_data(cplugin_t *cpl)
{
    struct cplugin_s *pl = (struct cplugin_s *)cpl;

    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return pl->idata;
}

cplugin_internal_data_t LIBEXPORT
    *cplugin_get_shutdown_arg(cplugin_internal_data_t *arg)
{
    return arg;
}

