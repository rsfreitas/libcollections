
/*
 * Description: API to handle timeout object.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Apr 12 13:35:23 2016
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
#include <sys/time.h>

#include "collections.h"

#define cl_timeout_members                          \
    cl_struct_member(cl_datetime_t *, dt)           \
    cl_struct_member(enum cl_timeout, precision)    \
    cl_struct_member(unsigned int, interval)

cl_struct_declare(cl_timeout_s, cl_timeout_members);

#define cl_timeout_s          cl_struct(cl_timeout_s)

static cl_timeout_s *new_cl_timeout_s(unsigned int interval,
    enum cl_timeout precision)
{
    cl_datetime_t *dt = NULL;
    cl_timeout_s *t = NULL;

    dt = cl_dt_localtime();

    if (NULL == dt)
        return NULL;

    t = calloc(1, sizeof(cl_timeout_s));

    if (NULL == t) {
        cl_dt_destroy(dt);
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    t->dt = dt;
    t->precision = precision;
    t->interval = interval;
    typeof_set(CL_OBJ_TIMEOUT, t);

    return t;
}

static void destroy_cl_timeout_s(cl_timeout_s *t)
{
    if (NULL == t)
        return;

    if (t->dt != NULL)
        cl_dt_destroy(t->dt);

    free(t);
}

__PUB_API__ cl_timeout_t *cl_timeout_create(unsigned int interval,
   enum cl_timeout precision)
{
    cl_timeout_s *t;

    __clib_function_init__(false, NULL, -1, NULL);
    t = new_cl_timeout_s(interval, precision);

    if (NULL == t)
        return NULL;

    return t;
}

__PUB_API__ int cl_timeout_destroy(cl_timeout_t *t)
{
    cl_timeout_s *ct = (cl_timeout_s *)t;

    __clib_function_init__(true, t, CL_OBJ_TIMEOUT, -1);
    destroy_cl_timeout_s(ct);

    return 0;
}

__PUB_API__ int cl_timeout_reset(cl_timeout_t *t, unsigned int interval,
    enum cl_timeout precision)
{
    cl_timeout_s *ct = (cl_timeout_s *)t;

    __clib_function_init__(true, t, CL_OBJ_TIMEOUT, -1);

    if (ct->dt != NULL)
        cl_dt_destroy(ct->dt);

    ct->dt = cl_dt_localtime();

    if (NULL == ct->dt)
        return -1;

    ct->precision = precision;
    ct->interval = interval;

    return 0;
}

__PUB_API__ bool cl_timeout_expired(const cl_timeout_t *t)
{
    cl_timeout_s *ct = (cl_timeout_s *)t;
    struct timeval tv;
    unsigned int i;
    unsigned long long l;

    __clib_function_init__(true, t, CL_OBJ_TIMEOUT, -1);
    gettimeofday(&tv, NULL);

    switch (ct->precision) {
        case CL_TM_SECONDS:
            i = cl_dt_get_seconds(ct->dt) + ct->interval;

            if (tv.tv_sec > (time_t)i)
                return true;

            break;

        case CL_TM_MSECONDS:
            i = cl_dt_get_mseconds(ct->dt) + ct->interval;

            if ((unsigned int)((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) > i)
                return true;

            break;

        case CL_TM_USECONDS:
            l = cl_dt_get_useconds(ct->dt) + ct->interval;

            if ((unsigned long long)((tv.tv_sec * 1000000) + tv.tv_usec) > l)
                return true;

            break;
    }

    return false;
}

