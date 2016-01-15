
/*
 * Description: API to handle a generic counter.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Jan 11 16:24:26 2016
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
#include <limits.h>

#include "collections.h"

struct counter_s {
    enum counter_precision   precision;
    bool             circular_counter;
    bool             negative_min;
    cvalue_t            *cnt;
    cvalue_t            *min;
    cvalue_t            *max;
    long long           start_value;

    /* reference count */
    struct ref_s        ref;
};

static void adjust_8bit_counter(struct counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cvalue_new(CL_LLONG, UCHAR_MAX, NULL);
    else
        c->max = cvalue_new(CL_LLONG, max, NULL);
}

static void adjust_16bit_counter(struct counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cvalue_new(CL_LLONG, USHRT_MAX, NULL);
    else
        c->max = cvalue_new(CL_LLONG, max, NULL);
}

static void adjust_32bit_counter(struct counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cvalue_new(CL_LLONG, UINT_MAX, NULL);
    else
        c->max = cvalue_new(CL_LLONG, max, NULL);
}

static void adjust_64bit_counter(struct counter_s *c, long long max)
{
    if (max <= 0)
        c->max = cvalue_new(CL_LLONG, ULLONG_MAX, NULL);
    else
        c->max = cvalue_new(CL_LLONG, max, NULL);
}

static void adjust_counter_limits(struct counter_s *c, long long min,
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
        c->min = cvalue_new(CL_LLONG, 0, NULL);
    else {
        if (min < 0)
            c->negative_min = true;

        c->min = cvalue_new(CL_LLONG, min, NULL);
    }
}

static void destroy_counter_s(const struct ref_s *ref)
{
    struct counter_s *c = container_of(ref, struct counter_s, ref);

    if (NULL == c)
        return;

    if (c->cnt != NULL)
        cvalue_destroy(c->cnt);

    if (c->min != NULL)
        cvalue_destroy(c->min);

    if (c->max != NULL)
        cvalue_destroy(c->max);
}

static struct counter_s *new_counter_s(enum counter_precision precision, long long min,
    long long max, long long start_value, bool circular)
{
    struct counter_s *c = NULL;

    c = calloc(1, sizeof(struct counter_s));

    if (NULL == c) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    c->precision = precision;
    c->circular_counter = circular;
    c->negative_min = false;
    c->cnt = cvalue_new(CL_LLONG, start_value, NULL);
    c->start_value = start_value;
    c->ref.free = destroy_counter_s;
    c->ref.count = 1;

    /* Adjust counter limits */
    adjust_counter_limits(c, min, max);

    return c;
}

counter_t LIBEXPORT *counter_ref(counter_t *c)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ref_inc(&p->ref);

    return c;
}

int LIBEXPORT counter_unref(counter_t *c)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ref_dec(&p->ref);

    return 0;
}

counter_t LIBEXPORT *counter_create(enum counter_precision precision,
    long long min, long long max, long long start_value, bool circular)
{
    return new_counter_s(precision, min, max, start_value, circular);
}

int LIBEXPORT counter_destroy(counter_t *c)
{
    return counter_unref(c);
}

static int __counter_increase(counter_t *c, long long gap)
{
    struct counter_s *p = (struct counter_s *)c;
    long long v;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    v = CVALUE_LLONG(p->cnt) + gap;

    if (v > CVALUE_LLONG(p->max)) {
        if (p->circular_counter == false)
            v = CVALUE_LLONG(p->max);
        else
            v = CVALUE_LLONG(p->min);
    }

    cvalue_set(p->cnt, v, NULL);

    return 0;
}

int LIBEXPORT counter_increase(counter_t *c)
{
    return __counter_increase(c, 1);
}

int LIBEXPORT counter_increase_by(counter_t *c, long long gap)
{
    return __counter_increase(c, gap);
}

static int __counter_decrease(counter_t *c, long long gap)
{
    struct counter_s *p = (struct counter_s *)c;
    long long v;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    v = CVALUE_LLONG(p->cnt) - gap;

    if (v < CVALUE_LLONG(p->min)) {
        if (p->circular_counter == false)
            v = CVALUE_LLONG(p->min);
        else
            v = CVALUE_LLONG(p->max);
    }

    cvalue_set(p->cnt, v, NULL);

    return 0;
}

int LIBEXPORT counter_decrease(counter_t *c)
{
    return __counter_decrease(c, 1);
}

int LIBEXPORT counter_decrease_by(counter_t *c, long long gap)
{
    return __counter_decrease(c, gap);
}

int LIBEXPORT counter_reset(counter_t *c)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    cvalue_set(p->cnt, p->start_value, NULL);

    return 0;
}

long long LIBEXPORT counter_get(counter_t *c)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return CVALUE_LLONG(p->cnt);
}

int LIBEXPORT counter_set_min(counter_t *c, long long min)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (NULL == p->min)
        p->min = cvalue_new(CL_LLONG, min, NULL);
    else
        cvalue_set(p->min, min, NULL);

    return 0;
}

int LIBEXPORT counter_set_max(counter_t *c, long long max)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (NULL == p->max)
        p->max = cvalue_new(CL_LLONG, max, NULL);
    else
        cvalue_set(p->max, max, NULL);

    return 0;
}

int LIBEXPORT counter_set_range(counter_t *c, long long min, long long max)
{
    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    counter_set_min(c, min);
    counter_set_max(c, max);

    return 0;
}

bool LIBEXPORT counter_lt(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (p->negative_min == true) {
        if (CVALUE_LLONG(p->cnt) < value)
            return true;
    } else
        if ((unsigned long long)CVALUE_LLONG(p->cnt) < (unsigned long long)value)
            return true;

    return false;
}

bool LIBEXPORT counter_le(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (p->negative_min == true) {
        if (CVALUE_LLONG(p->cnt) <= value)
            return true;
    } else
        if ((unsigned long long)CVALUE_LLONG(p->cnt) <= (unsigned long long)value)
            return true;

    return false;
}

bool LIBEXPORT counter_gt(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (p->negative_min == true) {
        if (CVALUE_LLONG(p->cnt) > value)
            return true;
    } else
        if ((unsigned long long)CVALUE_LLONG(p->cnt) > (unsigned long long)value)
            return true;

    return false;
}

bool LIBEXPORT counter_ge(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (p->negative_min == true) {
        if (CVALUE_LLONG(p->cnt) >= value)
            return true;
    } else
        if ((unsigned long long)CVALUE_LLONG(p->cnt) >= (unsigned long long)value)
            return true;

    return false;
}

bool LIBEXPORT counter_eq(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (CVALUE_LLONG(p->cnt) == value)
        return true;

    return false;
}

bool LIBEXPORT counter_ne(const counter_t *c, long long value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (CVALUE_LLONG(p->cnt) != value)
        return true;

    return false;
}

static bool is_between_limits(long long value, const struct counter_s *c)
{
    if ((value >= CVALUE_LLONG(c->min)) && (value <= CVALUE_LLONG(c->max)))
        return true;

    return false;
}

long long LIBEXPORT counter_get_and_set(counter_t *c, long long new_value)
{
    struct counter_s *p = (struct counter_s *)c;
    long long v = -1;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (is_between_limits(new_value, p) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    v = CVALUE_LLONG(p->cnt);
    cvalue_set(p->cnt, new_value, NULL);

    return v;
}

int LIBEXPORT counter_set(counter_t *c, long long new_value)
{
    struct counter_s *p = (struct counter_s *)c;

    cerrno_clear();

    if (NULL == c) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (is_between_limits(new_value, p) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    cvalue_set(p->cnt, new_value, NULL);

    return 0;
}

