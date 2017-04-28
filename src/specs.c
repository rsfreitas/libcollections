
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

#define cl_spec_members                                 \
    cl_struct_member(enum cl_spec_attrib, properties)   \
    cl_struct_member(cl_object_t *, max)                \
    cl_struct_member(cl_object_t *, min)                \
    cl_struct_member(unsigned int, max_length)

cl_struct_declare(cl_spec_s, cl_spec_members);

#define cl_spec_s             cl_struct(cl_spec_s)

static cl_spec_s *new_spec_s(enum cl_spec_attrib properties,
    cl_object_t *min, cl_object_t *max, unsigned int max_length)
{
    cl_spec_s *s = NULL;

    s = calloc(1, sizeof(cl_spec_s *));

    if (NULL == s) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    s->properties = properties;
    s->max = (max != NULL) ? cl_object_ref(max) : NULL;
    s->min = (min != NULL) ? cl_object_ref(min) : NULL;
    s->max_length = max_length;
    set_typeof(CL_OBJ_SPEC, s);

    return s;
}

static void destroy_spec_s(cl_spec_s *spec)
{
    if (NULL == spec)
        return;

    if (spec->min != NULL)
        cl_object_unref(spec->min);

    if (spec->max != NULL)
        cl_object_unref(spec->max);

    free(spec);
}

__PUB_API__ cl_spec_t *cl_spec_create(enum cl_spec_attrib properties,
    cl_object_t *min, cl_object_t *max, unsigned int max_length)
{
    __clib_function_init__(false, NULL, -1, NULL);

    if ((validate_object(min, CL_OBJ_OBJECT) == false) ||
        (validate_object(max, CL_OBJ_OBJECT) == false))
    {
        return NULL;
    }

    return new_spec_s(properties, min, max, max_length);
}

__PUB_API__ int cl_spec_destroy(cl_spec_t *spec)
{
    __clib_function_init__(true, spec, CL_OBJ_SPEC, -1);
    destroy_spec_s(spec);

    return 0;
}

static bool validate_accessibility(const cl_spec_s *spec, bool set)
{
    /* May write? */
    if ((set == true) && (!(spec->properties & CL_WRITABLE)))
        return false;

    /* May read? */
    if ((set == false) && (!(spec->properties & CL_READABLE)))
        return false;

    return true;
}

static bool validate_char(const cl_spec_s *spec, char value,
    enum cl_spec_validation_fmt validation)
{
    char min, max;

    cl_object_get(spec->min, CL_OBJECT_CHAR, &min);
    cl_object_get(spec->max, CL_OBJECT_CHAR, &max);

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

static bool validate_uchar(const cl_spec_s *spec, unsigned char value,
    enum cl_spec_validation_fmt validation)
{
    unsigned char min, max;

    cl_object_get(spec->min, CL_OBJECT_UCHAR, &min);
    cl_object_get(spec->max, CL_OBJECT_UCHAR, &max);

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

static bool validate_int(const cl_spec_s *spec, int value,
    enum cl_spec_validation_fmt validation)
{
    int min, max;

    cl_object_get(spec->min, CL_OBJECT_INT, &min);
    cl_object_get(spec->max, CL_OBJECT_INT, &max);

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

static bool validate_uint(const cl_spec_s *spec, unsigned int value,
    enum cl_spec_validation_fmt validation)
{
    unsigned int min, max;

    cl_object_get(spec->min, CL_OBJECT_UINT, &min);
    cl_object_get(spec->max, CL_OBJECT_UINT, &max);

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

static bool validate_sint(const cl_spec_s *spec, short int value,
    enum cl_spec_validation_fmt validation)
{
    short int min, max;

    cl_object_get(spec->min, CL_OBJECT_SINT, &min);
    cl_object_get(spec->max, CL_OBJECT_SINT, &max);

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

static bool validate_usint(const cl_spec_s *spec,
     unsigned short int value, enum cl_spec_validation_fmt validation)
{
    unsigned short int min, max;

    cl_object_get(spec->min, CL_OBJECT_USINT, &min);
    cl_object_get(spec->max, CL_OBJECT_USINT, &max);

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

static bool validate_long(const cl_spec_s *spec, long value,
    enum cl_spec_validation_fmt validation)
{
    long min, max;

    cl_object_get(spec->min, CL_OBJECT_LONG, &min);
    cl_object_get(spec->max, CL_OBJECT_LONG, &max);

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

static bool validate_ulong(const cl_spec_s *spec, unsigned long value,
    enum cl_spec_validation_fmt validation)
{
    unsigned long min, max;

    cl_object_get(spec->min, CL_OBJECT_ULONG, &min);
    cl_object_get(spec->max, CL_OBJECT_ULONG, &max);

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

static bool validate_llong(const cl_spec_s *spec, long long value,
    enum cl_spec_validation_fmt validation)
{
    long long min, max;

    cl_object_get(spec->min, CL_OBJECT_LLONG, &min);
    cl_object_get(spec->max, CL_OBJECT_LLONG, &max);

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

static bool validate_ullong(const cl_spec_s *spec,
    unsigned long long value, enum cl_spec_validation_fmt validation)
{
    unsigned long long min, max;

    cl_object_get(spec->min, CL_OBJECT_ULLONG, &min);
    cl_object_get(spec->max, CL_OBJECT_ULLONG, &max);

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

static bool validate_float(const cl_spec_s *spec, float value,
    enum cl_spec_validation_fmt validation)
{
    float min, max;

    cl_object_get(spec->min, CL_OBJECT_FLOAT, &min);
    cl_object_get(spec->max, CL_OBJECT_FLOAT, &max);

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

static bool validate_double(const cl_spec_s *spec, double value,
    enum cl_spec_validation_fmt validation)
{
    double min, max;

    cl_object_get(spec->min, CL_OBJECT_DOUBLE, &min);
    cl_object_get(spec->max, CL_OBJECT_DOUBLE, &max);

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

static bool validate_string(const cl_spec_s *spec, const char *s)
{
    if ((unsigned int)strlen(s) > spec->max_length)
        return false;

    return true;
}

static bool validate_cstring(const cl_spec_s *spec, cl_string_t *s)
{
    if ((unsigned int)cl_string_length(s) > spec->max_length)
        return false;

    return true;
}

static bool validate_value(const cl_spec_s *spec, cl_object_t *value,
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

    switch (cl_object_type(value)) {
        case CL_VOID:
        case CL_POINTER:
            /* noop */
            break;

        case CL_CHAR:
            c = (char)va_arg(ap, int);

            if (validate_char(spec, c, validation) == true) {
                cl_object_set_char(value, c);
                ret = true;
            }

            break;

        case CL_UCHAR:
            uc = (unsigned char)va_arg(ap, int);

            if (validate_uchar(spec, uc, validation) == true) {
                cl_object_set_uchar(value, uc);
                ret = true;
            }

            break;

        case CL_INT:
            i = va_arg(ap, int);

            if (validate_int(spec, i, validation) == true) {
                cl_object_set_int(value, i);
                ret = true;
            }

            break;

        case CL_UINT:
            ui = va_arg(ap, unsigned int);

            if (validate_uint(spec, ui, validation) == true) {
                cl_object_set_uint(value, ui);
                ret = true;
            }

            break;

        case CL_SINT:
            si = (short int)va_arg(ap, int);

            if (validate_sint(spec, si, validation) == true) {
                cl_object_set_int(value, si);
                ret = true;
            }

            break;

        case CL_USINT:
            usi = (unsigned short int)va_arg(ap, unsigned int);

            if (validate_usint(spec, usi, validation) == true) {
                cl_object_set_uint(value, usi);
                ret = true;
            }

            break;

        case CL_FLOAT:
            f = (float)va_arg(ap, double);

            if (validate_float(spec, f, validation) == true) {
                cl_object_set_float(value, f);
                ret = true;
            }

            break;

        case CL_DOUBLE:
            d = va_arg(ap, double);

            if (validate_double(spec, d, validation) == true) {
                cl_object_set_double(value, d);
                ret = true;
            }

            break;

        case CL_LONG:
            l = va_arg(ap, long);

            if (validate_long(spec, l, validation) == true) {
                cl_object_set_long(value, l);
                ret = true;
            }

            break;

        case CL_ULONG:
            ul = va_arg(ap, unsigned long);

            if (validate_ulong(spec, ul, validation) == true) {
                cl_object_set_ulong(value, ul);
                ret = true;
            }

            break;

        case CL_LLONG:
            ll = va_arg(ap, long long);

            if (validate_llong(spec, ll, validation) == true) {
                cl_object_set_llong(value, ll);
                ret = true;
            }

            break;

        case CL_ULLONG:
            ull = va_arg(ap, unsigned long long);

            if (validate_ullong(spec, ull, validation) == true) {
                cl_object_set_ullong(value, ull);
                ret = true;
            }

            break;

        case CL_CSTRING:
            p = va_arg(ap, void *);

            if (validate_cstring(spec, p) == true) {
                cl_object_set_cstring(value, p);
                ret = true;
            }

            break;

        case CL_STRING:
            cp = va_arg(ap, char *);

            if (validate_string(spec, cp) == true) {
                cl_object_set_string(value, cp);
                ret = true;
            }

            break;

        case CL_BOOLEAN:
            b = va_arg(ap, int);

            if (validate_boolean(b) == true) {
                cl_object_set_boolean(value, b);
                ret = true;
            }

            break;
    }

    return ret;
}

__PUB_API__ bool cl_spec_validate(const cl_spec_t *spec, cl_object_t *value,
    bool set_value, enum cl_spec_validation_fmt validation, va_list ap)
{
    cl_object_t *ref;
    bool ret;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(spec, CL_OBJ_SPEC) == false) ||
        (validate_object(value, CL_OBJ_OBJECT) == false))
    {
        return false;
    }

    ref = cl_object_ref(value);
    ret = validate_accessibility(spec, set_value);

    if (ret == false) {
        cl_object_unref(ref);
        return false;
    } else {
        /* Doesn't need to parse while checking for trying to read value */
        if (set_value == false) {
            cl_object_unref(ref);
            return ret;
        }
    }

    ret = validate_value(spec, ref, validation, ap);
    cl_object_unref(ref);

    return ret;
}

__PUB_API__ int cl_spec_set_min(cl_spec_t *spec, cl_object_t *min)
{
    cl_spec_s *s = (cl_spec_s *)spec;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(spec, CL_OBJ_SPEC) == false) ||
        (validate_object(min, CL_OBJ_OBJECT) == false))
    {
        return -1;
    }

    cl_object_unref(s->min);
    s->min = cl_object_ref(min);

    return 0;
}

__PUB_API__ int cl_spec_set_max(cl_spec_t *spec, cl_object_t *max)
{
    cl_spec_s *s = (cl_spec_s *)spec;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(spec, CL_OBJ_SPEC) == false) ||
        (validate_object(max, CL_OBJ_OBJECT) == false))
    {
        return -1;
    }

    cl_object_unref(s->max);
    s->max = cl_object_ref(max);

    return 0;
}

__PUB_API__ int cl_spec_set_max_length(cl_spec_t *spec, unsigned int max_length)
{
    cl_spec_s *s = (cl_spec_s *)spec;

    __clib_function_init__(true, spec, CL_OBJ_SPEC, -1);
    s->max_length = max_length;

    return 0;
}

__PUB_API__ int cl_spec_set_accessibility(cl_spec_t *spec,
    enum cl_spec_attrib attrib)
{
    cl_spec_s *s = (cl_spec_s *)spec;

    __clib_function_init__(true, spec, CL_OBJ_SPEC, -1);
    s->properties = attrib;

    return 0;
}

