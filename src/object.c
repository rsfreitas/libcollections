
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

#include "collections.h"

#define cobject_members                                 \
    cl_struct_member(enum cl_type, type)                \
    cl_struct_member(unsigned int, size)                \
    cl_struct_member(bool, dup_data)                    \
    cl_struct_member(void, (*free_object)(void *))      \
    cl_struct_member(size_t, psize)                     \
    cl_struct_member(cspec_t *, specs)                  \
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
    cl_struct_member(cstring_t *, s)                    \
    cl_struct_member(bool, b)                           \
    cl_struct_member(void *, p)                         \
    cl_struct_member(struct ref_s, ref)                 \
    cl_struct_member(bool, (*equals)(cobject_t *))      \
    cl_struct_member(int, (*compare_to)(cobject_t *))

cl_struct_declare(cobject_s, cobject_members);

#define cobject_s        cl_struct(cobject_s)

bool validate_cl_type(enum cl_type type)
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

static void free_object_data(cobject_s *o)
{
    if (o->free_object != NULL)
        (o->free_object)(o->p);
    else
        free(o->p);
}

static void destroy_cobject_s(const struct ref_s *ref)
{
    cobject_s *o = container_of(ref, cobject_s, ref);

    if (NULL == o)
        return;

    if (((o->type == CL_CSTRING) || (o->type == CL_STRING)) && (o->s != NULL))
        cstring_unref(o->s);

    if (o->dup_data == true)
        free_object_data(o);

    if (o->specs != NULL)
        cspec_destroy(o->specs);

    free(o);
}

static cobject_s *new_cobject_s(enum cl_type type)
{
    cobject_s *o = NULL;

    o = calloc(1, sizeof(cobject_s));

    if (NULL == o) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    o->type = type;
    o->dup_data = false;
    o->specs = NULL;

    /* ref count initialization */
    o->ref.free = destroy_cobject_s;
    o->ref.count = 1;

    set_typeof(COBJECT, o);

    return o;
}

void cobject_set_char(cobject_t *object, char c)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->c = c;
}

void cobject_set_uchar(cobject_t *object, unsigned char uc)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->uc = uc;
}

void cobject_set_int(cobject_t *object, int i)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->i = i;
}

void cobject_set_uint(cobject_t *object, unsigned int ui)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->ui = ui;
}

void cobject_set_sint(cobject_t *object, short int si)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->si = si;
}

void cobject_set_usint(cobject_t *object, unsigned short int usi)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->usi = usi;
}

void cobject_set_float(cobject_t *object, float f)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->f = f;
}

void cobject_set_double(cobject_t *object, double d)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->d = d;
}

void cobject_set_long(cobject_t *object, long l)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->l = l;
}

void cobject_set_ulong(cobject_t *object, unsigned long ul)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->ul = ul;
}

void cobject_set_llong(cobject_t *object, long long ll)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->ll = ll;
}

void cobject_set_ullong(cobject_t *object, unsigned long long ull)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->ull = ull;
}

void cobject_set_boolean(cobject_t *object, bool b)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    v->b = b;
}

void cobject_set_cstring(cobject_t *object, cstring_t *s)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    if (v->s != NULL)
        cstring_unref(v->s);

    v->s = cstring_ref(s);
}

void cobject_set_string(cobject_t *object, char *s)
{
    cobject_s *v = (cobject_s *)object;

    if (NULL == object)
        return;

    if (v->s != NULL)
        cstring_unref(v->s);

    v->s = cstring_create("%s", s);
}

static void set_cobject_object(cobject_s *o, va_list ap)
{
    void *p;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            cobject_set_char(o, (char)va_arg(ap, int));
            break;

        case CL_UCHAR:
            cobject_set_uchar(o, (unsigned char)va_arg(ap, int));
            break;

        case CL_INT:
            cobject_set_int(o, va_arg(ap, int));
            break;

        case CL_UINT:
            cobject_set_uint(o, va_arg(ap, unsigned int));
            break;

        case CL_SINT:
            cobject_set_sint(o, (short int)va_arg(ap, int));
            break;

        case CL_USINT:
            cobject_set_usint(o, (unsigned short int)va_arg(ap, unsigned int));
            break;

        case CL_FLOAT:
            cobject_set_float(o, (float)va_arg(ap, double));
            break;

        case CL_DOUBLE:
            cobject_set_double(o, va_arg(ap, double));
            break;

        case CL_LONG:
            cobject_set_long(o, va_arg(ap, long));
            break;

        case CL_ULONG:
            cobject_set_ulong(o, va_arg(ap, unsigned long));
            break;

        case CL_LLONG:
            cobject_set_llong(o, va_arg(ap, long long));
            break;

        case CL_ULLONG:
            cobject_set_ullong(o, va_arg(ap, unsigned long long));
            break;

        case CL_POINTER:
            /* Dup data flag, data, size, release function */
            o->dup_data = va_arg(ap, int);
            p = va_arg(ap, void *);
            o->psize = va_arg(ap, int);

            if (o->dup_data == true) {
                if (o->p != NULL)
                    free_object_data(o);

                o->p = cmemdup(p, o->psize);
            } else
                o->p = p;

            p = va_arg(ap, void *);

            if (p != NULL)
                o->free_object = p;

            break;

        case CL_CSTRING:
            cobject_set_cstring(o, va_arg(ap, void *));
            break;

        case CL_STRING:
            cobject_set_string(o, va_arg(ap, char *));
            break;

        case CL_BOOLEAN:
            cobject_set_boolean(o, va_arg(ap, int));
            break;
    }
}

int LIBEXPORT cobject_set(cobject_t *object, ...)
{
    cobject_s *o;
    va_list ap;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    o = cobject_ref(object);
    va_start(ap, NULL);

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, o, true, ap) == false)
            return -1;
    } else
        set_cobject_object(o, ap);

    va_end(ap);

    return 0;
}

cobject_t LIBEXPORT *cobject_create(enum cl_type type, ...)
{
    cobject_s *o = NULL;
    va_list ap;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_cl_type(type) == false)
        return NULL;

    o = new_cobject_s(type);

    if (NULL == o)
        return NULL;

    va_start(ap, NULL);
    set_cobject_object(o, ap);
    va_end(ap);

    return o;
}

cobject_t LIBEXPORT *cobject_create_with_spec(enum cl_type type, cspec_t *spec)
{
    cobject_s *o = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_cl_type(type) == false)
        return NULL;

    if (validate_object(spec, CSPEC) == false)
        return NULL;

    o = new_cobject_s(type);

    if (NULL == o)
        return NULL;

    o->specs = spec;

    return o;
}

int LIBEXPORT cobject_destroy(cobject_t *object)
{
    return cobject_unref(object);
}

static int get_cobject_sizeof(cobject_s *o)
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
            return cstring_length(o->s);
    }

    return -1;
}

int LIBEXPORT cobject_sizeof(const cobject_t *object)
{
    cobject_s *o;
    int s;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    o = cobject_ref((cobject_t *)object);
    s = get_cobject_sizeof(o);
    cobject_unref(o);

    return s;
}

enum cl_type LIBEXPORT cobject_type(const cobject_t *object)
{
    cobject_s *o;
    enum cl_type type;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    o = cobject_ref((cobject_t *)object);
    type = o->type;
    cobject_unref(o);

    return type;
}

static int get_object_check(const cobject_t *object, enum cl_type type)
{
    cobject_s *o = (cobject_s *)object;

    cerrno_clear();

    if (validate_object(object, COBJECT) == false)
        return -1;

    if (cobject_is_of_type(object, type) == false) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, (cobject_t *)object, false,
                           0) == false)
        {
            cset_errno(CL_INVALID_VALUE);
            return -1;
        }
    }

    return 0;
}

bool LIBEXPORT cobject_is_of_type(const cobject_t *object, unsigned int type)
{
    cobject_s *o;
    bool b;

    cerrno_clear();

    if (library_initialized() == false)
        return false;

    if (validate_object(object, COBJECT) == false)
        return false;

    if (validate_cl_type(type) == false)
        return false;

    o = cobject_ref((cobject_t *)object);
    b = (o->type == type) ? true : false;
    cobject_unref(o);

    return b;
}

static cstring_t *print_object(const cobject_s *o)
{
    cstring_t *s = NULL;
    char *tmp;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            s = cstring_create("%c", o->c);
            break;

        case CL_UCHAR:
            s = cstring_create("%c", o->uc);
            break;

        case CL_INT:
            s = cstring_create("%d", o->i);
            break;

        case CL_UINT:
            s = cstring_create("%u", o->ui);
            break;

        case CL_SINT:
            s = cstring_create("%d", o->si);
            break;

        case CL_USINT:
            s = cstring_create("%u", o->usi);
            break;

        case CL_FLOAT:
            s = cstring_create("%f", o->f);
            break;

        case CL_DOUBLE:
            s = cstring_create("%e", o->d);
            break;

        case CL_LONG:
            s = cstring_create("%ld", o->l);
            break;

        case CL_ULONG:
            s = cstring_create("%lu", o->ul);
            break;

        case CL_LLONG:
            s = cstring_create("%lld", o->ll);
            break;

        case CL_ULLONG:
            s = cstring_create("%llu", o->ull);
            break;

        case CL_POINTER:
            tmp = value_to_hex(o->p, o->psize);
            s = cstring_create("%s", tmp);
            free(tmp);
            break;

        case CL_STRING:
        case CL_CSTRING:
            s = cstring_dup(o->s);
            break;

        case CL_BOOLEAN:
            s = cstring_create("%s", (o->b == true ? "true" : "false"));
            break;
    }

    return s;
}

cstring_t LIBEXPORT *cobject_to_cstring(const cobject_t *object)
{
    cobject_t *ref;
    cstring_t *s = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(object, COBJECT) == false)
        return NULL;

    ref = cobject_ref((cobject_t *)object);
    s = print_object(ref);
    cobject_unref(ref);

    return s;
}

cobject_t LIBEXPORT *cobject_from_cstring(const cstring_t *object)
{
    cobject_t *o = NULL;
    cstring_t *ref = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(object, COBJECT) == false)
        return NULL;

    ref = cstring_ref((cstring_t *)object);

    if (cstring_is_number(ref) == true)
        o = cobject_create(CL_INT, cstring_to_int(ref));
    else if (cstring_is_float_number(ref) == true)
        o = cobject_create(CL_FLOAT, cstring_to_int(ref));
    else
        o = cobject_create(CL_STRING, ref);

    cstring_unref(ref);

    return o;
}

cobject_t LIBEXPORT *cobject_ref(cobject_t *object)
{
    cobject_s *v = (cobject_s *)object;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_object(object, COBJECT) == false)
        return NULL;

    ref_inc(&v->ref);

    return object;
}

int LIBEXPORT cobject_unref(cobject_t *object)
{
    cobject_s *v = (cobject_s *)object;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    ref_dec(&v->ref);

    return 0;
}

int LIBEXPORT cobject_get(const cobject_t *object, const char *fmt, ...)
{
    cobject_s *o;
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
    cstring_t *s;
    bool *b;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    o = cobject_ref((cobject_t *)object);
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
                if (get_object_check(object, CL_CSTRING) < 0)
                    goto end_block;

                pp = va_arg(ap, void **);
                s = cstring_ref(o->s);
                *pp = s;
                break;

            case 's':
                if (get_object_check(object, CL_STRING) < 0)
                    goto end_block;

                cc = va_arg(ap, char **);
                pc = strdup(cstring_valueof(o->s));
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
    cobject_unref((cobject_t *)object);

    return ret;
}

int LIBEXPORT cobject_set_equals(cobject_t *object,
    bool (*equals)(cobject_t *))
{
    cobject_s *v = (cobject_s *)object;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    if (NULL == equals) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    v->equals = equals;

    return 0;
}

bool LIBEXPORT cobject_equals(const cobject_t *ob1, const cobject_t *ob2)
{
    cobject_s *v = (cobject_s *)ob1;

    cerrno_clear();

    if (library_initialized() == false)
        return false;

    if ((validate_object(ob1, COBJECT) == false) ||
        (validate_object(ob2, COBJECT) == false))
    {
        return false;
    }

    if (NULL == v->equals) {
        cset_errno(CL_NULL_DATA);
        return false;
    }

    return (v->equals)((cobject_t *)ob2);
}

int LIBEXPORT cobject_set_compare_to(cobject_t *object,
    int (*compare_to)(cobject_t *))
{
    cobject_s *v = (cobject_s *)object;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(object, COBJECT) == false)
        return -1;

    if (NULL == compare_to) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    v->compare_to = compare_to;

    return 0;
}

int LIBEXPORT cobject_compare_to(const cobject_t *ob1, const cobject_t *ob2)
{
    cobject_s *v = (cobject_s *)ob1;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if ((validate_object(ob1, COBJECT) == false) ||
        (validate_object(ob2, COBJECT) == false))
    {
        return -1;
    }

    if (NULL == v->compare_to) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return (v->compare_to)((cobject_t *)ob2);
}

