
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
 * -- PLUGIN INFORMATION API
 */

__PUB_API__ cl_plugin_info_t *cl_plugin_info(const cl_plugin_t *cpl)
{
    cplugin_s *pl = (cplugin_s *)cpl;

    __clib_function_init__(true, cpl, CL_OBJ_PLUGIN, NULL);

    return info_ref(pl->info);
}

__PUB_API__ cl_plugin_info_t *cl_plugin_info_from_file(const char *pathname)
{
    void *handle = NULL;
    cl_plugin_info_t *info = NULL;
    struct dl_plugin_driver *pdriver = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

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

__PUB_API__ int cl_plugin_info_unref(cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, -1);
    info_unref(info);

    return 0;
}

__PUB_API__ const char *cl_plugin_name(const cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    return info_get_name(info);
}

__PUB_API__ const char *cl_plugin_version(const cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    return info_get_version(info);
}

__PUB_API__ const char *cl_plugin_author(const cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    return info_get_author(info);
}

__PUB_API__ const char *cl_plugin_description(const cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    return info_get_description(info);
}

__PUB_API__ cl_string_t *cl_plugin_API(const cl_plugin_info_t *info)
{
    cl_json_t *api;

    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);
    api = info_get_api(info);

    if (NULL == api)
        return NULL;

    return api_to_cstring(api);
}

__PUB_API__ cl_stringlist_t *cl_plugin_functions(const cl_plugin_info_t *info)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    return api_functions(info);
}

__PUB_API__ enum cl_type cl_plugin_function_return_type(const cl_plugin_info_t *info,
    const char *function_name)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, -1);

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_return_type(info, function_name);
}

__PUB_API__ cl_stringlist_t *cl_plugin_function_arguments(const cl_plugin_info_t *info,
    const char *function_name)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, NULL);

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_function_arguments(info, function_name);
}

__PUB_API__ enum cl_plugin_arg_mode cl_plugin_function_arg_mode(const cl_plugin_info_t *info,
    const char *function_name)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, -1);

    if (NULL == function_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_mode(info, function_name);
}

__PUB_API__ enum cl_type cl_plugin_function_arg_type(const cl_plugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    __clib_function_init__(true, info, CL_OBJ_PLUGIN_INFO, -1);

    if ((NULL == function_name) || (NULL == argument_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_type(info, function_name, argument_name);
}

/*
 * -- PLUGIN MANIPULATION
 */

__PUB_API__ cl_object_t *cl_plugin_call_ex(int argc, cl_plugin_t *cpl,
    const char *function_name, ...)
{
    cplugin_s *pl = (cplugin_s *)cpl;
    struct cplugin_function_s *foo = NULL;
    va_list ap;
    int fargc = 0;
    cl_object_t *cplv = NULL;
    struct function_argument args = {
        .jargs = NULL,
        .ptr = NULL
    };

    __clib_function_init__(true, cpl, CL_OBJ_PLUGIN, NULL);

    va_start(ap, NULL);
    argc -= CL_PLUGIN_CALL_DEF_ARGUMENTS;
    foo = cl_dll_map(pl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    /* Checks if the arguments are right */
    if (foo->arg_mode != CL_PLUGIN_ARGS_VOID) {
        fargc = cl_dll_size(foo->args);

        if ((argc / 2) != fargc) {
            cset_errno(CL_INVALID_VALUE);
            return NULL;
        }

        /* Set up arguments value */
        if (adjust_arguments(foo, &args, argc, ap) < 0)
            return NULL;
    }

    /* Call the function */
    cplv = dl_call(pl, foo, &args);

    /* Unload arguments */
    if (foo->arg_mode != CL_PLUGIN_ARGS_VOID) {
        if (args.jargs != NULL)
            free(args.jargs);
    }

    if ((NULL == cplv) && (foo->return_value != CL_VOID))
        /* It's an error? */
        return NULL;

    /* Release the returned object if we're a void function */
    if (foo->return_value == CL_VOID) {
        cl_object_destroy(cplv);
        cplv = NULL;
    }

    return cplv;
}

__PUB_API__ cl_plugin_t *cl_plugin_load(const char *pathname)
{
    struct cplugin_function_s *flist = NULL;
    cplugin_s *cpl = NULL;
    cl_plugin_info_t *info = NULL;
    void *handle = NULL;
    struct dl_plugin_driver *pdriver = NULL;
    int error;

    __clib_function_init__(false, NULL, -1, NULL);

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
    /* Save previous error code, so we can correctly return it */
    error = cl_get_last_error();

    if (handle != NULL)
        dl_close(pdriver, handle);

    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    cset_errno(error);

    return NULL;
}

__PUB_API__ int cl_plugin_unload(cl_plugin_t *cpl)
{
    cplugin_s *pl = (cplugin_s *)cpl;

    __clib_function_init__(true, cpl, CL_OBJ_PLUGIN, -1);

    /* Runs the plugin closing function */
    if (dl_plugin_shutdown(cpl) < 0) {
        cset_errno(CL_PLUGIN_SHUTDOWN);
        return -1;
    }

    dl_unload_functions(cpl);

    if (pl->handle != NULL)
        dl_close(pl->dl, pl->handle);

    if (destroy_cplugin_s(cpl) < 0)
        return -1;

    return 0;
}

__PUB_API__ void cl_plugin_set_supported_types(enum cl_plugin_type types)
{
    __clib_function_init_ex2__(false, NULL, -1);
    dl_enable_plugin_types(types);
}

