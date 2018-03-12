
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
#include "plugin.h"

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

static cl_object_t *plugin_call(cplugin_s *pl, struct cplugin_function_s *foo)
{
    cl_object_t *cplv = NULL;

    /* Call the function */
    cplv = dl_call(pl, foo);

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

__PUB_API__ cl_object_t *cl_plugin_call_ex(int argc, cl_plugin_t *cpl,
    const char *function_name, enum cl_type return_type, ...)
{
    struct cplugin_function_s *foo = NULL;
    cl_object_t *cplv = NULL;
    va_list ap;

    __clib_function_init__(true, cpl, CL_OBJ_PLUGIN, NULL);
    va_start(ap, NULL);

    /* We remove our permanent arguments here */
    argc -= CL_PLUGIN_FOREIGN_CALL_DEF_ARGUMENTS;
    foo = dl_load_function(cpl, function_name, return_type, argc, ap);

    if (NULL == foo) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    cplv = plugin_call(cpl, foo);
    va_end(ap);

    /* Release the loaded function */
    if (foo != NULL)
        destroy_cplugin_function_s(foo);

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

    /* Runs the plugin initialization function */
    if (dl_plugin_startup(pdriver, handle, info))
        goto error_block;

    cpl = new_cplugin_s();

    if (NULL == cpl)
        goto error_block;

    cpl->dl = pdriver;
    cpl->handle = handle;
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

    if (pl->handle != NULL)
        dl_close(pl->dl, pl->handle);

    if (destroy_cplugin_s(cpl) < 0)
        return -1;

    return 0;
}

__PUB_API__ bool cl_plugin_function_exists(cl_plugin_t *cpl,
    const char *function_name)
{
    return false;
    // TODO
}

__PUB_API__ char cl_plugin_argument_char(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_CHAR(value);
}

__PUB_API__ unsigned char cl_plugin_argument_uchar(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, 0);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_UCHAR(value);
}

__PUB_API__ int cl_plugin_argument_int(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_INT(value);
}

__PUB_API__ unsigned int cl_plugin_argument_uint(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, 0);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_UINT(value);
}

__PUB_API__ short int cl_plugin_argument_sint(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_SINT(value);
}

__PUB_API__ unsigned short int cl_plugin_argument_usint(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, 0);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_USINT(value);
}

__PUB_API__ long cl_plugin_argument_long(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_LONG(value);
}

__PUB_API__ unsigned long cl_plugin_argument_ulong(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, 0);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_ULONG(value);
}

__PUB_API__ long long cl_plugin_argument_llong(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_LLONG(value);
}

__PUB_API__ unsigned long long cl_plugin_argument_ullong(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, 0);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_ULLONG(value);
}

__PUB_API__ float cl_plugin_argument_float(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return -1;

    return CL_OBJECT_AS_FLOAT(value);
}

__PUB_API__ double cl_plugin_argument_double(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return 0;

    return CL_OBJECT_AS_DOUBLE(value);
}

__PUB_API__ bool cl_plugin_argument_bool(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, false);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return false;

    return CL_OBJECT_AS_BOOLEAN(value);
}

__PUB_API__ char *cl_plugin_argument_string(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, NULL);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return NULL;

    return CL_OBJECT_AS_STRING(value);
}

__PUB_API__ cl_string_t *cl_plugin_argument_cstring(const cl_plugin_arg_t *args,
    const char *argument_name)
{
    cl_object_t *value = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, NULL);
    value = cl_hashtable_get(args, argument_name);

    if (NULL == value)
        return NULL;

    return CL_OBJECT_AS_CSTRING(value);
}

__PUB_API__ int cl_plugin_argument_pointer(const cl_plugin_arg_t *args,
    const char *argument_name, void **ptr)
{
    cl_object_t *value = NULL;
    void *tmp = NULL;

    __clib_function_init__(true, args, CL_OBJ_HASHTABLE, -1);
    value = cl_hashtable_get(args, argument_name);
    printf("%s: Search for '%s'\n", __FUNCTION__, argument_name);

    if (NULL == value)
        return -1;

    tmp = CL_OBJECT_AS_POINTER(value);
    *ptr = tmp;

    return 0;
}

