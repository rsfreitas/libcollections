
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

/* supported languages */
#include "dl_c.h"
#include "dl_python.h"

struct dl_info {
#ifdef USE_LIBMAGIC
    magic_t         cookie;
#endif
    struct ref_s    ref;
};

static struct dl_info __dl = {
    .ref.count = 0,
};

static struct dl_plugin_driver __dl_driver[] = {
    {
        .type               = CPLUGIN_C,
        .library_init       = c_library_init,
        .library_uninit     = c_library_uninit,
        .load_info          = c_load_info,
        .load_functions     = c_load_functions,
        .open               = c_open,
        .close              = c_close,
        .call               = c_call,
        .plugin_startup     = c_plugin_startup,
        .plugin_shutdown    = c_plugin_shutdown,
    },

    {
        .type               = CPLUGIN_PYTHON,
        .library_init       = py_library_init,
        .library_uninit     = py_library_uninit,
        .load_info          = py_load_info,
        .load_functions     = py_load_functions,
        .open               = py_open,
        .close              = py_close,
        .call               = py_call,
        .plugin_startup     = py_plugin_startup,
        .plugin_shutdown    = py_plugin_shutdown,
    }
};

#define NDRIVERS            \
    (sizeof(__dl_driver) / sizeof(__dl_driver[0]))

static struct dl_plugin_driver *get_plugin_driver(enum cplugin_plugin_type type)
{
    int i = 0;

    for (i = NDRIVERS - 1; i >= 0; i--)
        if (__dl_driver[i].type == type)
            return &__dl_driver[i];

    return NULL;
}

static void __dl_library_uninit(const struct ref_s *ref __attribute__((unused)))
{
    int i = 0;

    /* call all drivers uninit function */
    for (i = NDRIVERS - 1; i >= 0; i--)
        (__dl_driver[i].library_uninit)();

#ifdef USE_LIBMAGIC
    magic_close(__dl.cookie);
#endif
}

static void dl_library_init(void)
{
    int old = 0, new = 1;
    unsigned int i = 0;

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

        /* call all drivers init function */
        for (i = 0; i < NDRIVERS; i++)
            (__dl_driver[i].library_init)();
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

struct dl_plugin_driver *dl_get_plugin_driver(const char *pathname)
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

    return get_plugin_driver(type);
}

/*
 * Load plugin to memory.
 */
void *dl_open(struct dl_plugin_driver *drv, const char *pathname)
{
    void *p = NULL;

    if (NULL == drv)
        goto end_block;

    p = (drv->open)(pathname);

end_block:
    if (NULL == p)
        cset_errno(CL_DLOPEN);

    return p;
}

/*
 * Release plugin from memory.
 */
int dl_close(struct dl_plugin_driver *drv, void *handle)
{
    int ret = -1;

    if ((NULL == drv) || (NULL == handle))
        goto end_block;

    ret = (drv->close)(handle);
    dl_library_uninit();

end_block:
    if (ret != 0)
        cset_errno(CL_DLCLOSE);

    return ret;
}

/*
 * Load plugin functions.
 */
int dl_load_functions(struct dl_plugin_driver *drv,
     struct cplugin_function_s *flist, void *handle)
{
    int ret = -1;

    if (NULL == drv)
        goto end_block;

    ret = (drv->load_functions)(flist, handle);

end_block:
    if (ret < 0)
        cset_errno(CL_OBJECT_NOT_FOUND);

    return ret;
}

/*
 * Load informations from a plugin.
 */
cplugin_info_t *dl_load_info(struct dl_plugin_driver *drv, void *handle)
{
    cplugin_info_t *info = NULL;

    if (NULL == drv)
        goto end_block;

    info = (drv->load_info)(handle);

end_block:
    if (NULL == info)
        cset_errno(CL_NO_PLUGIN_INFO);

    return info;
}

/*
 * Call the plugin startup function. It should return 0 on success or something
 * different otherwise.
 */
int dl_plugin_startup(struct dl_plugin_driver *drv, void *handle,
    cplugin_info_t *info)
{
    int ret = -1;

    if (NULL == drv)
        goto end_block;

    ret = (drv->plugin_startup)(handle, info);

end_block:
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

    if ((NULL == cpl) || (NULL == cpl->dl))
        goto end_block;

    ret = (cpl->dl->plugin_shutdown)(cpl->handle, cpl->info);

end_block:
    if (ret != 0)
        cset_errno(CL_PLUGIN_SHUTDOWN);

    return ret;
}

void dl_call(struct cplugin_s *cpl, struct cplugin_function_s *foo,
    uint32_t caller_id)
{
    if ((NULL == cpl) || (NULL == cpl->dl))
        return;

    (cpl->dl->call)(foo, caller_id, cpl);
}

