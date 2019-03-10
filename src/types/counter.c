
/*
 * Description: API to handle a generic counter.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Jan 11 16:24:26 2016
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
#include <limits.h>

#include <pthread.h>

#include "collections.h"

#define cl_counter_members                                  \
    cl_struct_member(enum cl_counter_precision, precision)  \
    cl_struct_member(bool, circular_counter)                \
    cl_struct_member(bool, negative_min)                    \
    cl_struct_member(cl_object_t *, cnt)                    \
    cl_struct_member(cl_object_t *, min)                    \
    cl_struct_member(cl_object_t *, max)                    \
    cl_struct_member(long long, start_value)                \
    cl_struct_member(pthread_mutex_t, lock)                 \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_counter_s, cl_counter_members);
#define cl_counter_s           cl_struct(cl_counter_s)

/*
 *
 * Internal functions
 *
 */

static void adjust_8bit_counter(cl_counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cl_object_create(CL_LLONG, UCHAR_MAX, NULL);
    else
        c->max = cl_object_create(CL_LLONG, max, NULL);
}

static void adjust_16bit_counter(cl_counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cl_object_create(CL_LLONG, USHRT_MAX, NULL);
    else
        c->max = cl_object_create(CL_LLONG, max, NULL);
}

static void adjust_32bit_counter(cl_counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cl_object_create(CL_LLONG, UINT_MAX, NULL);
    else
        c->max = cl_object_create(CL_LLONG, max, NULL);
}

static void adjust_64bit_counter(cl_counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cl_object_create(CL_LLONG, ULLONG_MAX, NULL);
    else
        c->max = cl_object_create(CL_LLONG, max, NULL);
}

static void adjust_counter_limits(cl_counter_s *c, long long min,
    long long max)
{
    switch (c->precision) {
    case CL_8BIT_COUNTER:
        adjust_8bit_counter(c, max);
        break;

    case CL_16BIT_COUNTER:
        adjust_16bit_counter(c, max);
        break;

    case CL_32BIT_COUNTER:
        adjust_32bit_counter(c, max);
        break;

    case CL_64BIT_COUNTER:
        adjust_64bit_counter(c, max);
        break;
    }

    if (min == 0)
        c->min = cl_object_create(CL_LLONG, 0, NULL);
    else {
        if (min < 0)
            c->negative_min = true;

        c->min = cl_object_create(CL_LLONG, min, NULL);
    }
}

static void destroy_counter_s(const struct cl_ref_s *ref)
{
    cl_counter_s *c = cl_container_of(ref, cl_counter_s, ref);

    if (NULL == c)
        return;

    if (c->cnt != NULL)
        cl_object_destroy(c->cnt);

    if (c->min != NULL)
        cl_object_destroy(c->min);

    if (c->max != NULL)
        cl_object_destroy(c->max);

    pthread_mutex_destroy(&c->lock);
    free(c);
    c = NULL;
}

static cl_counter_s *new_counter_s(enum cl_counter_precision precision,
    long long min, long long max, long long start_value, bool circular)
{
    cl_counter_s *c = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    c = calloc(1, sizeof(cl_counter_s));

    if (NULL == c) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    c->precision = precision;
    c->circular_counter = circular;
    c->negative_min = false;
    c->cnt = cl_object_create(CL_LLONG, start_value, NULL);
    c->start_value = start_value;
    c->ref.free = destroy_counter_s;
    c->ref.count = 1;

    typeof_set(CL_OBJ_COUNTER, c);
    pthread_mutex_init(&c->lock, NULL);

    /* Adjust counter limits */
    adjust_counter_limits(c, min, max);

    return c;
}

static int __counter_increase(cl_counter_t *c, long long gap)
{
    cl_counter_s *p;
    long long v, max;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);
    v = CL_OBJECT_AS_LLONG(p->cnt) + gap;
    max = CL_OBJECT_AS_LLONG(p->max);

    if (v > max) {
        if (p->circular_counter == false)
            v = max;
        else
            v = CL_OBJECT_AS_LLONG(p->min);
    }

    cl_object_set(p->cnt, v, NULL);
    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

static int __counter_decrease(cl_counter_t *c, long long gap)
{
    cl_counter_s *p;
    long long v, min;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);
    v = CL_OBJECT_AS_LLONG(p->cnt) - gap;
    min = CL_OBJECT_AS_LLONG(p->min);

    if (v < min) {
        if (p->circular_counter == false)
            v = min;
        else
            v = CL_OBJECT_AS_LLONG(p->max);
    }

    cl_object_set(p->cnt, v, NULL);
    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

static bool is_between_limits(long long value, const cl_counter_s *c)
{
    if ((value >= CL_OBJECT_AS_LLONG(c->min)) &&
        (value <= CL_OBJECT_AS_LLONG(c->max)))
    {
        return true;
    }

    return false;
}

/*
 *
 * API
 *
 */

cl_counter_t *cl_counter_ref(cl_counter_t *c)
{
    cl_counter_s *p = (cl_counter_s *)c;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, NULL);
    cl_ref_inc(&p->ref);

    return c;
}

int cl_counter_unref(cl_counter_t *c)
{
    cl_counter_s *p = (cl_counter_s *)c;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    cl_ref_dec(&p->ref);

    return 0;
}

cl_counter_t *cl_counter_create(enum cl_counter_precision precision,
    long long min, long long max, long long start_value, bool circular)
{
    return new_counter_s(precision, min, max, start_value, circular);
}

int cl_counter_destroy(cl_counter_t *c)
{
    return cl_counter_unref(c);
}

int cl_counter_increase(cl_counter_t *c)
{
    return __counter_increase(c, 1);
}

int cl_counter_increase_by(cl_counter_t *c, long long gap)
{
    return __counter_increase(c, gap);
}

int cl_counter_decrease(cl_counter_t *c)
{
    return __counter_decrease(c, 1);
}

int cl_counter_decrease_by(cl_counter_t *c, long long gap)
{
    return __counter_decrease(c, gap);
}

int cl_counter_reset(cl_counter_t *c)
{
    cl_counter_s *p;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);

    cl_object_set(p->cnt, p->start_value, NULL);

    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

long long cl_counter_get(cl_counter_t *c)
{
    cl_counter_s *p = (cl_counter_s *)c;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);

    return CL_OBJECT_AS_LLONG(p->cnt);
}

int cl_counter_set_min(cl_counter_t *c, long long min)
{
    cl_counter_s *p;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);

    if (NULL == p->min)
        p->min = cl_object_create(CL_LLONG, min, NULL);
    else
        cl_object_set(p->min, min, NULL);

    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

int cl_counter_set_max(cl_counter_t *c, long long max)
{
    cl_counter_s *p;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);
    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);

    if (NULL == p->max)
        p->max = cl_object_create(CL_LLONG, max, NULL);
    else
        cl_object_set(p->max, max, NULL);

    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

int cl_counter_set_range(cl_counter_t *c, long long min,
    long long max)
{
    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);

    cl_counter_set_min(c, min);
    cl_counter_set_max(c, max);

    return 0;
}

bool cl_counter_lt(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;
    long long v;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);
    v = CL_OBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v < value)
            return true;
    } else
        if ((unsigned long long)v < (unsigned long long)value)
            return true;

    return false;
}

bool cl_counter_le(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;
    long long v;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);
    v = CL_OBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v <= value)
            return true;
    } else
        if ((unsigned long long)v <= (unsigned long long)value)
            return true;

    return false;
}

bool cl_counter_gt(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;
    long long v;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);
    v = CL_OBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v > value)
            return true;
    } else
        if ((unsigned long long)v > (unsigned long long)value)
            return true;

    return false;
}

bool cl_counter_ge(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;
    long long v;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);
    v = CL_OBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v >= value)
            return true;
    } else
        if ((unsigned long long)v >= (unsigned long long)value)
            return true;

    return false;
}

bool cl_counter_eq(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);

    if (CL_OBJECT_AS_LLONG(p->cnt) == value)
        return true;

    return false;
}

bool cl_counter_ne(const cl_counter_t *c, long long value)
{
    cl_counter_s *p = (cl_counter_s *)c;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, false);

    if (CL_OBJECT_AS_LLONG(p->cnt) != value)
        return true;

    return false;
}

long long cl_counter_get_and_set(cl_counter_t *c,
    long long new_value)
{
    cl_counter_s *p;
    long long v = -1;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);

    if (is_between_limits(new_value, (cl_counter_s *)c) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);

    v = CL_OBJECT_AS_LLONG(p->cnt);
    cl_object_set(p->cnt, new_value, NULL);

    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return v;
}

int cl_counter_set(cl_counter_t *c, long long new_value)
{
    cl_counter_s *p;

    __clib_function_init__(true, c, CL_OBJ_COUNTER, -1);

    if (is_between_limits(new_value, (cl_counter_s *)c) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    p = cl_counter_ref(c);
    pthread_mutex_lock(&p->lock);

    cl_object_set(p->cnt, new_value, NULL);

    pthread_mutex_unlock(&p->lock);
    cl_counter_unref(p);

    return 0;
}

