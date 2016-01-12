
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan  1 22:20:20 2016
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

struct spec_s {
    enum cl_param_flags properties;

    /* number */
    cvalue_t            *max;
    cvalue_t            *min;

    /* string */
    unsigned int        max_length;
};

static struct spec_s *new_spec_s(enum cl_param_flags properties,
    cvalue_t *min, cvalue_t *max, unsigned int max_length)
{
    struct spec_s *s = NULL;

    s = calloc(1, sizeof(struct spec_s *));

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    s->properties = properties;
    s->max = (max != NULL) ? cvalue_ref(max) : NULL;
    s->min = (min != NULL) ? cvalue_ref(min) : NULL;
    s->max_length = max_length;

    return s;
}

static void destroy_spec_s(struct spec_s *spec)
{
    if (NULL == spec)
        return;

    if (spec->min != NULL)
        cvalue_unref(spec->min);

    if (spec->max != NULL)
        cvalue_unref(spec->max);

    free(spec);
}

cspec_t LIBEXPORT *cspec_new(enum cl_param_flags properties,
    cvalue_t *min, cvalue_t *max, unsigned int max_length)
{
    cerrno_clear();

    if ((NULL == min) || (NULL == max) || (max_length == 0)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return new_spec_s(properties, min, max, max_length);
}

int LIBEXPORT cspec_destroy(cspec_t *spec)
{
    cerrno_clear();

    if (NULL == spec) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_spec_s(spec);

    return 0;
}

static cbool_t validate_accessibility(const struct spec_s *spec, cbool_t set)
{
    /* May write? */
    if ((set == CL_TRUE) && (!(spec->properties & CL_WRITABLE)))
        return CL_FALSE;

    /* May read? */
    if ((set == CL_FALSE) && (!(spec->properties & CL_READABLE)))
        return CL_FALSE;

    return CL_TRUE;
}

static cbool_t validate_char(const struct spec_s *spec, char value)
{
    char min, max;

    min = cvalue_get_char(spec->min);
    max = cvalue_get_char(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_uchar(const struct spec_s *spec, unsigned char value)
{
    unsigned char min, max;

    min = cvalue_get_uchar(spec->min);
    max = cvalue_get_uchar(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_int(const struct spec_s *spec, int value)
{
    int min, max;

    min = cvalue_get_int(spec->min);
    max = cvalue_get_int(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_uint(const struct spec_s *spec, unsigned int value)
{
    unsigned int min, max;

    min = cvalue_get_uint(spec->min);
    max = cvalue_get_uint(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_sint(const struct spec_s *spec, short int value)
{
    short int min, max;

    min = cvalue_get_sint(spec->min);
    max = cvalue_get_sint(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_usint(const struct spec_s *spec,
     unsigned short int value)
{
    unsigned short int min, max;

    min = cvalue_get_usint(spec->min);
    max = cvalue_get_usint(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_long(const struct spec_s *spec, long value)
{
    long min, max;

    min = cvalue_get_long(spec->min);
    max = cvalue_get_long(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_ulong(const struct spec_s *spec, unsigned long value)
{
    unsigned long min, max;

    min = cvalue_get_ulong(spec->min);
    max = cvalue_get_ulong(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_llong(const struct spec_s *spec, long long value)
{
    long long min, max;

    min = cvalue_get_llong(spec->min);
    max = cvalue_get_llong(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_ullong(const struct spec_s *spec,
    unsigned long long value)
{
    unsigned long long min, max;

    min = cvalue_get_ullong(spec->min);
    max = cvalue_get_ullong(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_float(const struct spec_s *spec, float value)
{
    float min, max;

    min = cvalue_get_float(spec->min);
    max = cvalue_get_float(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_double(const struct spec_s *spec, double value)
{
    double min, max;

    min = cvalue_get_double(spec->min);
    max = cvalue_get_double(spec->max);

    if ((value >= min) && (value <= max))
        return CL_TRUE;

    return CL_FALSE;
}

static cbool_t validate_boolean(cbool_t value)
{
    if ((value != CL_TRUE) && (value != CL_FALSE))
        return CL_FALSE;

    return CL_TRUE;
}

static cbool_t validate_string(const struct spec_s *spec, cstring_t *s)
{
    if ((unsigned int)cstring_length(s) > spec->max_length)
        return CL_FALSE;

    return CL_TRUE;
}

static cbool_t validate_value(const struct spec_s *spec, cvalue_t *value,
    va_list ap)
{
    cbool_t b, ret = CL_FALSE;
    char c;
    unsigned char uc;
    int i;
    unsigned int ui;
    short int si;
    unsigned short int usi;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;
    float f;
    double d;
    void *p;

    switch (cvalue_type(value)) {
        case CL_VOID:
        case CL_POINTER:
            /* noop */
            break;

        case CL_CHAR:
            c = (char)va_arg(ap, int);

            if (validate_char(spec, c) == CL_TRUE) {
                cvalue_set_char(value, c);
                ret = CL_TRUE;
            }

            break;

        case CL_UCHAR:
            uc = (unsigned char)va_arg(ap, int);

            if (validate_uchar(spec, uc) == CL_TRUE) {
                cvalue_set_uchar(value, uc);
                ret = CL_TRUE;
            }

            break;

        case CL_INT:
            i = va_arg(ap, int);

            if (validate_int(spec, i) == CL_TRUE) {
                cvalue_set_int(value, i);
                ret = CL_TRUE;
            }

            break;

        case CL_UINT:
            ui = va_arg(ap, unsigned int);

            if (validate_uint(spec, ui) == CL_TRUE) {
                cvalue_set_uint(value, ui);
                ret = CL_TRUE;
            }

            break;

        case CL_SINT:
            si = (short int)va_arg(ap, int);

            if (validate_sint(spec, si) == CL_TRUE) {
                cvalue_set_int(value, si);
                ret = CL_TRUE;
            }

            break;

        case CL_USINT:
            usi = (unsigned short int)va_arg(ap, unsigned int);

            if (validate_usint(spec, usi) == CL_TRUE) {
                cvalue_set_uint(value, usi);
                ret = CL_TRUE;
            }

            break;

        case CL_FLOAT:
            f = (float)va_arg(ap, double);

            if (validate_float(spec, f) == CL_TRUE) {
                cvalue_set_float(value, f);
                ret = CL_TRUE;
            }

            break;

        case CL_DOUBLE:
            d = va_arg(ap, double);

            if (validate_double(spec, d) == CL_TRUE) {
                cvalue_set_double(value, d);
                ret = CL_TRUE;
            }

            break;

        case CL_LONG:
            l = va_arg(ap, long);

            if (validate_long(spec, l) == CL_TRUE) {
                cvalue_set_long(value, l);
                ret = CL_TRUE;
            }

            break;

        case CL_ULONG:
            ul = va_arg(ap, unsigned long);

            if (validate_ulong(spec, ul) == CL_TRUE) {
                cvalue_set_ulong(value, ul);
                ret = CL_TRUE;
            }

            break;

        case CL_LLONG:
            ll = va_arg(ap, long long);

            if (validate_llong(spec, ll) == CL_TRUE) {
                cvalue_set_llong(value, ll);
                ret = CL_TRUE;
            }

            break;

        case CL_ULLONG:
            ull = va_arg(ap, unsigned long long);

            if (validate_ullong(spec, ull) == CL_TRUE) {
                cvalue_set_ullong(value, ull);
                ret = CL_TRUE;
            }

            break;

        case CL_STRING:
            p = va_arg(ap, void *);

            if (validate_string(spec, p) == CL_TRUE) {
                cvalue_set_string(value, p);
                ret = CL_TRUE;
            }

            break;

        case CL_BOOLEAN:
            b = va_arg(ap, int);

            if (validate_boolean(b) == CL_TRUE) {
                cvalue_set_boolean(value, b);
                ret = CL_TRUE;
            }

            break;
    }

    return ret;
}

cbool_t LIBEXPORT cspec_validate(const cspec_t *spec, cvalue_t *value,
    cbool_t set_value, va_list ap)
{
    cvalue_t *ref = cvalue_ref((cvalue_t *)value);
    cbool_t ret;

    cerrno_clear();

    if ((NULL == spec) || (NULL == value)) {
        cvalue_unref(ref);
        cset_errno(CL_NULL_ARG);
        return CL_FALSE;
    }

    ret = validate_accessibility(spec, set_value);

    if (ret == CL_FALSE) {
        cvalue_unref(ref);
        return CL_FALSE;
    } else {
        /* Doesn't need to parse while checking for trying to read value */
        if (set_value == CL_FALSE) {
            cvalue_unref(ref);
            return ret;
        }
    }

    ret = validate_value(spec, ref, ap);
    cvalue_unref(ref);

    return ret;
}

