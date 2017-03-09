
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

/* Exported function prototypes */
typedef void (*elf_exported_function)(uint32_t, cplugin_t *, cplugin_arg_t *);
typedef int (*elf_startup_function)(void);
typedef void (*elf_shutdown_function)(void);
typedef const char *(*elf_plugin_info)(void);

/** void function classes by arguments */
typedef void (*elf_v_jptr)(const char *, void *);
typedef void (*elf_v_j)(const char *);
typedef void (*elf_v_ptr)(void *);
typedef void (*elf_v)(void);

/** char function classes by arguments */
typedef char (*elf_c_jptr)(const char *, void *);
typedef char (*elf_c_j)(const char *);
typedef char (*elf_c_ptr)(void *);
typedef char (*elf_c)(void);

/** unsigned char function classes by arguments */
typedef unsigned char (*elf_uc_jptr)(const char *, void *);
typedef unsigned char (*elf_uc_j)(const char *);
typedef unsigned char (*elf_uc_ptr)(void *);
typedef unsigned char (*elf_uc)(void);

/** int function classes by arguments */
typedef int (*elf_i_jptr)(const char *, void *);
typedef int (*elf_i_j)(const char *);
typedef int (*elf_i_ptr)(void *);
typedef int (*elf_i)(void);

/** unsigned int function classes by arguments */
typedef unsigned int (*elf_ui_jptr)(const char *, void *);
typedef unsigned int (*elf_ui_j)(const char *);
typedef unsigned int (*elf_ui_ptr)(void *);
typedef unsigned int (*elf_ui)(void);

/** short int function classes by arguments */
typedef short int (*elf_si_jptr)(const char *, void *);
typedef short int (*elf_si_j)(const char *);
typedef short int (*elf_si_ptr)(void *);
typedef short int (*elf_si)(void);

/** unsigned short int function classes by arguments */
typedef unsigned short int (*elf_usi_jptr)(const char *, void *);
typedef unsigned short int (*elf_usi_j)(const char *);
typedef unsigned short int (*elf_usi_ptr)(void *);
typedef unsigned short int (*elf_usi)(void);

/** long function classes by arguments */
typedef long (*elf_l_jptr)(const char *, void *);
typedef long (*elf_l_j)(const char *);
typedef long (*elf_l_ptr)(void *);
typedef long (*elf_l)(void);

/** unsigned long function classes by arguments */
typedef unsigned long (*elf_ul_jptr)(const char *, void *);
typedef unsigned long (*elf_ul_j)(const char *);
typedef unsigned long (*elf_ul_ptr)(void *);
typedef unsigned long (*elf_ul)(void);

/** long long function classes by arguments */
typedef long long (*elf_ll_jptr)(const char *, void *);
typedef long long (*elf_ll_j)(const char *);
typedef long long (*elf_ll_ptr)(void *);
typedef long long (*elf_ll)(void);

/** unsigned long long function classes by arguments */
typedef unsigned long long (*elf_ull_jptr)(const char *, void *);
typedef unsigned long long (*elf_ull_j)(const char *);
typedef unsigned long long (*elf_ull_ptr)(void *);
typedef unsigned long long (*elf_ull)(void);

/** float function classes by arguments */
typedef float (*elf_f_jptr)(const char *, void *);
typedef float (*elf_f_j)(const char *);
typedef float (*elf_f_ptr)(void *);
typedef float (*elf_f)(void);

/** double function classes by arguments */
typedef double (*elf_d_jptr)(const char *, void *);
typedef double (*elf_d_j)(const char *);
typedef double (*elf_d_ptr)(void *);
typedef double (*elf_d)(void);

/** string function classes by arguments */
typedef char *(*elf_cp_jptr)(const char *, void *);
typedef char *(*elf_cp_j)(const char *);
typedef char *(*elf_cp_ptr)(void *);
typedef char *(*elf_cp)(void);

/** pointer function classes by arguments */
typedef void *(*elf_p_jptr)(const char *, void *);
typedef void *(*elf_p_j)(const char *);
typedef void *(*elf_p_ptr)(void *);
typedef void *(*elf_p)(void);

/** boolean function classes by arguments */
typedef bool (*elf_b_jptr)(const char *, void *);
typedef bool (*elf_b_j)(const char *);
typedef bool (*elf_b_ptr)(void *);
typedef bool (*elf_b)(void);

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

static void elf_call_v(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    elf_v f;
    elf_v_ptr fptr;
    elf_v_j fj;
    elf_v_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            f();
            break;

        default:
            break;
    }
}

static char elf_call_c(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    char c = -1;
    elf_c f;
    elf_c_ptr fptr;
    elf_c_j fj;
    elf_c_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            c = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            c = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            c = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            c = f();
            break;

        default:
            break;
    }

    return c;
}

static unsigned char elf_call_uc(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    unsigned char c = 0;
    elf_uc f;
    elf_uc_ptr fptr;
    elf_uc_j fj;
    elf_uc_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            c = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            c = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            c = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            c = f();
            break;

        default:
            break;
    }

    return c;
}

static int elf_call_i(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    int i = -1;
    elf_i f;
    elf_i_ptr fptr;
    elf_i_j fj;
    elf_i_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            i = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            i = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            i = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            i = f();
            break;

        default:
            break;
    }

    return i;
}

static unsigned int elf_call_ui(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    unsigned int i = 0;
    elf_ui f;
    elf_ui_ptr fptr;
    elf_ui_j fj;
    elf_ui_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            i = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            i = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            i = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            i = f();
            break;

        default:
            break;
    }

    return i;
}

static short int elf_call_si(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    short int i = -1;
    elf_si f;
    elf_si_ptr fptr;
    elf_si_j fj;
    elf_si_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            i = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            i = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            i = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            i = f();
            break;

        default:
            break;
    }

    return i;
}

static unsigned short int elf_call_usi(struct cplugin_function_s *foo,
    const char *jarg, void *ptr_arg)
{
    unsigned short int i = 0;
    elf_usi f;
    elf_usi_ptr fptr;
    elf_usi_j fj;
    elf_usi_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            i = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            i = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            i = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            i = f();
            break;

        default:
            break;
    }

    return i;
}

static long elf_call_l(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    long l = -1;
    elf_l f;
    elf_l_ptr fptr;
    elf_l_j fj;
    elf_l_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            l = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            l = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            l = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            l = f();
            break;

        default:
            break;
    }

    return l;
}

static unsigned long elf_call_ul(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    unsigned long l = 0;
    elf_ul f;
    elf_ul_ptr fptr;
    elf_ul_j fj;
    elf_ul_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            l = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            l = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            l = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            l = f();
            break;

        default:
            break;
    }

    return l;
}

static long long elf_call_ll(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    long long l = -1;
    elf_ll f;
    elf_ll_ptr fptr;
    elf_ll_j fj;
    elf_ll_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            l = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            l = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            l = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            l = f();
            break;

        default:
            break;
    }

    return l;
}

static unsigned long long elf_call_ull(struct cplugin_function_s *foo,
    const char *jarg, void *ptr_arg)
{
    unsigned long long l = 0;
    elf_ull f;
    elf_ull_ptr fptr;
    elf_ull_j fj;
    elf_ull_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            l = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            l = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            l = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            l = f();
            break;

        default:
            break;
    }

    return l;
}

static float elf_call_f(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    float d = 0.0f;
    elf_f f;
    elf_f_ptr fptr;
    elf_f_j fj;
    elf_f_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            d = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            d = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            d = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            d = f();
            break;

        default:
            break;
    }

    return d;
}

static double elf_call_d(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    double d = 0.0;
    elf_d f;
    elf_d_ptr fptr;
    elf_d_j fj;
    elf_d_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            d = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            d = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            d = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            d = f();
            break;

        default:
            break;
    }

    return d;
}

static bool elf_call_b(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    bool b = false;
    elf_b f;
    elf_b_ptr fptr;
    elf_b_j fj;
    elf_b_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            b = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            b = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            b = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            b = f();
            break;

        default:
            break;
    }

    return b;
}

static char *elf_call_cp(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    char *ptr = NULL;
    elf_cp f;
    elf_cp_ptr fptr;
    elf_cp_j fj;
    elf_cp_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            ptr = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            ptr = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            ptr = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            ptr = f();
            break;

        default:
            break;
    }

    return ptr;
}

static void *elf_call_p(struct cplugin_function_s *foo, const char *jarg,
    void *ptr_arg)
{
    void *ptr = NULL;
    elf_p f;
    elf_p_ptr fptr;
    elf_p_j fj;
    elf_p_jptr fjptr;

    switch (foo->arg_mode) {
        case CPLUGIN_ARGS_POINTER_AND_ARGS:
            fjptr = foo->symbol;
            ptr = fjptr(jarg, ptr_arg);
            break;

        case CPLUGIN_ARGS_ONLY:
            fj = foo->symbol;
            ptr = fj(jarg);
            break;

        case CPLUGIN_ARGS_POINTER_ONLY:
            fptr = foo->symbol;
            ptr = fptr(ptr_arg);
            break;

        case CPLUGIN_ARGS_VOID:
            f = foo->symbol;
            ptr = f();
            break;

        default:
            break;
    }

    return ptr;
}

cobject_t *elf_call(void *data __attribute__((unused)),
    struct cplugin_function_s *foo, cplugin_t *cpl __attribute__((unused)),
    struct function_argument *args)
{
    cobject_t *ret = NULL;
    char *tmp;
    void *ptr;

    if (NULL == foo->symbol)
        /* do nothing */
        return NULL;

    ret = cobject_create_empty(foo->return_value);

    switch (foo->return_value) {
        case CL_VOID:
            elf_call_v(foo, args->jargs, args->ptr);
            break;

        case CL_CHAR:
            cobject_set_char(ret, elf_call_c(foo, args->jargs, args->ptr));
            break;

        case CL_UCHAR:
            cobject_set_uchar(ret, elf_call_uc(foo, args->jargs, args->ptr));
            break;

        case CL_INT:
            cobject_set_int(ret, elf_call_i(foo, args->jargs, args->ptr));
            break;

        case CL_UINT:
            cobject_set_uint(ret, elf_call_ui(foo, args->jargs, args->ptr));
            break;

        case CL_SINT:
            cobject_set_sint(ret, elf_call_si(foo, args->jargs, args->ptr));
            break;

        case CL_USINT:
            cobject_set_usint(ret, elf_call_usi(foo, args->jargs, args->ptr));
            break;

        case CL_FLOAT:
            cobject_set_float(ret, elf_call_f(foo, args->jargs, args->ptr));
            break;

        case CL_DOUBLE:
            cobject_set_double(ret, elf_call_d(foo, args->jargs, args->ptr));
            break;

        case CL_LONG:
            cobject_set_long(ret, elf_call_l(foo, args->jargs, args->ptr));
            break;

        case CL_ULONG:
            cobject_set_ulong(ret, elf_call_ul(foo, args->jargs, args->ptr));
            break;

        case CL_LLONG:
            cobject_set_llong(ret, elf_call_ll(foo, args->jargs, args->ptr));
            break;

        case CL_ULLONG:
            cobject_set_ullong(ret, elf_call_ull(foo, args->jargs, args->ptr));
            break;

        case CL_POINTER:
            ptr = elf_call_p(foo, args->jargs, args->ptr);
            cobject_set(ret, false, ptr, -1);
            break;

        case CL_STRING:
            tmp = elf_call_cp(foo, args->jargs, args->ptr);

            if (tmp != NULL) {
                cobject_set_string(ret, tmp);
                free(tmp);
            } else {
                /* We must destroy the object and return NULL to the caller */
                cobject_destroy(ret);
                ret = NULL;
            }

            break;

        case CL_BOOLEAN:
            cobject_set_boolean(ret, elf_call_b(foo, args->jargs, args->ptr));
            break;

        default:
            break;
    }

    return ret;
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

