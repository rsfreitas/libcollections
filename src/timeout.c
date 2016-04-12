
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

struct ctimeout_s {
    struct cdatetime_s  *dt;
    enum ctimeout       precision;
    unsigned int        interval;
};

static struct ctimeout_s *new_ctimeout_s(unsigned int interval,
    enum ctimeout precision)
{
    cdatetime_t *dt = NULL;
    struct ctimeout_s *t = NULL;

    dt = cdt_localtime();

    if (NULL == dt)
        return NULL;

    t = calloc(1, sizeof(struct ctimeout_s));

    if (NULL == t) {
        cdt_destroy(dt);
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    t->dt = dt;
    t->precision = precision;
    t->interval = interval;

    return t;
}

static void destroy_ctimeout_s(struct ctimeout_s *t)
{
    if (NULL == t)
        return;

    if (t->dt != NULL)
        cdt_destroy(t->dt);

    free(t);
}

ctimeout_t LIBEXPORT *ctimeout_create(unsigned int interval,
   enum ctimeout precision)
{
    struct ctimeout_s *t;

    cerrno_clear();
    t = new_ctimeout_s(interval, precision);

    if (NULL == t)
        return NULL;

    return t;
}

int LIBEXPORT ctimeout_destroy(ctimeout_t *t)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_ctimeout_s(ct);

    return 0;
}

int LIBEXPORT ctimeout_reset(ctimeout_t *t, unsigned int interval,
    enum ctimeout precision)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (ct->dt != NULL)
        cdt_destroy(ct->dt);

    ct->dt = cdt_localtime();

    if (NULL == ct->dt)
        return -1;

    ct->precision = precision;
    ct->interval = interval;

    return 0;
}

bool LIBEXPORT ctimeout_expired(const ctimeout_t *t)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;
    struct timeval tv;
    unsigned int i;
    unsigned long long l;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    gettimeofday(&tv, NULL);

    switch (ct->precision) {
        case CL_TM_SECONDS:
            i = cdt_get_seconds(ct->dt) + ct->interval;

            if (tv.tv_sec > (time_t)i)
                return true;

            break;

        case CL_TM_MSECONDS:
            i = cdt_get_mseconds(ct->dt) + ct->interval;

            if ((unsigned int)((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) > i)
                return true;

            break;

        case CL_TM_USECONDS:
            l = cdt_get_useconds(ct->dt) + ct->interval;

            if ((unsigned long long)((tv.tv_sec * 1000000) + tv.tv_usec) > l)
                return true;

            break;
    }

    return false;
}

