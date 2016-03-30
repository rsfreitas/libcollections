
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

#include "collections.h"

struct cvalue_s {
    enum cl_type        type;
    unsigned int        size;
    bool                dup_data;
    void                (*free_value)(void *);
    size_t              psize;
    cspec_t             *specs;

    /* data */
    char                c;
    unsigned char       uc;
    int                 i;
    unsigned int        ui;
    short int           si;
    unsigned short int  usi;
    float               f;
    double              d;
    long                l;
    unsigned long       ul;
    long long           ll;
    unsigned long long  ull;
    cstring_t           *s;
    bool                b;
    void                *p;

    /* ref count */
    struct ref_s        ref;
};

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
        cstring_unref(o->s);

    if (o->dup_data == true)
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
    o->dup_data = false;
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

void cvalue_set_sint(cvalue_t *value, short int si)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->si = si;
}

void cvalue_set_usint(cvalue_t *value, unsigned short int usi)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->usi = usi;
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

void cvalue_set_boolean(cvalue_t *value, bool b)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    v->b = b;
}

void cvalue_set_cstring(cvalue_t *value, cstring_t *s)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    if (v->s != NULL)
        cstring_unref(v->s);

    v->s = cstring_ref(s);
}

void cvalue_set_string(cvalue_t *value, char *s)
{
    struct cvalue_s *v = (struct cvalue_s *)value;

    if (NULL == value)
        return;

    if (v->s != NULL)
        cstring_unref(v->s);

    v->s = cstring_create("%s", s);
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

        case CL_SINT:
            cvalue_set_sint(o, (short int)va_arg(ap, int));
            break;

        case CL_USINT:
            cvalue_set_usint(o, (unsigned short int)va_arg(ap, unsigned int));
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

            if (o->dup_data == true) {
                if (o->p != NULL)
                    free_value_data(o);

                o->p = cmemdup(p, o->psize);
            } else
                o->p = p;

            p = va_arg(ap, void *);

            if (p != NULL)
                o->free_value = p;

            break;

        case CL_CSTRING:
            cvalue_set_cstring(o, va_arg(ap, void *));
            break;

        case CL_STRING:
            cvalue_set_string(o, va_arg(ap, char *));
            break;

        case CL_BOOLEAN:
            cvalue_set_boolean(o, va_arg(ap, int));
            break;
    }
}

int LIBEXPORT cvalue_set(cvalue_t *value, ...)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    va_list ap;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, NULL);

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, o, true, ap) == false)
            return -1;
    } else
        set_cvalue_value(o, ap);

    va_end(ap);

    return 0;
}

cvalue_t LIBEXPORT *cvalue_create(enum cl_type type, ...)
{
    struct cvalue_s *o = NULL;
    va_list ap;

    cerrno_clear();

    if (validate_cl_type(type) == false)
        return NULL;

    o = new_cvalue_s(type);

    if (NULL == o)
        return NULL;

    va_start(ap, NULL);
    set_cvalue_value(o, ap);
    va_end(ap);

    return o;
}

cvalue_t LIBEXPORT *cvalue_create_with_spec(enum cl_type type, cspec_t *spec)
{
    struct cvalue_s *o = NULL;

    cerrno_clear();

    if (validate_cl_type(type) == false)
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

int LIBEXPORT cvalue_sizeof(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    int s;

    cerrno_clear();

    if (NULL == value) {
        cvalue_unref(o);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s = get_cvalue_sizeof(o);
    cvalue_unref(o);

    return s;
}

enum cl_type LIBEXPORT cvalue_type(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    enum cl_type type;

    cerrno_clear();

    if (NULL == value) {
        cvalue_unref(o);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    type = o->type;
    cvalue_unref(o);

    return type;
}

static int get_value_check(const cvalue_t *value, enum cl_type type)
{
    struct cvalue_s *o = (struct cvalue_s *)value;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (cvalue_is_of_type(value, type) == false) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    if (o->specs != NULL) {
        if (cspec_validate(o->specs, (cvalue_t *)value, false,
                           0) == false)
        {
            cset_errno(CL_INVALID_VALUE);
            return -1;
        }
    }

    return 0;
}

int LIBEXPORT cvalue_get_int(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    int v = -1;

    if (get_value_check(value, CL_INT) == 0)
        v = o->i;

    cvalue_unref(o);

    return v;
}

unsigned int LIBEXPORT cvalue_get_uint(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    unsigned int v = 0;

    if (get_value_check(value, CL_UINT) == 0)
        v = o->ui;

    cvalue_unref(o);

    return v;
}

short int LIBEXPORT cvalue_get_sint(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    short int v = -1;

    if (get_value_check(value, CL_SINT) == 0)
        v = o->si;

    cvalue_unref(o);

    return v;
}

unsigned short int LIBEXPORT cvalue_get_usint(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    unsigned short int v = 0;

    if (get_value_check(value, CL_USINT) == 0)
        v = o->usi;

    cvalue_unref(o);

    return v;
}

float LIBEXPORT cvalue_get_float(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    float f = -1;

    if (get_value_check(value, CL_FLOAT) == 0)
        f = o->f;

    cvalue_unref(o);

    return f;
}

double LIBEXPORT cvalue_get_double(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    double d = -1;

    if (get_value_check(value, CL_DOUBLE) == 0)
        d = o->d;

    cvalue_unref(o);

    return d;
}

long long LIBEXPORT cvalue_get_llong(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    long long ll = -1;

    if (get_value_check(value, CL_LLONG) == 0)
        ll = o->ll;

    cvalue_unref(o);

    return ll;
}

unsigned long long LIBEXPORT cvalue_get_ullong(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    unsigned long long ull = 0;

    if (get_value_check(value, CL_ULLONG) == 0)
        ull = o->ull;

    cvalue_unref(o);

    return ull;
}

const char LIBEXPORT *cvalue_get_string(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    char *s = NULL;

    if (get_value_check(value, CL_STRING) == 0)
        s = (char *)cstring_valueof(o->s);

    cvalue_unref(o);

    return s;
}

cstring_t LIBEXPORT *cvalue_get_cstring(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    cstring_t *s = NULL;

    if (get_value_check(value, CL_CSTRING) == 0)
        s = cstring_ref(o->s);

    cvalue_unref(o);

    return s;
}

long LIBEXPORT cvalue_get_long(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    long l = -1;

    if (get_value_check(value, CL_LONG) == 0)
        l = o->l;

    cvalue_unref(o);

    return l;
}

unsigned long LIBEXPORT cvalue_get_ulong(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    unsigned long ul = 0;

    if (get_value_check(value, CL_ULONG) == 0)
        ul = o->ul;

    cvalue_unref(o);

    return ul;
}

char LIBEXPORT cvalue_get_char(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    char c = -1;

    if (get_value_check(value, CL_CHAR) == 0)
        c = o->c;

    cvalue_unref(o);

    return c;
}

unsigned char LIBEXPORT cvalue_get_uchar(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    unsigned char uc = 0;

    if (get_value_check(value, CL_UCHAR) == 0)
        uc = o->uc;

    cvalue_unref(o);

    return uc;
}

void LIBEXPORT *cvalue_get_pointer(const cvalue_t *value, unsigned int *size)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    void *p = NULL;

    cerrno_clear();

    if (NULL == value) {
        cvalue_unref(o);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (cvalue_is_of_type(o, CL_POINTER) == false) {
        cvalue_unref(o);
        cset_errno(CL_WRONG_TYPE);
        return NULL;
    }

    *size = o->psize;
    p = o->p;
    cvalue_unref(o);

    return p;
}

bool LIBEXPORT cvalue_get_boolean(const cvalue_t *value)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    bool b = -1;

    if (get_value_check(value, CL_BOOLEAN) == 0)
        b = o->b;

    cvalue_unref(o);

    return b;
}

bool LIBEXPORT cvalue_is_of_type(const cvalue_t *value, unsigned int type)
{
    struct cvalue_s *o = cvalue_ref((cvalue_t *)value);
    bool b;

    cerrno_clear();

    if (NULL == value) {
        cvalue_unref(o);
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (validate_cl_type(type) == false) {
        cvalue_unref(o);
        return false;
    }

    b = (o->type == type) ? true : false;
    cvalue_unref(o);

    return b;
}

static cstring_t *print_value(const struct cvalue_s *o)
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

cstring_t LIBEXPORT *cvalue_to_cstring(const cvalue_t *value)
{
    cvalue_t *ref = cvalue_ref((cvalue_t *)value);
    cstring_t *s = NULL;

    cerrno_clear();

    if (NULL == value) {
        cvalue_unref(ref);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    s = print_value(ref);
    cvalue_unref(ref);

    return s;
}

cvalue_t LIBEXPORT *cvalue_from_cstring(const cstring_t *value)
{
    cvalue_t *o = NULL;
    cstring_t *ref = NULL;

    cerrno_clear();

    if (NULL == value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ref = cstring_ref((cstring_t *)value);

    if (cstring_is_number(ref) == true)
        o = cvalue_create(CL_INT, cstring_to_int(ref));
    else if (cstring_is_float_number(ref) == true)
        o = cvalue_create(CL_FLOAT, cstring_to_int(ref));
    else
        o = cvalue_create(CL_STRING, ref);

    cstring_unref(ref);

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

