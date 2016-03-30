
/*
 * Description: Functions to manipulate internal plugin informations whether
 *              functions or data.
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

#include <stdlib.h>

#ifdef USE_LIBMAGIC
# include <magic.h>
#endif

#include "collections.h"
#include "plugin.h"

struct dl_info {
#ifdef USE_LIBMAGIC
    magic_t         cookie;
#endif
    struct ref_s    ref;
};

static struct dl_info __dl = {
    .ref.count = 0,
};

static void __dl_library_uninit(const struct ref_s *ref __attribute__((unused)))
{
    py_library_uninit();
    c_library_uninit();

    /* dl_uninit */
#ifdef USE_LIBMAGIC
    magic_close(__dl.cookie);
#endif
}

static void dl_library_init(void)
{
    int old = 0, new = 1;

    if (ref_bool_compare(&__dl.ref, old, new) == true) {
        srandom(time(NULL) + cseed());

#ifdef USE_LIBMAGIC
        __dl.cookie = magic_open(MAGIC_MIME_TYPE);

        if (NULL == __dl.cookie)
            return;

        if (magic_load(__dl.cookie, NULL) != 0) {
            magic_close(__dl.cookie);
            return;
        }
#endif

        __dl.ref.free = __dl_library_uninit;
        c_library_init();
        py_library_init();
    } else
        ref_inc(&__dl.ref);
}

static void dl_library_uninit(void)
{
    ref_dec(&__dl.ref);
}

static cstring_t *get_file_info(const char *filename)
{
    cstring_t *s = NULL;

#ifdef USE_LIBMAGIC
    s = cstring_create("%s", magic_file(__dl.cookie, filename));
#endif

    return s;
}

static enum cplugin_plugin_type parse_plugin_type(cstring_t *s)
{
    enum cplugin_plugin_type t = CPLUGIN_UNKNOWN;
    cstring_t *p = NULL;

    p = cstring_create("application/x-sharedlib");

    if (cstring_cmp(s, p) == 0) {
        t = CPLUGIN_C;
        goto ok;
    }

    cstring_clear(p);
    cstring_cat(p, "text/x-python");

    if (cstring_cmp(s, p) == 0) {
        t = CPLUGIN_PYTHON;
        goto ok;
    }

ok:
    if (p != NULL)
        cstring_unref(p);

    return t;
}

enum cplugin_plugin_type dl_get_plugin_type(const char *pathname)
{
    cstring_t *info = NULL;
    enum cplugin_plugin_type type;

    dl_library_init();
    info = get_file_info(pathname);

    if (NULL == info)
        return CPLUGIN_UNKNOWN;

    /* DEBUG */
    printf("%s: '%s'\n", __FUNCTION__, cstring_valueof(info));
    type = parse_plugin_type(info);
    cstring_unref(info);

    return type;
}

/*
 * Load plugin to memory.
 */
void *dl_open(const char *pathname, enum cplugin_plugin_type plugin_type)
{
    void *p = NULL;

    switch (plugin_type) {
        case CPLUGIN_C:
            p = c_open(pathname);
            break;

        case CPLUGIN_PYTHON:
            p = py_open(pathname);
            break;

        default:
            break;
    }

    if (NULL == p)
        cset_errno(CL_DLOPEN);

    return p;
}

/*
 * Release plugin from memory.
 */
int dl_close(void *handle, enum cplugin_plugin_type plugin_type)
{
    int ret = -1;

    if (NULL == handle)
        return 0;

    switch (plugin_type) {
        case CPLUGIN_C:
            ret = c_close(handle);
            break;

        case CPLUGIN_PYTHON:
            ret = py_close(handle);
            break;

        default:
            return 0;
    }

    dl_library_uninit();

    if (ret != 0)
        cset_errno(CL_DLCLOSE);

    return ret;
}

/*
 * Load plugin functions.
 */
int dl_load_functions(struct cplugin_function_s *flist, void *handle,
    enum cplugin_plugin_type plugin_type)
{
    int ret = -1;

    switch (plugin_type) {
        case CPLUGIN_C:
            ret = c_load_functions(flist, handle);
            break;

        case CPLUGIN_PYTHON:
            ret = py_load_functions(flist, handle);
            break;

        default:
            /* Does not have any function */
            ret = 0;
            break;
    }

    if (ret < 0)
        cset_errno(CL_OBJECT_NOT_FOUND);

    return ret;
}

/*
 * Load informations from a plugin.
 */
cplugin_info_t *dl_load_info(void *handle, enum cplugin_plugin_type plugin_type)
{
    cplugin_info_t *info = NULL;

    switch (plugin_type) {
        case CPLUGIN_C:
            info = c_load_info(handle);
            break;

        case CPLUGIN_PYTHON:
            info = py_load_info(handle);

        default:
            break;
    }

    if (NULL == info)
        cset_errno(CL_NO_PLUGIN_INFO);

    return info;
}

/*
 * Call the plugin startup function. It should return 0 on success or something
 * different otherwise.
 */
int dl_plugin_startup(void *handle, enum cplugin_plugin_type plugin_type,
    cplugin_info_t *info)
{
    int ret = -1;

    switch (plugin_type) {
        case CPLUGIN_C:
            ret = c_plugin_startup(info);
            break;

        case CPLUGIN_PYTHON:
            ret = py_plugin_startup(handle, info);
            break;

        default:
            break;
    }

    if (ret != 0)
        cset_errno(CL_PLUGIN_STARTUP);

    return ret;
}

/*
 * Call the plugin shutdown function. It should return 0 on success or
 * something different otherwise.
 */
int dl_plugin_shutdown(struct cplugin_s *cpl)
{
    int ret = -1;

    switch (cpl->type) {
        case CPLUGIN_C:
            ret = c_plugin_shutdown(cpl->info);
            break;

        case CPLUGIN_PYTHON:
            ret = py_plugin_shutdown(cpl->handle, cpl->info);
            break;

        default:
            break;
    }

    if (ret != 0)
        cset_errno(CL_PLUGIN_SHUTDOWN);

    return ret;
}

void dl_call(struct cplugin_function_s *foo, uint32_t caller_id,
    struct cplugin_s *cpl)
{
    switch (cpl->type) {
        case CPLUGIN_C:
            c_call(foo, caller_id, cpl);
            break;

        case CPLUGIN_PYTHON:
            py_call(foo, caller_id, cpl);
            break;

        default:
            break;
    }
}

