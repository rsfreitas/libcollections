
/*
 * Description: Functions to manipulate internal plugin informations whether
 *              are they functions or data.
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

#include <math.h>

#include "collections.h"
#include "plugin.h"

/* supported languages */
#include "dl_elf.h"

#ifdef PYPLUGIN
# include "dl_python.h"
#endif

#ifdef JAVAPLUGIN
# include "dl_java.h"
#endif

struct dl_plugin {
    struct cl_ref_s     ref;
};

static struct dl_plugin __dl = {
    .ref.count  = 0,
};

static struct dl_plugin_driver __dl_driver[] = {
    {
        .type               = CL_PLUGIN_UNKNOWN,
        .enabled            = false,
    },

#ifdef PLUGIN_ELF
    {
        .type               = CL_PLUGIN_ELF,
        .enabled            = true,
        .plugin_test        = elf_plugin_test,
        .library_init       = elf_library_init,
        .library_uninit     = elf_library_uninit,
        .load_info          = elf_load_info,
        .open               = elf_open,
        .close              = elf_close,
        .call               = elf_call,
        .plugin_startup     = elf_plugin_startup,
        .plugin_shutdown    = elf_plugin_shutdown,
        .load_function      = elf_load_function,
        .data               = NULL,
    },
#endif

#ifdef PLUGIN_PYTHON
    {
        .type               = CL_PLUGIN_PYTHON,
        .enabled            = true,
        .plugin_test        = py_plugin_test,
        .library_init       = py_library_init,
        .library_uninit     = py_library_uninit,
        .load_info          = py_load_info,
        .open               = py_open,
        .close              = py_close,
        .call               = py_call,
        .plugin_startup     = py_plugin_startup,
        .plugin_shutdown    = py_plugin_shutdown,
        .load_function      = NULL,
        .data               = NULL,
    },
#endif

/*
 * Does not need to compile this with the python version from the library,
 * since we don't want another library dependency (libjvm).
 */
#ifdef PLUGIN_JAVA
    {
        .type               = CL_PLUGIN_JAVA,
        .enabled            = true,
        .plugin_test        = jni_plugin_test,
        .library_init       = jni_library_init,
        .library_uninit     = jni_library_uninit,
        .load_info          = jni_load_info,
        .open               = jni_open,
        .close              = jni_close,
        .call               = jni_call,
        .plugin_startup     = jni_plugin_startup,
        .plugin_shutdown    = jni_plugin_shutdown,
        .load_function      = NULL,
        .data               = NULL,
    }
#endif
};

#define NDRIVERS            \
    (sizeof(__dl_driver) / sizeof(__dl_driver[0]))

void dl_enable_plugin_types(enum cl_plugin_type types)
{
    unsigned int i = 0;

    for (i = 1; i < NDRIVERS; i++) {
        if (types & (int)pow(2, i))
            __dl_driver[i].enabled = true;
        else
            __dl_driver[i].enabled = false;
    }
}

bool dl_is_plugin_enabled(enum cl_plugin_type type)
{
    int index = 0, value = type;

    while (value >>= 1)
        index++;

    if ((unsigned int)index < NDRIVERS)
        if (__dl_driver[index].enabled == true)
            return true;

    return false;
}

static struct dl_plugin_driver *get_plugin_driver(enum cl_plugin_type type)
{
    int i = 0;

    for (i = NDRIVERS - 1; i >= 0; i--)
        if ((__dl_driver[i].type == type) && (__dl_driver[i].enabled == true))
            return &__dl_driver[i];

    return NULL;
}

static void __dl_library_uninit(const struct cl_ref_s *ref __attribute__((unused)))
{
    int i = 0;

    /* call all drivers uninit function */
    for (i = NDRIVERS - 1; i >= 0; i--)
        if (__dl_driver[i].enabled == true)
            (__dl_driver[i].library_uninit)(__dl_driver[i].data);
}

void dl_library_init(void)
{
    int old = 0, new = 1;
    unsigned int i = 0;

    if (cl_ref_bool_compare(&__dl.ref, old, new) == true) {
        __dl.ref.free = __dl_library_uninit;

        /* call all drivers init function */
        for (i = 0; i < NDRIVERS; i++)
            if (__dl_driver[i].enabled == true)
                __dl_driver[i].data = (__dl_driver[i].library_init)();
    } else
        cl_ref_inc(&__dl.ref);
}

void dl_library_uninit(void)
{
    cl_ref_dec(&__dl.ref);
}

static cl_string_t *get_file_info(const char *filename)
{
    char *mime;
    cl_string_t *s = NULL;

    mime = library_file_mime_type(filename);
    s = cl_string_create("%s", mime);
    free(mime);

    return s;
}

static enum cl_plugin_type parse_plugin_type(cl_string_t *s)
{
    enum cl_plugin_type t = CL_PLUGIN_UNKNOWN;
    unsigned int i;

    for (i = 1; i < NDRIVERS; i++)
        if (__dl_driver[i].plugin_test(s) == true) {
            t = __dl_driver[i].type;
            break;
        }

    return t;
}

struct dl_plugin_driver *dl_get_plugin_driver(const char *pathname)
{
    cl_string_t *info = NULL;
    enum cl_plugin_type type;

    info = get_file_info(pathname);

    if (NULL == info)
        return NULL;

    /* DEBUG */
    type = parse_plugin_type(info);
    cl_string_unref(info);

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

    p = (drv->open)(drv->data, pathname);

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

    ret = (drv->close)(drv->data, handle);

end_block:
    if (ret != 0)
        cset_errno(CL_DLCLOSE);

    return ret;
}

/*
 * Load informations from a plugin.
 */
cl_plugin_info_t *dl_load_info(struct dl_plugin_driver *drv, void *handle)
{
    cl_plugin_info_t *info = NULL;

    if (NULL == drv)
        goto end_block;

    info = (drv->load_info)(drv->data, handle);

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
    cl_plugin_info_t *info)
{
    int ret = -1;

    if (NULL == drv)
        goto end_block;

    ret = (drv->plugin_startup)(drv->data, handle, info);

end_block:
    if (ret != 0)
        cset_errno(CL_PLUGIN_STARTUP);

    return ret;
}

/*
 * Call the plugin shutdown function. It should return 0 on success or
 * something different otherwise.
 */
int dl_plugin_shutdown(cplugin_s *cpl)
{
    int ret = -1;
    struct dl_plugin_driver *drv = NULL;

    if ((NULL == cpl) || (NULL == cpl->dl))
        goto end_block;

    drv = cpl->dl;
    ret = (drv->plugin_shutdown)(drv->data, cpl->handle, cpl->info);

end_block:
    if (ret != 0)
        cset_errno(CL_PLUGIN_SHUTDOWN);

    return ret;
}

cl_object_t *dl_call(cplugin_s *cpl, struct cplugin_function_s *foo)
{
    struct dl_plugin_driver *drv = NULL;

    if ((NULL == cpl) || (NULL == cpl->dl))
        return NULL;

    drv = cpl->dl;

    return (drv->call)(drv->data, foo, cpl);
}

struct cplugin_function_s *dl_load_function(cplugin_s *cpl,
    const char *name, enum cl_type return_type, int argc, va_list ap)
{
    struct dl_plugin_driver *drv = NULL;
    struct cplugin_function_s *foo = NULL;

    if ((NULL == cpl) || (NULL == cpl->dl))
        return NULL;

    drv = cpl->dl;
    foo = new_cplugin_function_s(name, return_type);

    if (NULL == foo)
        return NULL;

    if (arguments_parse(foo, argc, ap) < 0)
        goto end_block;

    if ((drv->load_function != NULL) &&
        ((drv->load_function)(drv->data, cpl->handle, foo) == 0))
    {
        return foo;
    }

end_block:
    destroy_cplugin_function_s(foo);
    return NULL;
}

