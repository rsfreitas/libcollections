
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
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>

#include "collections.h"
#include "plugin.h"

/* Structure to save custom plugin information. */
struct elf_info {
    int     (*startup)(void);
    void    (*shutdown)(void);
};

/* Exported function prototypes */
typedef int (*elf_startup_function)(void);
typedef void (*elf_shutdown_function)(void);
typedef const char *(*elf_plugin_info)(void);

/** plugin functions */
typedef void (*elf_v_ptr)(void *);
typedef char (*elf_c_ptr)(void *);
typedef unsigned char (*elf_uc_ptr)(void *);
typedef int (*elf_i_ptr)(void *);
typedef unsigned int (*elf_ui_ptr)(void *);
typedef short int (*elf_si_ptr)(void *);
typedef unsigned short int (*elf_usi_ptr)(void *);
typedef long (*elf_l_ptr)(void *);
typedef unsigned long (*elf_ul_ptr)(void *);
typedef long long (*elf_ll_ptr)(void *);
typedef unsigned long long (*elf_ull_ptr)(void *);
typedef float (*elf_f_ptr)(void *);
typedef double (*elf_d_ptr)(void *);
typedef char *(*elf_cp_ptr)(void *);
typedef void *(*elf_p_ptr)(void *);
typedef bool (*elf_b_ptr)(void *);

/*
 *
 * Internal functions
 *
 */

/*
 * Sets the startup and shutdown function pointers to the real functions
 * inside the plugin ELF. If they're not found, they become NULL and we
 * ignore them later.
 */
static void set_custom_plugin_info(cl_plugin_info_t *info, void *handle)
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

static void elf_call_v(struct cplugin_function_s *foo)
{
    elf_v_ptr fptr;

    fptr = foo->symbol;
    fptr(foo->arguments);
}

static char elf_call_c(struct cplugin_function_s *foo)
{
    char c = -1;
    elf_c_ptr fptr;

    fptr = foo->symbol;
    c = fptr(foo->arguments);

    return c;
}

static unsigned char elf_call_uc(struct cplugin_function_s *foo)
{
    unsigned char c = 0;
    elf_uc_ptr fptr;

    fptr = foo->symbol;
    c = fptr(foo->arguments);

    return c;
}

static int elf_call_i(struct cplugin_function_s *foo)
{
    int i = -1;
    elf_i_ptr fptr;

    fptr = foo->symbol;
    i = fptr(foo->arguments);

    return i;
}

static unsigned int elf_call_ui(struct cplugin_function_s *foo)
{
    unsigned int i = 0;
    elf_ui_ptr fptr;

    fptr = foo->symbol;
    i = fptr(foo->arguments);

    return i;
}

static short int elf_call_si(struct cplugin_function_s *foo)
{
    short int i = -1;
    elf_si_ptr fptr;

    fptr = foo->symbol;
    i = fptr(foo->arguments);

    return i;
}

static unsigned short int elf_call_usi(struct cplugin_function_s *foo)
{
    unsigned short int i = 0;
    elf_usi_ptr fptr;

    fptr = foo->symbol;
    i = fptr(foo->arguments);

    return i;
}

static long elf_call_l(struct cplugin_function_s *foo)
{
    long l = -1;
    elf_l_ptr fptr;

    fptr = foo->symbol;
    l = fptr(foo->arguments);

    return l;
}

static unsigned long elf_call_ul(struct cplugin_function_s *foo)
{
    unsigned long l = 0;
    elf_ul_ptr fptr;

    fptr = foo->symbol;
    l = fptr(foo->arguments);

    return l;
}

static long long elf_call_ll(struct cplugin_function_s *foo)
{
    long long l = -1;
    elf_ll_ptr fptr;

    fptr = foo->symbol;
    l = fptr(foo->arguments);

    return l;
}

static unsigned long long elf_call_ull(struct cplugin_function_s *foo)
{
    unsigned long long l = 0;
    elf_ull_ptr fptr;

    fptr = foo->symbol;
    l = fptr(foo->arguments);

    return l;
}

static float elf_call_f(struct cplugin_function_s *foo)
{
    float d = 0.0f;
    elf_f_ptr fptr;

    fptr = foo->symbol;
    d = fptr(foo->arguments);

    return d;
}

static double elf_call_d(struct cplugin_function_s *foo)
{
    double d = 0.0;
    elf_d_ptr fptr;

    fptr = foo->symbol;
    d = fptr(foo->arguments);

    return d;
}

static bool elf_call_b(struct cplugin_function_s *foo)
{
    bool b = false;
    elf_b_ptr fptr;

    fptr = foo->symbol;
    b = fptr(foo->arguments);

    return b;
}

static char *elf_call_cp(struct cplugin_function_s *foo)
{
    char *ptr = NULL;
    elf_cp_ptr fptr;

    fptr = foo->symbol;
    ptr = fptr(foo->arguments);

    return ptr;
}

static void *elf_call_p(struct cplugin_function_s *foo)
{
    void *ptr = NULL;
    elf_p_ptr fptr;

    fptr = foo->symbol;
    ptr = fptr(foo->arguments);

    return ptr;
}

/*
 *
 * Internal API
 *
 */

/*
 *
 * Plugin Driver API
 *
 */

CL_INTERNAL_API
void *elf_library_init(void)
{
    /* noop */
    return NULL;
}

CL_INTERNAL_API
void elf_library_uninit(void *data __attribute__((unused)))
{
    /* noop */
}

CL_INTERNAL_API
cl_plugin_info_t *elf_load_info(void *data __attribute__((unused)), void *handle)
{
    elf_plugin_info foo;
    cl_plugin_info_t *info = NULL;
    int i, t;
    struct plugin_internal_function functions[] = {
        { "plugin_name",        NULL },
        { "plugin_version",     NULL },
        { "plugin_author",      NULL },
        { "plugin_description", NULL }
    };

    t = sizeof(functions) / sizeof(functions[0]);

    for (i = 0; i < t; i++) {
        dlerror();
        foo = dlsym(handle, functions[i].name);

        if (dlerror() != NULL)
            return NULL;

        functions[i].return_value = strdup((char *)foo());
    }

    info = info_create_from_data(functions[0].return_value,
                                 functions[1].return_value,
                                 functions[2].return_value,
                                 functions[3].return_value);

    if (info != NULL)
        set_custom_plugin_info(info, handle);

    for (i = 0; i < t; i++)
        free(functions[i].return_value);

    return info;
}

CL_INTERNAL_API
void *elf_open(void *data __attribute__((unused)), const char *pathname)
{
#ifdef DEBUG
    void *p;

    p = dlopen(pathname, RTLD_LAZY);

    if (NULL == p)
        printf("%s: %s\n", __FUNCTION__, dlerror());

    return p;
#else
    return dlopen(pathname, RTLD_LAZY);
#endif
}

CL_INTERNAL_API
int elf_close(void *data __attribute__((unused)), void *handle)
{
    return dlclose(handle);
}

CL_INTERNAL_API
cl_object_t *elf_call(void *data __attribute__((unused)),
    struct cplugin_function_s *foo, cl_plugin_t *cpl __attribute__((unused)))
{
    cl_object_t *ret = NULL;
    char *tmp;
    void *ptr;

    if (NULL == foo->symbol)
        /* do nothing */
        return NULL;

    ret = cl_object_create_empty(foo->return_value);

    switch (foo->return_value) {
    case CL_VOID:
        elf_call_v(foo);
        break;

    case CL_CHAR:
        cl_object_set_char(ret, elf_call_c(foo));
        break;

    case CL_UCHAR:
        cl_object_set_uchar(ret, elf_call_uc(foo));
        break;

    case CL_INT:
        cl_object_set_int(ret, elf_call_i(foo));
        break;

    case CL_UINT:
        cl_object_set_uint(ret, elf_call_ui(foo));
        break;

    case CL_SINT:
        cl_object_set_sint(ret, elf_call_si(foo));
        break;

    case CL_USINT:
        cl_object_set_usint(ret, elf_call_usi(foo));
        break;

    case CL_FLOAT:
        cl_object_set_float(ret, elf_call_f(foo));
        break;

    case CL_DOUBLE:
        cl_object_set_double(ret, elf_call_d(foo));
        break;

    case CL_LONG:
        cl_object_set_long(ret, elf_call_l(foo));
        break;

    case CL_ULONG:
        cl_object_set_ulong(ret, elf_call_ul(foo));
        break;

    case CL_LLONG:
        cl_object_set_llong(ret, elf_call_ll(foo));
        break;

    case CL_ULLONG:
        cl_object_set_ullong(ret, elf_call_ull(foo));
        break;

    case CL_POINTER:
        ptr = elf_call_p(foo);
        cl_object_set(ret, false, ptr, -1, NULL, NULL);
        break;

    case CL_STRING:
        tmp = elf_call_cp(foo);

        if (tmp != NULL) {
            cl_object_set_string(ret, tmp);
            free(tmp);
        } else {
            /* We must destroy the object and return NULL to the caller */
            cl_object_destroy(ret);
            ret = NULL;
        }

        break;

    case CL_BOOLEAN:
        cl_object_set_boolean(ret, elf_call_b(foo));
        break;

    default:
        break;
    }

    return ret;
}

CL_INTERNAL_API
int elf_plugin_startup(void *data __attribute__((unused)),
    void *handle __attribute__((unused)), cl_plugin_info_t *info)
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

CL_INTERNAL_API
int elf_plugin_shutdown(void *data __attribute__((unused)),
    void *handle __attribute__((unused)), cl_plugin_info_t *info)
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

CL_INTERNAL_API
bool elf_plugin_test(const cl_string_t *mime)
{
    cl_string_t *p = NULL;
    bool ret = false;

    p = cl_string_create("application/x-sharedlib");

    if (cl_string_cmp(mime, p) == 0)
        ret = true;

    cl_string_unref(p);

    return ret;
}

CL_INTERNAL_API
int elf_load_function(void *data __attribute__((unused)), void *handle,
    struct cplugin_function_s *foo)
{
    char *error = NULL;

    foo->symbol = dlsym(handle, foo->name);

    if ((error = dlerror()) != NULL)
        return -1;

    return 0;
}

