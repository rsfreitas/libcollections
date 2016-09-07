
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan  1 22:20:20 2016
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

#define cspec_members                                   \
    cl_struct_member(enum cl_param_flags, properties)   \
    cl_struct_member(cobject_t *, max)                  \
    cl_struct_member(cobject_t *, min)                  \
    cl_struct_member(unsigned int, max_length)

cl_struct_declare(cspec_s, cspec_members);

#define cspec_s             cl_struct(cspec_s)

static cspec_s *new_spec_s(enum cl_param_flags properties,
    cobject_t *min, cobject_t *max, unsigned int max_length)
{
    cspec_s *s = NULL;

    s = calloc(1, sizeof(cspec_s *));

    if (NULL == s) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    s->properties = properties;
    s->max = (max != NULL) ? cobject_ref(max) : NULL;
    s->min = (min != NULL) ? cobject_ref(min) : NULL;
    s->max_length = max_length;
    set_typeof(CSPEC, s);

    return s;
}

static void destroy_spec_s(cspec_s *spec)
{
    if (NULL == spec)
        return;

    if (spec->min != NULL)
        cobject_unref(spec->min);

    if (spec->max != NULL)
        cobject_unref(spec->max);

    free(spec);
}

cspec_t LIBEXPORT *cspec_create(enum cl_param_flags properties,
    cobject_t *min, cobject_t *max, unsigned int max_length)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if ((validate_object(min, COBJECT) == false) ||
        (validate_object(max, COBJECT) == false))
    {
        return NULL;
    }

    if (max_length == 0) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return new_spec_s(properties, min, max, max_length);
}

int LIBEXPORT cspec_destroy(cspec_t *spec)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(spec, CSPEC) == false)
        return -1;

    destroy_spec_s(spec);

    return 0;
}

static bool validate_accessibility(const cspec_s *spec, bool set)
{
    /* May write? */
    if ((set == true) && (!(spec->properties & CL_WRITABLE)))
        return false;

    /* May read? */
    if ((set == false) && (!(spec->properties & CL_READABLE)))
        return false;

    return true;
}

static bool validate_char(const cspec_s *spec, char value)
{
    char min, max;

    cobject_get(spec->min, COBJECT_CHAR, &min);
    cobject_get(spec->min, COBJECT_CHAR, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_uchar(const cspec_s *spec, unsigned char value)
{
    unsigned char min, max;

    cobject_get(spec->min, COBJECT_UCHAR, &min);
    cobject_get(spec->min, COBJECT_UCHAR, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_int(const cspec_s *spec, int value)
{
    int min, max;

    cobject_get(spec->min, COBJECT_INT, &min);
    cobject_get(spec->min, COBJECT_INT, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_uint(const cspec_s *spec, unsigned int value)
{
    unsigned int min, max;

    cobject_get(spec->min, COBJECT_UINT, &min);
    cobject_get(spec->min, COBJECT_UINT, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_sint(const cspec_s *spec, short int value)
{
    short int min, max;

    cobject_get(spec->min, COBJECT_SINT, &min);
    cobject_get(spec->min, COBJECT_SINT, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_usint(const cspec_s *spec,
     unsigned short int value)
{
    unsigned short int min, max;

    cobject_get(spec->min, COBJECT_USINT, &min);
    cobject_get(spec->min, COBJECT_USINT, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_long(const cspec_s *spec, long value)
{
    long min, max;

    cobject_get(spec->min, COBJECT_LONG, &min);
    cobject_get(spec->min, COBJECT_LONG, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_ulong(const cspec_s *spec, unsigned long value)
{
    unsigned long min, max;

    cobject_get(spec->min, COBJECT_ULONG, &min);
    cobject_get(spec->min, COBJECT_ULONG, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_llong(const cspec_s *spec, long long value)
{
    long long min, max;

    cobject_get(spec->min, COBJECT_LLONG, &min);
    cobject_get(spec->min, COBJECT_LLONG, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_ullong(const cspec_s *spec,
    unsigned long long value)
{
    unsigned long long min, max;

    cobject_get(spec->min, COBJECT_ULLONG, &min);
    cobject_get(spec->min, COBJECT_ULLONG, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_float(const cspec_s *spec, float value)
{
    float min, max;

    cobject_get(spec->min, COBJECT_FLOAT, &min);
    cobject_get(spec->min, COBJECT_FLOAT, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_double(const cspec_s *spec, double value)
{
    double min, max;

    cobject_get(spec->min, COBJECT_DOUBLE, &min);
    cobject_get(spec->min, COBJECT_DOUBLE, &max);

    if ((value >= min) && (value <= max))
        return true;

    return false;
}

static bool validate_boolean(bool value)
{
    if ((value != true) && (value != false))
        return false;

    return true;
}

static bool validate_string(const cspec_s *spec, const char *s)
{
    if ((unsigned int)strlen(s) > spec->max_length)
        return false;

    return true;
}

static bool validate_cstring(const cspec_s *spec, cstring_t *s)
{
    if ((unsigned int)cstring_length(s) > spec->max_length)
        return false;

    return true;
}

static bool validate_value(const cspec_s *spec, cobject_t *value,
    va_list ap)
{
    bool b, ret = false;
    char c, *cp;
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

    switch (cobject_type(value)) {
        case CL_VOID:
        case CL_POINTER:
            /* noop */
            break;

        case CL_CHAR:
            c = (char)va_arg(ap, int);

            if (validate_char(spec, c) == true) {
                cobject_set_char(value, c);
                ret = true;
            }

            break;

        case CL_UCHAR:
            uc = (unsigned char)va_arg(ap, int);

            if (validate_uchar(spec, uc) == true) {
                cobject_set_uchar(value, uc);
                ret = true;
            }

            break;

        case CL_INT:
            i = va_arg(ap, int);

            if (validate_int(spec, i) == true) {
                cobject_set_int(value, i);
                ret = true;
            }

            break;

        case CL_UINT:
            ui = va_arg(ap, unsigned int);

            if (validate_uint(spec, ui) == true) {
                cobject_set_uint(value, ui);
                ret = true;
            }

            break;

        case CL_SINT:
            si = (short int)va_arg(ap, int);

            if (validate_sint(spec, si) == true) {
                cobject_set_int(value, si);
                ret = true;
            }

            break;

        case CL_USINT:
            usi = (unsigned short int)va_arg(ap, unsigned int);

            if (validate_usint(spec, usi) == true) {
                cobject_set_uint(value, usi);
                ret = true;
            }

            break;

        case CL_FLOAT:
            f = (float)va_arg(ap, double);

            if (validate_float(spec, f) == true) {
                cobject_set_float(value, f);
                ret = true;
            }

            break;

        case CL_DOUBLE:
            d = va_arg(ap, double);

            if (validate_double(spec, d) == true) {
                cobject_set_double(value, d);
                ret = true;
            }

            break;

        case CL_LONG:
            l = va_arg(ap, long);

            if (validate_long(spec, l) == true) {
                cobject_set_long(value, l);
                ret = true;
            }

            break;

        case CL_ULONG:
            ul = va_arg(ap, unsigned long);

            if (validate_ulong(spec, ul) == true) {
                cobject_set_ulong(value, ul);
                ret = true;
            }

            break;

        case CL_LLONG:
            ll = va_arg(ap, long long);

            if (validate_llong(spec, ll) == true) {
                cobject_set_llong(value, ll);
                ret = true;
            }

            break;

        case CL_ULLONG:
            ull = va_arg(ap, unsigned long long);

            if (validate_ullong(spec, ull) == true) {
                cobject_set_ullong(value, ull);
                ret = true;
            }

            break;

        case CL_CSTRING:
            p = va_arg(ap, void *);

            if (validate_cstring(spec, p) == true) {
                cobject_set_cstring(value, p);
                ret = true;
            }

            break;

        case CL_STRING:
            cp = va_arg(ap, char *);

            if (validate_string(spec, cp) == true) {
                cobject_set_string(value, cp);
                ret = true;
            }

            break;

        case CL_BOOLEAN:
            b = va_arg(ap, int);

            if (validate_boolean(b) == true) {
                cobject_set_boolean(value, b);
                ret = true;
            }

            break;
    }

    return ret;
}

bool LIBEXPORT cspec_validate(const cspec_t *spec, cobject_t *value,
    bool set_value, va_list ap)
{
    cobject_t *ref;
    bool ret;

    cerrno_clear();

    if (library_initialized() == false)
        return false;

    if ((validate_object(spec, CSPEC) == false) ||
        (validate_object(value, COBJECT) == false))
    {
        return false;
    }

    ref = cobject_ref(value);
    ret = validate_accessibility(spec, set_value);

    if (ret == false) {
        cobject_unref(ref);
        return false;
    } else {
        /* Doesn't need to parse while checking for trying to read value */
        if (set_value == false) {
            cobject_unref(ref);
            return ret;
        }
    }

    ret = validate_value(spec, ref, ap);
    cobject_unref(ref);

    return ret;
}

