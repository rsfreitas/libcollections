
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

struct cobject_s {
    enum cl_type    type;
    unsigned int    size;
    cbool_t         dup_data;
    void            (*free_object)(void *);

    /* data */
    char            c;
    int             i;
    float           f;
    double          d;
    long            l;
    long long       ll;
    cstring_t       *s;
    void            *p;
    size_t          psize;
};

static cbool_t validate_cl_type(enum cl_type type)
{
    if ((type == CL_CHAR) ||
        (type == CL_INT) ||
        (type == CL_FLOAT) ||
        (type == CL_DOUBLE) ||
        (type == CL_LONG) ||
        (type == CL_LLONG) ||
        (type == CL_VOIDP) ||
        (type == CL_STRING))
    {
        return CL_TRUE;
    }

    return CL_FALSE;
}

static struct cobject_s *new_cobject_s(enum cl_type type)
{
    struct cobject_s *o = NULL;

    o = calloc(1, sizeof(struct cobject_s));

    if (NULL == o) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    o->type = type;
    o->dup_data = CL_FALSE;

    return o;
}

static void free_object_data(struct cobject_s *o)
{
    if (o->free_object != NULL)
        (o->free_object)(o->p);
    else
        free(o->p);
}

static void destroy_cobject_s(struct cobject_s *o)
{
    if (NULL == o)
        return;

    if ((o->type == CL_STRING) && (o->s != NULL))
        cstring_destroy(o->s);

    if (o->dup_data == CL_TRUE)
        free_object_data(o);

    free(o);
}

static void set_cobject_value(struct cobject_s *o, va_list ap)
{
    char *s = NULL;
    void *p;

    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            o->c = (char)va_arg(ap, int);
            break;

        case CL_INT:
            o->i = va_arg(ap, int);
            break;

        case CL_FLOAT:
            o->f = (float)va_arg(ap, double);
            break;

        case CL_DOUBLE:
            o->d = va_arg(ap, double);
            break;

        case CL_LONG:
            o->l = va_arg(ap, long);
            break;

        case CL_LLONG:
            o->ll = va_arg(ap, long long);
            break;

        case CL_VOIDP:
            /* Dup data flag, data, size, release function */
            o->dup_data = va_arg(ap, int);
            p = va_arg(ap, void *);
            o->psize = va_arg(ap, int);

            if (o->dup_data == CL_TRUE) {
                if (o->p != NULL)
                    free_object_data(o);

                o->p = cmemdup(p, o->psize);
            } else
                o->p = p;

            p = va_arg(ap, void *);

            if (p != NULL)
                o->free_object = p;

            break;

        case CL_STRING:
            s = va_arg(ap, char *);

            if (o->s != NULL)
                cstring_destroy(o->s);

            o->s = cstring_new("%s", s);
            break;
    }
}

static int get_cobject_sizeof(struct cobject_s *o)
{
    switch (o->type) {
        case CL_VOID:
            /* noop */
            break;

        case CL_CHAR:
            return sizeof(char);

        case CL_INT:
            return sizeof(int);

        case CL_FLOAT:
            return sizeof(float);

        case CL_DOUBLE:
            return sizeof(double);

        case CL_LONG:
            return sizeof(long);

        case CL_LLONG:
            return sizeof(long long);

        case CL_VOIDP:
            return o->psize;

        case CL_STRING:
            return cstring_length(o->s);
    }

    return -1;
}

cobject_t LIBEXPORT *cobject_new(enum cl_type type, ...)
{
    struct cobject_s *o = NULL;
    va_list ap;

    cerrno_clear();

    if (validate_cl_type(type) == CL_FALSE) {
        return NULL;
    }

    o = new_cobject_s(type);

    if (NULL == o)
        return NULL;

    va_start(ap, NULL);
    set_cobject_value(o, ap);
    va_end(ap);

    return o;
}

int LIBEXPORT cobject_destroy(cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_cobject_s(o);

    return 0;
}

int LIBEXPORT cobject_sizeof(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return get_cobject_sizeof(o);
}

enum cl_type LIBEXPORT cl_type(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->type;
}

int LIBEXPORT cobject_set(cobject_t *object, ...)
{
    struct cobject_s *o = (struct cobject_s *)object;
    va_list ap;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, NULL);
    set_cobject_value(o, ap);
    va_end(ap);

    return 0;
}

int LIBEXPORT cobject_get_int(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->i;
}

float LIBEXPORT cobject_get_float(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->f;
}

double LIBEXPORT cobject_get_double(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->d;
}

long long LIBEXPORT cobject_get_llong(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->ll;
}

const cstring_t LIBEXPORT *cobject_get_string(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return o->s;
}

long LIBEXPORT cobject_get_long(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->l;
}

char LIBEXPORT cobject_get_char(const cobject_t *object)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return o->c;
}

void LIBEXPORT *cobject_get(const cobject_t *object, unsigned int *size)
{
    struct cobject_s *o = (struct cobject_s *)object;

    cerrno_clear();

    if (NULL == object) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    *size = o->psize;

    return o->p;
}

cbool_t LIBEXPORT cobject_is_cl_type(int value)
{
    cerrno_clear();

    return validate_cl_type(value);
}

