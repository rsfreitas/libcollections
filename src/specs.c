
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
    cl_struct_member(enum cl_spec_attrib, properties)   \
    cl_struct_member(cobject_t *, max)                  \
    cl_struct_member(cobject_t *, min)                  \
    cl_struct_member(unsigned int, max_length)

cl_struct_declare(cspec_s, cspec_members);

#define cspec_s             cl_struct(cspec_s)

static cspec_s *new_spec_s(enum cl_spec_attrib properties,
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

cspec_t LIBEXPORT *cspec_create(enum cl_spec_attrib properties,
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

static bool validate_char(const cspec_s *spec, char value,
    enum cl_spec_validation_fmt validation)
{
    char min, max;

    cobject_get(spec->min, COBJECT_CHAR, &min);
    cobject_get(spec->max, COBJECT_CHAR, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_uchar(const cspec_s *spec, unsigned char value,
    enum cl_spec_validation_fmt validation)
{
    unsigned char min, max;

    cobject_get(spec->min, COBJECT_UCHAR, &min);
    cobject_get(spec->max, COBJECT_UCHAR, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_int(const cspec_s *spec, int value,
    enum cl_spec_validation_fmt validation)
{
    int min, max;

    cobject_get(spec->min, COBJECT_INT, &min);
    cobject_get(spec->max, COBJECT_INT, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_uint(const cspec_s *spec, unsigned int value,
    enum cl_spec_validation_fmt validation)
{
    unsigned int min, max;

    cobject_get(spec->min, COBJECT_UINT, &min);
    cobject_get(spec->max, COBJECT_UINT, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_sint(const cspec_s *spec, short int value,
    enum cl_spec_validation_fmt validation)
{
    short int min, max;

    cobject_get(spec->min, COBJECT_SINT, &min);
    cobject_get(spec->max, COBJECT_SINT, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_usint(const cspec_s *spec,
     unsigned short int value, enum cl_spec_validation_fmt validation)
{
    unsigned short int min, max;

    cobject_get(spec->min, COBJECT_USINT, &min);
    cobject_get(spec->max, COBJECT_USINT, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_long(const cspec_s *spec, long value,
    enum cl_spec_validation_fmt validation)
{
    long min, max;

    cobject_get(spec->min, COBJECT_LONG, &min);
    cobject_get(spec->max, COBJECT_LONG, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_ulong(const cspec_s *spec, unsigned long value,
    enum cl_spec_validation_fmt validation)
{
    unsigned long min, max;

    cobject_get(spec->min, COBJECT_ULONG, &min);
    cobject_get(spec->max, COBJECT_ULONG, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_llong(const cspec_s *spec, long long value,
    enum cl_spec_validation_fmt validation)
{
    long long min, max;

    cobject_get(spec->min, COBJECT_LLONG, &min);
    cobject_get(spec->max, COBJECT_LLONG, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_ullong(const cspec_s *spec,
    unsigned long long value, enum cl_spec_validation_fmt validation)
{
    unsigned long long min, max;

    cobject_get(spec->min, COBJECT_ULLONG, &min);
    cobject_get(spec->max, COBJECT_ULLONG, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_float(const cspec_s *spec, float value,
    enum cl_spec_validation_fmt validation)
{
    float min, max;

    cobject_get(spec->min, COBJECT_FLOAT, &min);
    cobject_get(spec->max, COBJECT_FLOAT, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

    return false;
}

static bool validate_double(const cspec_s *spec, double value,
    enum cl_spec_validation_fmt validation)
{
    double min, max;

    cobject_get(spec->min, COBJECT_DOUBLE, &min);
    cobject_get(spec->max, COBJECT_DOUBLE, &max);

    switch (validation) {
        case CL_VALIDATE_IGNORED:
        case CL_VALIDATE_RANGE:
            if ((value >= min) && (value <= max))
                return true;

        case CL_VALIDATE_MIN_EQ:
            if (value == min)
                return true;

        case CL_VALIDATE_MIN_NE:
            if (value != min)
                return true;

        case CL_VALIDATE_MIN_LE:
            if (value <= min)
                return true;

        case CL_VALIDATE_MIN_LT:
            if (value < min)
                return true;

        case CL_VALIDATE_MIN_GE:
            if (value >= min)
                return true;

        case CL_VALIDATE_MIN_GT:
            if (value > min)
                return true;

        case CL_VALIDATE_MAX_EQ:
            if (value == max)
                return true;

        case CL_VALIDATE_MAX_NE:
            if (value != max)
                return true;

        case CL_VALIDATE_MAX_LE:
            if (value <= max)
                return true;

        case CL_VALIDATE_MAX_LT:
            if (value < max)
                return true;

        case CL_VALIDATE_MAX_GE:
            if (value >= max)
                return true;

        case CL_VALIDATE_MAX_GT:
            if (value > max)
                return true;

        default:
            return true;
    }

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
    enum cl_spec_validation_fmt validation, va_list ap)
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

            if (validate_char(spec, c, validation) == true) {
                cobject_set_char(value, c);
                ret = true;
            }

            break;

        case CL_UCHAR:
            uc = (unsigned char)va_arg(ap, int);

            if (validate_uchar(spec, uc, validation) == true) {
                cobject_set_uchar(value, uc);
                ret = true;
            }

            break;

        case CL_INT:
            i = va_arg(ap, int);

            if (validate_int(spec, i, validation) == true) {
                cobject_set_int(value, i);
                ret = true;
            }

            break;

        case CL_UINT:
            ui = va_arg(ap, unsigned int);

            if (validate_uint(spec, ui, validation) == true) {
                cobject_set_uint(value, ui);
                ret = true;
            }

            break;

        case CL_SINT:
            si = (short int)va_arg(ap, int);

            if (validate_sint(spec, si, validation) == true) {
                cobject_set_int(value, si);
                ret = true;
            }

            break;

        case CL_USINT:
            usi = (unsigned short int)va_arg(ap, unsigned int);

            if (validate_usint(spec, usi, validation) == true) {
                cobject_set_uint(value, usi);
                ret = true;
            }

            break;

        case CL_FLOAT:
            f = (float)va_arg(ap, double);

            if (validate_float(spec, f, validation) == true) {
                cobject_set_float(value, f);
                ret = true;
            }

            break;

        case CL_DOUBLE:
            d = va_arg(ap, double);

            if (validate_double(spec, d, validation) == true) {
                cobject_set_double(value, d);
                ret = true;
            }

            break;

        case CL_LONG:
            l = va_arg(ap, long);

            if (validate_long(spec, l, validation) == true) {
                cobject_set_long(value, l);
                ret = true;
            }

            break;

        case CL_ULONG:
            ul = va_arg(ap, unsigned long);

            if (validate_ulong(spec, ul, validation) == true) {
                cobject_set_ulong(value, ul);
                ret = true;
            }

            break;

        case CL_LLONG:
            ll = va_arg(ap, long long);

            if (validate_llong(spec, ll, validation) == true) {
                cobject_set_llong(value, ll);
                ret = true;
            }

            break;

        case CL_ULLONG:
            ull = va_arg(ap, unsigned long long);

            if (validate_ullong(spec, ull, validation) == true) {
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
    bool set_value, enum cl_spec_validation_fmt validation, va_list ap)
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

    ret = validate_value(spec, ref, validation, ap);
    cobject_unref(ref);

    return ret;
}

int LIBEXPORT cspec_set_min(cspec_t *spec, cobject_t *min)
{
    cspec_s *s = (cspec_s *)spec;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if ((validate_object(spec, CSPEC) == false) ||
        (validate_object(min, COBJECT) == false))
    {
        return -1;
    }

    cobject_unref(s->min);
    s->min = cobject_ref(min);

    return 0;
}

int LIBEXPORT cspec_set_max(cspec_t *spec, cobject_t *max)
{
    cspec_s *s = (cspec_s *)spec;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if ((validate_object(spec, CSPEC) == false) ||
        (validate_object(max, COBJECT) == false))
    {
        return -1;
    }

    cobject_unref(s->max);
    s->max = cobject_ref(max);

    return 0;
}

int LIBEXPORT cspec_set_max_length(cspec_t *spec, unsigned int max_length)
{
    cspec_s *s = (cspec_s *)spec;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(spec, CSPEC) == false)
        return -1;

    s->max_length = max_length;

    return 0;
}

int LIBEXPORT cspec_set_accessibility(cspec_t *spec, enum cl_spec_attrib attrib)
{
    cspec_s *s = (cspec_s *)spec;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (validate_object(spec, CSPEC) == false)
        return -1;

    s->properties = attrib;

    return 0;
}

