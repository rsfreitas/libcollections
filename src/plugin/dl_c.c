
/*
 * Description: C/C++ plugin manipulation.
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

#include <stdio.h>
#include <dlfcn.h>

#include "collections.h"
#include "plugin.h"

/* Exported function prototype */
#define C_EXPORTED_FUNCTION                     \
    void (*)(uint32_t, cplugin_t *, cplugin_arg_t *)

/* Function name to get the internal plugin information */
#define CPLUGIN_SET_PLUGIN_ENTRY_NAME_SYMBOL    "cplugin_set_plugin_entry_name"

void c_library_init(void)
{
    /* noop */
}

void c_library_uninit(void)
{
    /* noop */
}

static struct cplugin_entry_s *call_entry_symbol(void *handle)
{
    struct cplugin_entry_s *(*pl_foo)(void), *entry = NULL;
    char *error = NULL;

    pl_foo = dlsym(handle, CPLUGIN_SET_PLUGIN_ENTRY_NAME_SYMBOL);

    if ((error = dlerror()) != NULL) {
        cset_errno(CL_ENTRY_SYMBOL_NOT_FOUND);
        return NULL;
    }

    entry = (pl_foo)();

    if (NULL == entry) {
        cset_errno(CL_NO_PLUGIN_INFO);
        return NULL;
    }

    return entry;
}

cplugin_info_t *c_load_info(void *handle)
{
    struct cplugin_entry_s *entry;

    entry = call_entry_symbol(handle);

    if (NULL == entry)
        return NULL;

    return info_create_from_entry(entry);
}

/*
 * Points to the real plugin function, inside it.
 */
static int c_load_function(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    void *handle = b;
    char *error = NULL;

    foo->p = dlsym(handle, foo->name);

    if ((error = dlerror()) != NULL) {
        cset_errno(CL_FUNCTION_SYMBOL_NOT_FOUND);
        return -1;
    }

    return 0;
}

int c_load_functions(struct cplugin_function_s *flist, void *handle)
{
    if (cdll_map(flist, c_load_function, handle) != NULL)
        return -1;

    return 0;
}

void *c_open(const char *pathname)
{
#ifdef DEBUG_LIBCOLLECTIONS
    void *p;

    p = dlopen(pathname, RTLD_LAZY);

    if (NULL == p)
        printf("%s: %s\n", __FUNCTION__, dlerror());

    return p;
#else
    return dlopen(pathname, RTLD_LAZY);
#endif
}

int c_close(void *handle)
{
    return dlclose(handle);
}

void c_call(struct cplugin_function_s *foo, uint32_t caller_id,
    struct cplugin_s *cpl)
{
    (foo->p)(caller_id, cpl, foo->args);
}

cplugin_internal_data_t *c_plugin_startup(void *handle)
{
    cplugin_internal_data_t *(*foo)(CPLUGIN_STARTUP_ARGS)=NULL;
    struct cplugin_entry_s *entry;

    entry = call_entry_symbol(handle);

    if (NULL == entry)
        return NULL;

    foo = entry->startup;

    /*
     * If the startup function is not found returns a valid pointer so we
     * prevent later errors, since this is not a mandatory function.
     */
    if (NULL == foo)
        return handle;

    return (foo)();
}

int c_plugin_shutdown(cplugin_internal_data_t *plugin_idata, void *handle)
{
    int (*foo)(CPLUGIN_SHUTDOWN_ARGS)=NULL;
    struct cplugin_entry_s *entry;

    entry = call_entry_symbol(handle);

    if (NULL == entry)
        return -1;

    foo = entry->shutdown;

    if (NULL == foo)
        return 0;

    return (foo)(plugin_idata);
}

