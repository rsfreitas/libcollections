
/*
 * Description: Simple API to handle a generic object for store any type
 *              of data.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 17:11:23 2015
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
#include <string.h>
#include <locale.h>

#include "collections.h"

#define cl_object_members                               \
    cl_struct_member(enum cl_type, type)                \
    cl_struct_member(unsigned int, size)                \
    cl_struct_member(bool, dup_data)                    \
    cl_struct_member(void, (*free_object)(void *))      \
    cl_struct_member(size_t, psize)                     \
    cl_struct_member(cl_spec_t *, specs)                \
    cl_struct_member(char, c)                           \
    cl_struct_member(unsigned char, uc)                 \
    cl_struct_member(int, i)                            \
    cl_struct_member(unsigned int, ui)                  \
    cl_struct_member(short int, si)                     \
    cl_struct_member(unsigned short int, usi)           \
    cl_struct_member(float, f)                          \
    cl_struct_member(double, d)                         \
    cl_struct_member(long, l)                           \
    cl_struct_member(unsigned long, ul)                 \
    cl_struct_member(long long, ll)                     \
    cl_struct_member(unsigned long long, ull)           \
    cl_struct_member(cl_string_t *, s)                  \
    cl_struct_member(bool, b)                           \
    cl_struct_member(void *, p)                         \
    cl_struct_member(struct cl_ref_s, ref)              \
    cl_struct_member(bool, (*equals)(cl_object_t *))    \
    cl_struct_member(int, (*compare_to)(cl_object_t *))

cl_struct_declare(cl_object_s, cl_object_members);

#define cl_object_s        cl_struct(cl_object_s)

bool cl_object_is_valid(enum cl_type type)
{
    if ((type == CL_VOID) ||
        (type == CL_CHAR) ||
        (type == CL_UCHAR) ||
        (type == CL_SINT) ||
        (type == CL_USINT) ||
        (type == CL_INT) ||
        (type == CL_UINT) ||
        (type == CL_FLOAT) ||
        (type == CL_DOUBLE) ||
        (type == CL_LONG) ||
        (type == CL_ULONG) ||
        (type == CL_LLONG) ||
        (type == CL_ULLONG) ||
        (type == CL_POINTER) ||
        (type == CL_STRING) ||
        (type == CL_CSTRING) ||
        (type == CL_BOOLEAN))
    {
        return true;
    }

    cset_errno(CL_UNSUPPORTED_TYPE);

    return false;
}

static void free_object_data(cl_object_s *o)
{
    if (o->free_object != NULL)
        (o->free_object)(o->p);
    else
        free(o->p);
}

static void destroy_cl_object_s(const struct cl_ref_s *ref)
{
    cl_object_s *o = cl_container_of(ref, cl_object_s, ref);

    if (NULL == o)
        return;

    if (((o->type == CL_CSTRING) || (o->type == CL_STRING)) && (o->s != NULL))
        cl_string_unref(o->s);

    if (o->dup_data == true)
        free_object_data(o);

    if (o->specs != NULL)
        cl_spec_destroy(o->specs);

    free(o);
    o = NULL;
}

static cl_object_s *new_cl_object_s(enum cl_type type)
{
    cl_object_s *o = NULL;

    o = calloc(1, sizeof(cl_object_s));

    if (NULL == o) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    o->type = type;
    o->dup_data = false;
    o->specs = NULL;

    /* ref count initialization */
    o->ref.free = destroy_cl_object_s;
    o->ref.count = 1;

    typeof_set(CL_OBJ_OBJECT, o);

    return o;
}

void cl_object_set_char(cl_object_t *object, char c)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->c = c;
}

void cl_object_set_uchar(cl_object_t *object, unsigned char uc)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->uc = uc;
}

void cl_object_set_int(cl_object_t *object, int i)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->i = i;
}

void cl_object_set_uint(cl_object_t *object, unsigned int ui)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->ui = ui;
}

void cl_object_set_sint(cl_object_t *object, short int si)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->si = si;
}

void cl_object_set_usint(cl_object_t *object, unsigned short int usi)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->usi = usi;
}

void cl_object_set_float(cl_object_t *object, float f)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->f = f;
}

void cl_object_set_double(cl_object_t *object, double d)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->d = d;
}

void cl_object_set_long(cl_object_t *object, long l)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->l = l;
}

void cl_object_set_ulong(cl_object_t *object, unsigned long ul)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->ul = ul;
}

void cl_object_set_llong(cl_object_t *object, long long ll)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->ll = ll;
}

void cl_object_set_ullong(cl_object_t *object, unsigned long long ull)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->ull = ull;
}

void cl_object_set_boolean(cl_object_t *object, bool b)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    v->b = b;
}

void cl_object_set_cstring(cl_object_t *object, cl_string_t *s)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    if (v->s != NULL)
        cl_string_unref(v->s);

    v->s = cl_string_ref(s);
}

void cl_object_set_string(cl_object_t *object, char *s)
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    if (v->s != NULL)
        cl_string_unref(v->s);

    v->s = cl_string_create("%s", s);
}

void cl_object_set_pointer(cl_object_t *object, bool dup_data, void *data,
    unsigned int size, void (*free_object)(void *))
{
    cl_object_s *v = (cl_object_s *)object;

    if (NULL == object)
        return;

    if (v->s != NULL)
        cl_string_unref(v->s);

    v->dup_data = dup_data;
    v->psize = size;

    if (v->dup_data == true) {
        if (v->p != NULL)
            free_object_data(v);

        v->p = cl_memdup(data, v->psize);
    } else
        v->p = data;

    if (free_object != NULL)
        v->free_object = free_object;
}

static void set_cl_object_object(cl_object_s *o, va_list ap)
{
    void *p;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            cl_object_set_char(o, (char)va_arg(ap, int));
            break;

        case CL_UCHAR:
            cl_object_set_uchar(o, (unsigned char)va_arg(ap, int));
            break;

        case CL_INT:
            cl_object_set_int(o, va_arg(ap, int));
            break;

        case CL_UINT:
            cl_object_set_uint(o, va_arg(ap, unsigned int));
            break;

        case CL_SINT:
            cl_object_set_sint(o, (short int)va_arg(ap, int));
            break;

        case CL_USINT:
            cl_object_set_usint(o, (unsigned short int)va_arg(ap, unsigned int));
            break;

        case CL_FLOAT:
            cl_object_set_float(o, (float)va_arg(ap, double));
            break;

        case CL_DOUBLE:
            cl_object_set_double(o, va_arg(ap, double));
            break;

        case CL_LONG:
            cl_object_set_long(o, va_arg(ap, long));
            break;

        case CL_ULONG:
            cl_object_set_ulong(o, va_arg(ap, unsigned long));
            break;

        case CL_LLONG:
            cl_object_set_llong(o, va_arg(ap, long long));
            break;

        case CL_ULLONG:
            cl_object_set_ullong(o, va_arg(ap, unsigned long long));
            break;

        case CL_POINTER:
            /* Dup data flag, data, size, release function */
            o->dup_data = va_arg(ap, int);
            p = va_arg(ap, void *);
            o->psize = va_arg(ap, int);

            if (o->dup_data == true) {
                if (o->p != NULL)
                    free_object_data(o);

                o->p = cl_memdup(p, o->psize);
            } else
                o->p = p;

            /* Object free function */
            p = va_arg(ap, void *);

            if (p != NULL)
                o->free_object = p;

            break;

        case CL_CSTRING:
            cl_object_set_cstring(o, va_arg(ap, void *));
            break;

        case CL_STRING:
            cl_object_set_string(o, va_arg(ap, char *));
            break;

        case CL_BOOLEAN:
            cl_object_set_boolean(o, va_arg(ap, int));
            break;
    }
}

__PUB_API__ int cl_object_set(cl_object_t *object, ...)
{
    cl_object_s *o;
    va_list ap;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);
    o = cl_object_ref(object);
    va_start(ap, NULL);

    if (o->specs != NULL) {
        if (cl_spec_validate_va(o->specs, o, true, CL_VALIDATE_IGNORED,
                                ap) == false)
        {
            return -1;
        }
    } else
        set_cl_object_object(o, ap);

    va_end(ap);

    return 0;
}

__PUB_API__ cl_object_t *cl_object_create(enum cl_type type, ...)
{
    cl_object_s *o = NULL;
    va_list ap;

    __clib_function_init__(false, NULL, -1, NULL);

    if (cl_object_is_valid(type) == false)
        return NULL;

    o = new_cl_object_s(type);

    if (NULL == o)
        return NULL;

    va_start(ap, NULL);
    set_cl_object_object(o, ap);
    va_end(ap);

    return o;
}

__PUB_API__ cl_object_t *cl_object_vcreate(enum cl_type type, va_list ap)
{
    cl_object_s *o = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (cl_object_is_valid(type) == false)
        return NULL;

    o = new_cl_object_s(type);

    if (NULL == o)
        return NULL;

    set_cl_object_object(o, ap);

    return o;
}

__PUB_API__ cl_object_t *cl_object_create_empty(enum cl_type type)
{
    cl_object_s *o = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (cl_object_is_valid(type) == false)
        return NULL;

    o = new_cl_object_s(type);

    if (NULL == o)
        return NULL;

    return o;
}

__PUB_API__ cl_object_t *cl_object_create_with_spec(enum cl_type type,
    cl_spec_t *spec)
{
    cl_object_s *o = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (cl_object_is_valid(type) == false)
        return NULL;

    if (typeof_validate_object(spec, CL_OBJ_SPEC) == false)
        return NULL;

    o = new_cl_object_s(type);

    if (NULL == o)
        return NULL;

    o->specs = spec;

    return o;
}

__PUB_API__ int cl_object_destroy(cl_object_t *object)
{
    return cl_object_unref(object);
}

static int get_cl_object_sizeof(cl_object_s *o)
{
    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
        case CL_UCHAR:
            return sizeof(char);

        case CL_INT:
        case CL_UINT:
        case CL_BOOLEAN:
            return sizeof(int);

        case CL_SINT:
        case CL_USINT:
            return sizeof(short int);

        case CL_FLOAT:
            return sizeof(float);

        case CL_DOUBLE:
            return sizeof(double);

        case CL_LONG:
        case CL_ULONG:
            return sizeof(long);

        case CL_LLONG:
        case CL_ULLONG:
            return sizeof(long long);

        case CL_POINTER:
            return o->psize;

        case CL_STRING:
        case CL_CSTRING:
            return cl_string_length(o->s);
    }

    return -1;
}

__PUB_API__ int cl_object_sizeof(const cl_object_t *object)
{
    cl_object_s *o;
    int s;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);

    o = cl_object_ref((cl_object_t *)object);
    s = get_cl_object_sizeof(o);
    cl_object_unref(o);

    return s;
}

__PUB_API__ enum cl_type cl_object_type(const cl_object_t *object)
{
    cl_object_s *o;
    enum cl_type type;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);

    o = cl_object_ref((cl_object_t *)object);
    type = o->type;
    cl_object_unref(o);

    return type;
}

static int get_object_check(const cl_object_t *object, enum cl_type type)
{
    cl_object_s *o = (cl_object_s *)object;
    va_list ap;

    if (cl_object_is_of_type(object, type) == false) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    if (o->specs != NULL) {
        if (cl_spec_validate_va(o->specs, (cl_object_t *)object, false,
                                CL_VALIDATE_IGNORED, ap) == false)
        {
            cset_errno(CL_INVALID_VALUE);
            return -1;
        }
    }

    return 0;
}

__PUB_API__ bool cl_object_is_of_type(const cl_object_t *object,
    unsigned int type)
{
    cl_object_s *o;
    bool b;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, false);

    if (cl_object_is_valid(type) == false)
        return false;

    o = cl_object_ref((cl_object_t *)object);
    b = (o->type == type) ? true : false;
    cl_object_unref(o);

    return b;
}

static cl_string_t *print_object(const cl_object_s *o)
{
    cl_string_t *s = NULL;
    char *tmp, *old_locale = NULL;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            s = cl_string_create("%c", o->c);
            break;

        case CL_UCHAR:
            s = cl_string_create("%c", o->uc);
            break;

        case CL_INT:
            s = cl_string_create("%d", o->i);
            break;

        case CL_UINT:
            s = cl_string_create("%u", o->ui);
            break;

        case CL_SINT:
            s = cl_string_create("%d", o->si);
            break;

        case CL_USINT:
            s = cl_string_create("%u", o->usi);
            break;

        case CL_FLOAT:
            tmp = setlocale(LC_NUMERIC, NULL);

            if (tmp != NULL)
                old_locale = strdup(tmp);

            setlocale(LC_NUMERIC, "en_US.UTF-8");
            s = cl_string_create("%.2f", o->f);

            if (old_locale != NULL) {
                setlocale(LC_NUMERIC, old_locale);
                free(old_locale);
            }

            break;

        case CL_DOUBLE:
            s = cl_string_create("%e", o->d);
            break;

        case CL_LONG:
            s = cl_string_create("%ld", o->l);
            break;

        case CL_ULONG:
            s = cl_string_create("%lu", o->ul);
            break;

        case CL_LLONG:
            s = cl_string_create("%lld", o->ll);
            break;

        case CL_ULLONG:
            s = cl_string_create("%llu", o->ull);
            break;

        case CL_POINTER:
            tmp = value_to_hex(o->p, o->psize);
            s = cl_string_create("%s", tmp);
            free(tmp);
            break;

        case CL_STRING:
        case CL_CSTRING:
            s = cl_string_dup(o->s);
            break;

        case CL_BOOLEAN:
            s = cl_string_create("%s", (o->b == true ? "true" : "false"));
            break;
    }

    return s;
}

__PUB_API__ cl_string_t *cl_object_to_cstring(const cl_object_t *object)
{
    cl_object_t *ref;
    cl_string_t *s = NULL;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, NULL);

    ref = cl_object_ref((cl_object_t *)object);
    s = print_object(ref);
    cl_object_unref(ref);

    return s;
}

__PUB_API__ cl_object_t *cl_object_from_cstring(const cl_string_t *object)
{
    cl_object_t *o = NULL;
    cl_string_t *ref = NULL;

    __clib_function_init__(true, object, CL_OBJ_STRING, NULL);
    ref = cl_string_ref((cl_string_t *)object);

    /* Are we receiving an empty string? */
    if (cl_string_length(ref) == 0)
        o = cl_object_create(CL_STRING, "");
    else if (cl_string_is_number(ref) == true)
        o = cl_object_create(CL_INT, cl_string_to_int(ref));
    else if (cl_string_is_float_number(ref) == true)
        o = cl_object_create(CL_FLOAT, cl_string_to_float(ref));
    else
        o = cl_object_create(CL_STRING, cl_string_valueof(ref));

    cl_string_unref(ref);

    return o;
}

__PUB_API__ cl_object_t *cl_object_ref(cl_object_t *object)
{
    cl_object_s *v = (cl_object_s *)object;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, NULL);
    cl_ref_inc(&v->ref);

    return object;
}

__PUB_API__ int cl_object_unref(cl_object_t *object)
{
    cl_object_s *v = (cl_object_s *)object;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);
    cl_ref_dec(&v->ref);

    return 0;
}

__PUB_API__ int cl_object_get(const cl_object_t *object, const char *fmt, ...)
{
    cl_object_s *o;
    va_list ap;
    char *pc, c, **cc;
    unsigned char *uc;
    int ret = -1, *i;
    unsigned int *ui;
    short int *si;
    unsigned short int *usi;
    float *f;
    double *d;
    long *l;
    unsigned long *ul;
    long long *ll;
    unsigned long long *ull;
    void **pp;
    cl_string_t *s;
    bool *b;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);
    o = cl_object_ref((cl_object_t *)object);
    va_start(ap, fmt);

    while (*fmt) {
        switch (*fmt++) {
            case 'b':
                if (get_object_check(object, CL_CHAR) < 0)
                    goto end_block;

                c = o->c;
                pc = va_arg(ap, char *);
                memcpy(pc, &c, sizeof(char));
                break;

            case 'B':
                if (get_object_check(object, CL_UCHAR) < 0)
                    goto end_block;

                uc = (unsigned char *)va_arg(ap, char *);
                memcpy(uc, &o->uc, sizeof(unsigned char));
                break;

            case 'i':
                if (get_object_check(object, CL_INT) < 0)
                    goto end_block;

                i = va_arg(ap, int *);
                memcpy(i, &o->i, sizeof(int));
                break;

            case 'I':
                if (get_object_check(object, CL_UINT) < 0)
                    goto end_block;

                ui = (unsigned int *)va_arg(ap, int *);
                memcpy(ui, &o->ui, sizeof(unsigned int));
                break;

            case 'h':
                if (get_object_check(object, CL_SINT) < 0)
                    goto end_block;

                si = va_arg(ap, short *);
                memcpy(si, &o->si, sizeof(short int));
                break;

            case 'H':
                if (get_object_check(object, CL_USINT) < 0)
                    goto end_block;

                usi = (unsigned short int *)va_arg(ap, short *);
                memcpy(usi, &o->usi, sizeof(unsigned short int));
                break;

            case 'f':
                if (get_object_check(object, CL_FLOAT) < 0)
                    goto end_block;

                f = va_arg(ap, float *);
                memcpy(f, &o->f, sizeof(float));
                break;

            case 'd':
                if (get_object_check(object, CL_DOUBLE) < 0)
                    goto end_block;

                d = va_arg(ap, double *);
                memcpy(d, &o->d, sizeof(double));
                break;

            case 'l':
                if (get_object_check(object, CL_LONG) < 0)
                    goto end_block;

                l = va_arg(ap, long *);
                memcpy(l, &o->l, sizeof(long));
                break;

            case 'k':
                if (get_object_check(object, CL_ULONG) < 0)
                    goto end_block;

                ul = (unsigned long *)va_arg(ap, long *);
                memcpy(ul, &o->ul, sizeof(unsigned long));
                break;

            case 'L':
                if (get_object_check(object, CL_LLONG) < 0)
                    goto end_block;

                ll = va_arg(ap, long long *);
                memcpy(ll, &o->ll, sizeof(long long));
                break;

            case 'K':
                if (get_object_check(object, CL_ULLONG) < 0)
                    goto end_block;

                ull = (unsigned long long *)va_arg(ap, long long *);
                memcpy(ull, &o->ull, sizeof(unsigned long long));
                break;

            case 'Z':
                if (get_object_check(object, CL_BOOLEAN) < 0)
                    goto end_block;

                b = va_arg(ap, bool *);
                memcpy(b, &o->b, sizeof(bool));
                break;

            case 'z':
                /*
                 * If we're also holding a CL_STRING we can continue and
                 * return it.
                 */
                if ((get_object_check(object, CL_CSTRING) < 0) &&
                    (get_object_check(object, CL_STRING) < 0))
                {
                    goto end_block;
                }

                pp = va_arg(ap, void **);
                s = cl_string_ref(o->s);
                *pp = s;
                break;

            case 's':
                if (get_object_check(object, CL_STRING) < 0)
                    goto end_block;

                cc = va_arg(ap, char **);
                pc = strdup(cl_string_valueof(o->s));
                *cc = pc;
                break;

            case 'P':
                if (get_object_check(object, CL_POINTER) < 0)
                    goto end_block;

                pp = va_arg(ap, void **);
                i = va_arg(ap, int *);
                memcpy(i, &o->psize, sizeof(int));
                *pp = o->p;
                break;

            default:
                break;
        }
    }

    /* success */
    ret = 0;

end_block:
    va_end(ap);
    cl_object_unref((cl_object_t *)object);

    return ret;
}

__PUB_API__ int cl_object_set_equals(cl_object_t *object,
    bool (*equals)(cl_object_t *))
{
    cl_object_s *v = (cl_object_s *)object;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);

    if (NULL == equals) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    v->equals = equals;

    return 0;
}

static bool object_equals(const cl_object_s *o1, const cl_object_s *o2)
{
    bool ret = false;

    switch (o1->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_POINTER:
            if (NULL == o1->equals) {
                cset_errno(CL_NULL_DATA);
                return false;
            }

            ret = (o1->equals)((cl_object_t *)o2);
            break;

        case CL_CHAR:
            ret = (o1->c == o2->c);
            break;

        case CL_UCHAR:
            ret = (o1->uc == o2->uc);
            break;

        case CL_INT:
            ret = (o1->i == o2->i);
            break;

        case CL_UINT:
            ret = (o1->ui == o2->ui);
            break;

        case CL_SINT:
            ret = (o1->si == o2->si);
            break;

        case CL_USINT:
            ret = (o1->usi == o2->usi);
            break;

        case CL_FLOAT:
            ret = (o1->f == o2->f);
            break;

        case CL_DOUBLE:
            ret = (o1->d == o2->d);
            break;

        case CL_LONG:
            ret = (o1->l == o2->l);
            break;

        case CL_ULONG:
            ret = (o1->ul == o2->ul);
            break;

        case CL_LLONG:
            ret = (o1->ll == o2->ll);
            break;

        case CL_ULLONG:
            ret = (o1->ull == o2->ull);
            break;

        case CL_CSTRING:
        case CL_STRING:
            if (strcmp(cl_string_valueof(o1->s),
                       cl_string_valueof(o2->s)) == 0)
            {
                ret = true;
            }

            break;

        case CL_BOOLEAN:
            ret = (o1->b == o2->b);
            break;
    }

    return ret;
}

__PUB_API__ bool cl_object_equals(const cl_object_t *ob1, const cl_object_t *ob2)
{
    cl_object_s *v1 = (cl_object_s *)ob1,
                *v2 = (cl_object_s *)ob2;

    __clib_function_init__(false, NULL, -1, false);

    if ((typeof_validate_object(ob1, CL_OBJ_OBJECT) == false) ||
        (typeof_validate_object(ob2, CL_OBJ_OBJECT) == false))
    {
        return false;
    }

    /* We can compare only objects of the same type */
    if (v1->type != v2->type) {
        cset_errno(CL_INVALID_VALUE);
        return false;
    }

    return object_equals(v1, v2);
}

__PUB_API__ int cl_object_set_compare_to(cl_object_t *object,
    int (*compare_to)(cl_object_t *))
{
    cl_object_s *v = (cl_object_s *)object;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, -1);

    if (NULL == compare_to) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    v->compare_to = compare_to;

    return 0;
}

static int object_compare_to(const cl_object_s *o1, const cl_object_s *o2)
{
    int ret = -1;

    switch (o1->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_POINTER:
            if (NULL == o1->compare_to) {
                cset_errno(CL_NULL_DATA);
                return -1;
            }

            ret = (o1->compare_to)((cl_object_t *)o2);
            break;

        case CL_CHAR:
            ret = o1->c - o2->c;
            break;

        case CL_UCHAR:
            ret = o1->uc - o2->uc;
            break;

        case CL_INT:
            ret = o1->i - o2->i;
            break;

        case CL_UINT:
            ret = o1->ui - o2->ui;
            break;

        case CL_SINT:
            ret = o1->si - o2->si;
            break;

        case CL_USINT:
            ret = o1->usi - o2->usi;
            break;

        case CL_FLOAT:
            ret = o1->f - o2->f;
            break;

        case CL_DOUBLE:
            ret = o1->d - o2->d;
            break;

        case CL_LONG:
            ret = o1->l - o2->l;
            break;

        case CL_ULONG:
            ret = o1->ul - o2->ul;
            break;

        case CL_LLONG:
            ret = o1->ll - o2->ll;
            break;

        case CL_ULLONG:
            ret = o1->ull - o2->ull;
            break;

        case CL_CSTRING:
        case CL_STRING:
            ret = cl_string_cmp(o1->s, o2->s);
            break;

        case CL_BOOLEAN:
            /* We can't compare booleans like this */
            cset_errno(CL_INVALID_VALUE);
            break;
    }

    return ((ret < 0) ? -1 : ((ret > 0) ? ret : 0));
}

__PUB_API__ int cl_object_compare_to(const cl_object_t *ob1,
    const cl_object_t *ob2)
{
    cl_object_s *v1 = (cl_object_s *)ob1,
                *v2 = (cl_object_s *)ob2;;

    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object(ob1, CL_OBJ_OBJECT) == false) ||
        (typeof_validate_object(ob2, CL_OBJ_OBJECT) == false))
    {
        return -1;
    }

    /* We can compare only objects of the same type */
    if (v1->type != v2->type) {
        cset_errno(CL_INVALID_VALUE);
        return false;
    }

    return object_compare_to(v1, v2);
}

static void dup_object_content(cl_object_s *new, const cl_object_s *object)
{
    switch (new->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            cl_object_set(new, CL_OBJECT_AS_CHAR(object));
            break;

        case CL_UCHAR:
            cl_object_set(new, CL_OBJECT_AS_UCHAR(object));
            break;

        case CL_INT:
            cl_object_set(new, CL_OBJECT_AS_INT(object));
            break;

        case CL_UINT:
            cl_object_set(new, CL_OBJECT_AS_UINT(object));
            break;

        case CL_SINT:
            cl_object_set(new, CL_OBJECT_AS_SINT(object));
            break;

        case CL_USINT:
            cl_object_set(new, CL_OBJECT_AS_USINT(object));
            break;

        case CL_FLOAT:
            cl_object_set(new, CL_OBJECT_AS_FLOAT(object));
            break;

        case CL_DOUBLE:
            cl_object_set(new, CL_OBJECT_AS_DOUBLE(object));
            break;

        case CL_LONG:
            cl_object_set(new, CL_OBJECT_AS_LONG(object));
            break;

        case CL_ULONG:
            cl_object_set(new, CL_OBJECT_AS_ULONG(object));
            break;

        case CL_LLONG:
            cl_object_set(new, CL_OBJECT_AS_LLONG(object));
            break;

        case CL_ULLONG:
            cl_object_set(new, CL_OBJECT_AS_ULLONG(object));
            break;

        case CL_POINTER:
            /* TODO */
            break;

        case CL_STRING:
        case CL_CSTRING:
            new->s = cl_string_dup(object->s);
            break;

        case CL_BOOLEAN:
            cl_object_set(new, CL_OBJECT_AS_BOOLEAN(object));
            break;
    }
}

static void dup_object_events(cl_object_s *new, const cl_object_s *object)
{
    new->equals = object->equals;
    new->compare_to = object->compare_to;

    /* TODO: specs */
}

__PUB_API__ cl_object_t *cl_object_dup(const cl_object_t *object)
{
    cl_object_s *new = NULL;

    __clib_function_init__(true, object, CL_OBJ_OBJECT, NULL);
    new = new_cl_object_s(cl_object_type(object));

    if (NULL == new)
        return NULL;

    dup_object_content(new, object);
    dup_object_events(new, object);

    return new;
}

