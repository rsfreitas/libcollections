
/*
 * Description: Simple API to handle a generic value for store any type
 *              of data.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 17:11:23 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <stdarg.h>

#include "collections.h"

struct cvalue_s {
    enum cl_type        type;
    unsigned int        size;
    cbool_t             dup_data;
    void                (*free_value)(void *);
    size_t              psize;
    cspec_t             *specs;

    /* data */
    char                c;
    unsigned char       uc;
    int                 i;
    unsigned int        ui;
    float               f;
    double              d;
    long                l;
    unsigned long       ul;
    long long           ll;
    unsigned long long  ull;
    cstring_t           *s;
    cbool_t             b;
    void                *p;

    /* ref count */
    struct ref_s        ref;
};

static cbool_t validate_cl_type(enum cl_type type)
{
    if ((type == CL_VOID) ||
        (type == CL_CHAR) ||
        (type == CL_UCHAR) ||
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
        (type == CL_BOOLEAN))
    {
        return CL_TRUE;
    }

    cset_errno(CL_UNSUPPORTED_TYPE);

    return CL_FALSE;
}

static void free_value_data(struct cvalue_s *o)
{
    if (o->free_value != NULL)
        (o->free_value)(o->p);
    else
        free(o->p);
}

static void destroy_cvalue_s(const struct ref_s *ref)
{
    struct cvalue_s *o = container_of(ref, struct cvalue_s, ref);

    if (NULL == o)
        return;

    if ((o->type == CL_STRING) && (o->s != NULL))
        cstring_destroy(o->s);

    if (o->dup_data == CL_TRUE)
        free_value_data(o);

    if (o->specs != NULL)
        cspec_destroy(o->specs);

    free(o);
}

static struct cvalue_s *new_cvalue_s(enum cl_type type)
{
    struct cvalue_s *o = NULL;

    o = calloc(1, sizeof(struct cvalue_s));

    if (NULL == o) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    o->type = type;
    o->dup_data = CL_FALSE;
    o->specs = NULL;

    /* ref count initialization */
    o->ref.free = destroy_cvalue_s;
    o->ref.count = 1;

    return o;
}

void cvalue_set_char(cvalue_t *value, char c)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->c = c;
}

void cvalue_set_uchar(cvalue_t *value, unsigned char uc)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->uc = uc;
}

void cvalue_set_int(cvalue_t *value, int i)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->i = i;
}

void cvalue_set_uint(cvalue_t *value, unsigned int ui)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->ui = ui;
}

void cvalue_set_float(cvalue_t *value, float f)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->f = f;
}

void cvalue_set_double(cvalue_t *value, double d)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->d = d;
}

void cvalue_set_long(cvalue_t *value, long l)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->l = l;
}

void cvalue_set_ulong(cvalue_t *value, unsigned long ul)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->ul = ul;
}

void cvalue_set_llong(cvalue_t *value, long long ll)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->ll = ll;
}

void cvalue_set_ullong(cvalue_t *value, unsigned long long ull)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->ull = ull;
}

void cvalue_set_boolean(cvalue_t *value, cbool_t b)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->b = b;
}

void cvalue_set_string(cvalue_t *value, cstring_t *s)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    if (v->s != NULL)
        cstring_destroy(v->s);

    v->s = cstring_dup(s);
}

static void set_cvalue_value(struct cvalue_s *o, va_list ap)
{
    void *p;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            cvalue_set_char(o, (char)va_arg(ap, int));
            break;

        case CL_UCHAR:
            cvalue_set_uchar(o, (unsigned char)va_arg(ap, int));
            break;

        case CL_INT:
            cvalue_set_int(o, va_arg(ap, int));
            break;

        case CL_UINT:
            cvalue_set_uint(o, va_arg(ap, unsigned int));
            break;

        case CL_FLOAT:
            cvalue_set_float(o, (float)va_arg(ap, double));
            break;

        case CL_DOUBLE:
            cvalue_set_double(o, va_arg(ap, double));
            break;

        case CL_LONG:
            cvalue_set_long(o, va_arg(ap, long));
            break;

        case CL_ULONG:
            cvalue_set_ulong(o, va_arg(ap, unsigned long));
            break;

        case CL_LLONG:
            cvalue_set_llong(o, va_arg(ap, long long));
            break;

        case CL_ULLONG:
            cvalue_set_ullong(o, va_arg(ap, unsigned long long));
            break;

        case CL_POINTER:
            /* Dup data flag, data, size, release function */
            o->dup_data = va_arg(ap, int);
            p = va_arg(ap, void *);
            o->psize = va_arg(ap, int);

            if (o->dup_data == CL_TRUE) {
                if (o->p != NULL)
                    free_value_data(o);

                o->p = cmemdup(p, o->psize);
            } else
                o->p = p;

            p = va_arg(ap, void *);

            if (p != NULL)
                o->free_value = p;

            break;

        case CL_STRING:
            cvalue_set_string(o, va_arg(ap, void *));
            break;

        case CL_BOOLEAN:
            cvalue_set_boolean(o, va_arg(ap, int));
            break;
    }
}

int LIBEXPORT cvalue_set(cvalue_t *value, ...)
{
    struct cvalue_s *o = (struct cvalue_s *)value;
    va_list ap;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, NULL);

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, o, CL_TRUE, ap) == CL_FALSE)
            return -1;
    } else
        set_cvalue_value(o, ap);

    va_end(ap);

    return 0;
}
cvalue_t LIBEXPORT *cvalue_new(enum cl_type type, ...)
{
    struct cvalue_s *o = NULL;
    va_list ap;

    cerrno_clear();

    if (validate_cl_type(type) == CL_FALSE)
        return NULL;

    o = new_cvalue_s(type);

    if (NULL == o)
        return NULL;

    va_start(ap, NULL);
    set_cvalue_value(o, ap);
    va_end(ap);

    return o;
}

cvalue_t LIBEXPORT *cvalue_new_with_spec(enum cl_type type, cspec_t *spec)
{
    struct cvalue_s *o = NULL;

    cerrno_clear();

    if (validate_cl_type(type) == CL_FALSE)
        return NULL;

    if (NULL == spec) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = new_cvalue_s(type);

    if (NULL == o)
        return NULL;

    o->specs = spec;

    return o;
}

int LIBEXPORT cvalue_destroy(cvalue_t *value)
{
    return cvalue_unref(value);
}

static int get_cvalue_sizeof(struct cvalue_s *o)
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
            return cstring_length(o->s);
    }

    return -1;
}

int LIBEXPORT cvalue_sizeof(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return get_cvalue_sizeof(o);
}

enum cl_type LIBEXPORT cvalue_type(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->type;
}

static int get_value_check(const cvalue_t *value, enum cl_type type)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (cvalue_is_of_type(value, type) == CL_FALSE) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, (cvalue_t *)value, CL_FALSE,
                           0) == CL_FALSE)
        {
            cset_errno(CL_INVALID_VALUE);
            return -1;
        }
    }

    return 0;
}

int LIBEXPORT cvalue_get_int(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_INT) < 0)
        return -1;

    return o->i;
}

unsigned int LIBEXPORT cvalue_get_uint(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_UINT) < 0)
        return -1;

    return o->ui;
}

float LIBEXPORT cvalue_get_float(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_FLOAT) < 0)
        return -1;

    return o->f;
}

double LIBEXPORT cvalue_get_double(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_DOUBLE) < 0)
        return -1;

    return o->d;
}

long long LIBEXPORT cvalue_get_llong(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_LLONG) < 0)
        return -1;

    return o->ll;
}

unsigned long long LIBEXPORT cvalue_get_ullong(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_ULLONG) < 0)
        return -1;

    return o->ull;
}

const cstring_t LIBEXPORT *cvalue_get_string(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_STRING) < 0)
        return NULL;

    return o->s;
}

long LIBEXPORT cvalue_get_long(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_LONG) < 0)
        return -1;

    return o->l;
}

unsigned long LIBEXPORT cvalue_get_ulong(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_ULONG) < 0)
        return -1;

    return o->ul;
}

char LIBEXPORT cvalue_get_char(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_CHAR) < 0)
        return -1;

    return o->c;
}

unsigned char LIBEXPORT cvalue_get_uchar(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_UCHAR) < 0)
        return -1;

    return o->uc;
}

void LIBEXPORT *cvalue_get_pointer(const cvalue_t *value, unsigned int *size)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (cvalue_is_of_type(o, CL_POINTER) == CL_FALSE) {
        cset_errno(CL_WRONG_TYPE);
        return NULL;
    }

    *size = o->psize;

    return o->p;
}

cbool_t LIBEXPORT cvalue_get_boolean(const cvalue_t *value)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    if (get_value_check(value, CL_BOOLEAN) < 0)
        return -1;

    return o->b;
}

cbool_t LIBEXPORT cvalue_is_of_type(const cvalue_t *value, unsigned int type)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return CL_FALSE;
    }

    if (validate_cl_type(type) == CL_FALSE)
        return CL_FALSE;

    return o->type == type ? CL_TRUE : CL_FALSE;
}

static cstring_t *print_value(const struct cvalue_s *o)
{
    cstring_t *s = NULL;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            s = cstring_new("%c", o->c);
            break;

        case CL_UCHAR:
            s = cstring_new("%c", o->uc);
            break;

        case CL_INT:
            s = cstring_new("%d", o->i);
            break;

        case CL_UINT:
            s = cstring_new("%u", o->ui);
            break;

        case CL_FLOAT:
            s = cstring_new("%f", o->f);
            break;

        case CL_DOUBLE:
            s = cstring_new("%e", o->d);
            break;

        case CL_LONG:
            s = cstring_new("%ld", o->l);
            break;

        case CL_ULONG:
            s = cstring_new("%lu", o->ul);
            break;

        case CL_LLONG:
            s = cstring_new("%lld", o->ll);
            break;

        case CL_ULLONG:
            s = cstring_new("%llu", o->ull);
            break;

        case CL_POINTER:
            /* TODO */
            break;

        case CL_STRING:
            s = cstring_dup(o->s);
            break;

        case CL_BOOLEAN:
            s = cstring_new("%s", (o->b == CL_TRUE ? "true" : "false"));
            break;
    }

    return s;
}

cstring_t LIBEXPORT *cvalue_to_string(const cvalue_t *value)
{
    cstring_t *s = NULL;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    s = print_value(value);

    return s;
}

cvalue_t LIBEXPORT *cvalue_from_string(const cstring_t *value)
{
    cvalue_t *o = NULL;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (cstring_is_number(value) == CL_TRUE)
        o = cvalue_new(CL_INT, cstring_value_as_int(value));
    else if (cstring_is_float_number(value) == CL_TRUE)
        o = cvalue_new(CL_FLOAT, cstring_value_as_float(value));
    else
        o = cvalue_new(CL_STRING, value);

    return o;
}

cvalue_t LIBEXPORT *cvalue_ref(cvalue_t *value)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ref_inc(&v->ref);

    return value;
}

int LIBEXPORT cvalue_unref(cvalue_t *value)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ref_dec(&v->ref);

    return 0;
}

