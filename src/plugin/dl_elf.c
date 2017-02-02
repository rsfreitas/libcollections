
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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "collections.h"
#include "plugin.h"

/* Structure to save custom plugin informations. */
struct elf_info {
    int     (*startup)(void);
    void    (*shutdown)(void);
};

/* Exported functions prototype */
typedef void (*elf_exported_function)(uint32_t, cplugin_t *, cplugin_arg_t *);
typedef int (*elf_startup_function)(void);
typedef void (*elf_shutdown_function)(void);

/* Function name to get the internal plugin information */
#define CPLUGIN_SET_PLUGIN_ENTRY_NAME_SYMBOL    "cplugin_set_plugin_entry_name"

static void set_custom_plugin_info(cplugin_info_t *info,
    struct cplugin_entry_s *entry)
{
    struct elf_info *p = NULL;

    p = calloc(1, sizeof(struct elf_info));

    if (NULL == p)
        return;

    p->startup = entry->startup;
    p->shutdown = entry->shutdown;

    info_set_custom_data(info, p);
}

static void release_custom_plugin_info(struct elf_info *info)
{
    if (NULL == info)
        return;

    free(info);
}

static struct cplugin_entry_s *call_entry_symbol(void *handle)
{
    struct cplugin_entry_s *(*pl_foo)(void), *entry = NULL;
    char *error = NULL;

    pl_foo = dlsym(handle, CPLUGIN_SET_PLUGIN_ENTRY_NAME_SYMBOL);

    if ((error = dlerror()) != NULL)
        return NULL;

    entry = (pl_foo)();

    if (NULL == entry)
        return NULL;

    return entry;
}

/*
 * Points to the real plugin function, inside it.
 */
static int elf_load_function(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    void *handle = b;
    char *error = NULL;

    foo->symbol = dlsym(handle, foo->name);

    if ((error = dlerror()) != NULL)
        return -1;

    return 0;
}

/*
 *
 * Plugin Driver API
 *
 */

void *elf_library_init(void)
{
    /* noop */
    return NULL;
}

void elf_library_uninit(void *data __attribute__((unused)))
{
    /* noop */
}

cplugin_info_t *elf_load_info(void *data __attribute__((unused)), void *handle)
{
    struct cplugin_entry_s *entry;
    cplugin_info_t *info = NULL;

    entry = call_entry_symbol(handle);

    if (NULL == entry)
        return NULL;

    info = info_create_from_entry(entry);

    if (info != NULL)
        set_custom_plugin_info(info, entry);

    return info;
}

int elf_load_functions(void *data __attribute__((unused)),
     struct cplugin_function_s *flist, void *handle)
{
    if (cdll_map(flist, elf_load_function, handle) != NULL)
        return -1;

    return 0;
}

void *elf_open(void *data __attribute__((unused)), const char *pathname)
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

int elf_close(void *data __attribute__((unused)), void *handle)
{
    return dlclose(handle);
}

void elf_call(void *data __attribute__((unused)), struct cplugin_function_s *foo,
    uint32_t caller_id, cplugin_t *cpl)
{
    elf_exported_function f;

    if (NULL == foo->symbol)
        /* do nothing */
        return;

    f = foo->symbol;
    f(caller_id, cpl, foo->args);
}

int elf_plugin_startup(void *data __attribute__((unused)),
    void *handle __attribute__((unused)), cplugin_info_t *info)
{
    elf_startup_function f;
    struct elf_info *plinfo = NULL;

    plinfo = (struct elf_info *)info_get_custom_data(info);

    if (NULL == plinfo)
        return -1;

    f = plinfo->startup;

    return f();
}

int elf_plugin_shutdown(void *data __attribute__((unused)),
    void *handle __attribute__((unused)), cplugin_info_t *info)
{
    elf_shutdown_function f;
    struct elf_info *plinfo = NULL;

    plinfo = (struct elf_info *)info_get_custom_data(info);

    if (NULL == plinfo)
        return -1;

    /* call the plugin shutdown function */
    f = plinfo->shutdown;
    f();
    release_custom_plugin_info(plinfo);

    return 0;
}

bool elf_plugin_test(const cstring_t *mime)
{
    cstring_t *p = NULL;
    bool ret = false;

    p = cstring_create("application/x-sharedlib");

    if (cstring_cmp(mime, p) == 0)
        ret = true;

    cstring_unref(p);

    return ret;
}

