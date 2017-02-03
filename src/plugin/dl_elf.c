
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
typedef const char *(*elf_plugin_info)(void);

/* Function name to get the internal plugin information */
#define CPLUGIN_SET_PLUGIN_ENTRY_NAME_SYMBOL    "cplugin_set_plugin_entry_name"

/*
 * Sets the startup and shutdown function pointers to the real functions
 * inside the plugin ELF. If they're not found, they become NULL and we
 * ignore them later.
 */
static void set_custom_plugin_info(cplugin_info_t *info, void *handle)
{
    struct elf_info *p = NULL;

    p = calloc(1, sizeof(struct elf_info));

    if (NULL == p)
        return;

    dlerror();
    p->startup = dlsym(handle, "plugin_init");

    if (dlerror() != NULL)
        p->startup = NULL;

    dlerror();
    p->shutdown = dlsym(handle, "plugin_uninit");

    if (dlerror() != NULL)
        p->shutdown = NULL;

    info_set_custom_data(info, p);
}

static void release_custom_plugin_info(struct elf_info *info)
{
    if (NULL == info)
        return;

    free(info);
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
    elf_plugin_info foo;
    cplugin_info_t *info = NULL;
    int i, t;
    struct plugin_internal_function functions[] = {
        { "plugin_name",        NULL },
        { "plugin_version",     NULL },
        { "plugin_author",      NULL },
        { "plugin_description", NULL },
        { "plugin_api",         NULL }
    };

    t = sizeof(functions) / sizeof(functions[0]);

    for (i = 0; i < t; i++) {
        dlerror();
        foo = dlsym(handle, functions[i].name);

        if (dlerror() != NULL)
            return NULL;

        functions[i].return_value = (char *)foo();
    }

    info = info_create_from_data(functions[0].return_value,
                                 functions[1].return_value,
                                 functions[2].return_value,
                                 functions[3].return_value,
                                 functions[4].return_value);

    if (info != NULL)
        set_custom_plugin_info(info, handle);

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

    if (plinfo->startup != NULL) {
        f = plinfo->startup;
        return f();
    }

    /* No startup function, ignores and continue execution. */
    return 0;
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
    if (plinfo->shutdown != NULL) {
        f = plinfo->shutdown;
        f();
    }

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

