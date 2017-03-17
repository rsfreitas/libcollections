
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

#define counter_members                                     \
    cl_struct_member(enum cl_counter_precision, precision)  \
    cl_struct_member(bool, circular_counter)                \
    cl_struct_member(bool, negative_min)                    \
    cl_struct_member(cobject_t *, cnt)                      \
    cl_struct_member(cobject_t *, min)                      \
    cl_struct_member(cobject_t *, max)                      \
    cl_struct_member(long long, start_value)                \
    cl_struct_member(pthread_mutex_t, lock)                 \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(counter_s, counter_members);

#define counter_s           cl_struct(counter_s)

static void adjust_8bit_counter(counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cobject_create(CL_LLONG, UCHAR_MAX, NULL);
    else
        c->max = cobject_create(CL_LLONG, max, NULL);
}

static void adjust_16bit_counter(counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cobject_create(CL_LLONG, USHRT_MAX, NULL);
    else
        c->max = cobject_create(CL_LLONG, max, NULL);
}

static void adjust_32bit_counter(counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cobject_create(CL_LLONG, UINT_MAX, NULL);
    else
        c->max = cobject_create(CL_LLONG, max, NULL);
}

static void adjust_64bit_counter(counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cobject_create(CL_LLONG, ULLONG_MAX, NULL);
    else
        c->max = cobject_create(CL_LLONG, max, NULL);
}

static void adjust_counter_limits(counter_s *c, long long min,
    long long max)
{
    switch (c->precision) {
        case CNT_8BIT:
            adjust_8bit_counter(c, max);
            break;

        case CNT_16BIT:
            adjust_16bit_counter(c, max);
            break;

        case CNT_32BIT:
            adjust_32bit_counter(c, max);
            break;

        case CNT_64BIT:
            adjust_64bit_counter(c, max);
            break;
    }

    if (min == 0)
        c->min = cobject_create(CL_LLONG, 0, NULL);
    else {
        if (min < 0)
            c->negative_min = true;

        c->min = cobject_create(CL_LLONG, min, NULL);
    }
}

static void destroy_counter_s(const struct cref_s *ref)
{
    counter_s *c = cl_container_of(ref, counter_s, ref);

    if (NULL == c)
        return;

    if (c->cnt != NULL)
        cobject_destroy(c->cnt);

    if (c->min != NULL)
        cobject_destroy(c->min);

    if (c->max != NULL)
        cobject_destroy(c->max);

    pthread_mutex_destroy(&c->lock);
    free(c);
    c = NULL;
}

static counter_s *new_counter_s(enum cl_counter_precision precision,
    long long min, long long max, long long start_value, bool circular)
{
    counter_s *c = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    c = calloc(1, sizeof(counter_s));

    if (NULL == c) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    c->precision = precision;
    c->circular_counter = circular;
    c->negative_min = false;
    c->cnt = cobject_create(CL_LLONG, start_value, NULL);
    c->start_value = start_value;
    c->ref.free = destroy_counter_s;
    c->ref.count = 1;

    set_typeof(COUNTER, c);
    pthread_mutex_init(&c->lock, NULL);

    /* Adjust counter limits */
    adjust_counter_limits(c, min, max);

    return c;
}

__PUB_API__ counter_t *counter_ref(counter_t *c)
{
    counter_s *p = (counter_s *)c;

    __clib_function_init__(true, c, COUNTER, NULL);
    cref_inc(&p->ref);

    return c;
}

__PUB_API__ int counter_unref(counter_t *c)
{
    counter_s *p = (counter_s *)c;

    __clib_function_init__(true, c, COUNTER, -1);
    cref_dec(&p->ref);

    return 0;
}

__PUB_API__ counter_t *counter_create(enum cl_counter_precision precision,
    long long min, long long max, long long start_value, bool circular)
{
    return new_counter_s(precision, min, max, start_value, circular);
}

__PUB_API__ int counter_destroy(counter_t *c)
{
    return counter_unref(c);
}

static int __counter_increase(counter_t *c, long long gap)
{
    counter_s *p;
    long long v, max;

    __clib_function_init__(true, c, COUNTER, -1);
    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);
    v = COBJECT_AS_LLONG(p->cnt) + gap;
    max = COBJECT_AS_LLONG(p->max);

    if (v > max) {
        if (p->circular_counter == false)
            v = max;
        else
            v = COBJECT_AS_LLONG(p->min);
    }

    cobject_set(p->cnt, v, NULL);
    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

__PUB_API__ int counter_increase(counter_t *c)
{
    return __counter_increase(c, 1);
}

__PUB_API__ int counter_increase_by(counter_t *c, long long gap)
{
    return __counter_increase(c, gap);
}

static int __counter_decrease(counter_t *c, long long gap)
{
    counter_s *p;
    long long v, min;

    __clib_function_init__(true, c, COUNTER, -1);
    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);
    v = COBJECT_AS_LLONG(p->cnt) - gap;
    min = COBJECT_AS_LLONG(p->min);

    if (v < min) {
        if (p->circular_counter == false)
            v = min;
        else
            v = COBJECT_AS_LLONG(p->max);
    }

    cobject_set(p->cnt, v, NULL);
    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

__PUB_API__ int counter_decrease(counter_t *c)
{
    return __counter_decrease(c, 1);
}

__PUB_API__ int counter_decrease_by(counter_t *c, long long gap)
{
    return __counter_decrease(c, gap);
}

__PUB_API__ int counter_reset(counter_t *c)
{
    counter_s *p;

    __clib_function_init__(true, c, COUNTER, -1);
    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);

    cobject_set(p->cnt, p->start_value, NULL);

    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

__PUB_API__ long long counter_get(counter_t *c)
{
    counter_s *p = (counter_s *)c;

    __clib_function_init__(true, c, COUNTER, -1);

    return COBJECT_AS_LLONG(p->cnt);
}

__PUB_API__ int counter_set_min(counter_t *c, long long min)
{
    counter_s *p;

    __clib_function_init__(true, c, COUNTER, -1);
    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);

    if (NULL == p->min)
        p->min = cobject_create(CL_LLONG, min, NULL);
    else
        cobject_set(p->min, min, NULL);

    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

__PUB_API__ int counter_set_max(counter_t *c, long long max)
{
    counter_s *p;

    __clib_function_init__(true, c, COUNTER, -1);
    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);

    if (NULL == p->max)
        p->max = cobject_create(CL_LLONG, max, NULL);
    else
        cobject_set(p->max, max, NULL);

    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

__PUB_API__ int counter_set_range(counter_t *c, long long min, long long max)
{
    __clib_function_init__(true, c, COUNTER, -1);

    counter_set_min(c, min);
    counter_set_max(c, max);

    return 0;
}

__PUB_API__ bool counter_lt(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;
    long long v;

    __clib_function_init__(true, c, COUNTER, false);
    v = COBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v < value)
            return true;
    } else
        if ((unsigned long long)v < (unsigned long long)value)
            return true;

    return false;
}

__PUB_API__ bool counter_le(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;
    long long v;

    __clib_function_init__(true, c, COUNTER, false);
    v = COBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v <= value)
            return true;
    } else
        if ((unsigned long long)v <= (unsigned long long)value)
            return true;

    return false;
}

__PUB_API__ bool counter_gt(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;
    long long v;

    __clib_function_init__(true, c, COUNTER, false);
    v = COBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v > value)
            return true;
    } else
        if ((unsigned long long)v > (unsigned long long)value)
            return true;

    return false;
}

__PUB_API__ bool counter_ge(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;
    long long v;

    __clib_function_init__(true, c, COUNTER, false);
    v = COBJECT_AS_LLONG(p->cnt);

    if (p->negative_min == true) {
        if (v >= value)
            return true;
    } else
        if ((unsigned long long)v >= (unsigned long long)value)
            return true;

    return false;
}

__PUB_API__ bool counter_eq(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;

    __clib_function_init__(true, c, COUNTER, false);

    if (COBJECT_AS_LLONG(p->cnt) == value)
        return true;

    return false;
}

__PUB_API__ bool counter_ne(const counter_t *c, long long value)
{
    counter_s *p = (counter_s *)c;

    __clib_function_init__(true, c, COUNTER, false);

    if (COBJECT_AS_LLONG(p->cnt) != value)
        return true;

    return false;
}

static bool is_between_limits(long long value, const counter_s *c)
{
    if ((value >= COBJECT_AS_LLONG(c->min)) &&
        (value <= COBJECT_AS_LLONG(c->max)))
    {
        return true;
    }

    return false;
}

__PUB_API__ long long counter_get_and_set(counter_t *c, long long new_value)
{
    counter_s *p;
    long long v = -1;

    __clib_function_init__(true, c, COUNTER, -1);

    if (is_between_limits(new_value, (counter_s *)c) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);

    v = COBJECT_AS_LLONG(p->cnt);
    cobject_set(p->cnt, new_value, NULL);

    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return v;
}

__PUB_API__ int counter_set(counter_t *c, long long new_value)
{
    counter_s *p;

    __clib_function_init__(true, c, COUNTER, -1);

    if (is_between_limits(new_value, (counter_s *)c) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    p = counter_ref(c);
    pthread_mutex_lock(&p->lock);

    cobject_set(p->cnt, new_value, NULL);

    pthread_mutex_unlock(&p->lock);
    counter_unref(p);

    return 0;
}

