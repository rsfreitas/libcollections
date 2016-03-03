
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

#include "collections.h"
#include "plugin.h"

static struct ref_s __lib_ref = {
    .count = 0,
};

static void __dl_library_uninit(const struct ref_s *ref __attribute__((unused)))
{
    c_library_uninit();
}

static void dl_library_init(void)
{
    int old = 0, new = 1;

    if (ref_bool_compare(&__lib_ref, old, new) == true) {
        __lib_ref.free = __dl_library_uninit;
        c_library_init();
    } else
        ref_inc(&__lib_ref);
}

static void dl_library_uninit(void)
{
    ref_dec(&__lib_ref);
}

/*
 * Load plugin to memory.
 */
void *dl_open(const char *pathname, enum cplugin_plugin_type plugin_type)
{
    void *p = NULL;

    dl_library_init();

    switch (plugin_type) {
        case CPLUGIN_C:
            p = c_open(pathname);
            break;
    }

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
    }

    dl_library_uninit();

    return ret;
}

/*
 * Load plugin functions.
 */
int dl_load_functions(struct cplugin_function_s *flist, void *handle,
    enum cplugin_plugin_type plugin_type)
{
    switch (plugin_type) {
        case CPLUGIN_C:
            return c_load_functions(flist, handle);
    }

    return 0;
}

/*
 * Load informations from a plugin.
 */
struct cplugin_info_s *dl_load_info(void *handle,
    enum cplugin_plugin_type plugin_type)
{
    struct cplugin_info_s *info = NULL;

    switch (plugin_type) {
        case CPLUGIN_C:
            info = c_load_info(handle);
            break;
    }

    return info;
}

/*
 * Call the plugin startup function. It should return 0 on success or something
 * different otherwise.
 */
cplugin_internal_data_t *dl_plugin_startup(void *handle,
    enum cplugin_plugin_type plugin_type)
{
    switch (plugin_type) {
        case CPLUGIN_C:
            return c_plugin_startup(handle);
    }

    return NULL;
}

/*
 * Call the plugin shutdown function. It should return 0 on success or
 * something different otherwise.
 */
int dl_plugin_shutdown(struct cplugin_s *cpl)
{
    switch (cpl->type) {
        case CPLUGIN_C:
            return c_plugin_shutdown(cpl->idata, cpl->handle);
    }

    return -1;
}

void dl_call(struct cplugin_function_s *foo, uint32_t caller_id,
    struct cplugin_s *cpl)
{
    switch (cpl->type) {
        case CPLUGIN_C:
            c_call(foo, caller_id, cpl);
            break;
    }
}

