
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Apr 21 12:29:16 2016
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

#include "collections.h"

struct last_msg {
    cstring_t       *msg;
    unsigned int    count;
};

struct log_fmt {
    struct log_fmt      *prev;
    struct log_fmt      *next;
    enum clog_fmt_field field;

    /* TODO: Add priority */
};

struct clog_fmt_s {
    struct ref_s        ref;
    struct log_fmt      *fmts;
};

struct clog_s {
    FILE                *f;
    char                *pathname;
    enum clog_mode      mode;
    enum clog_level     level;
    unsigned int        max_repeat;
    cstring_t           *header;
    struct last_msg     lmsg;
    struct clog_fmt_s   *lfmt;
    enum clog_fmt_field final_fmt;
};

static struct log_fmt *new_log_fmt(enum clog_fmt_field field)
{
    struct log_fmt *l = NULL;

    l = calloc(1, sizeof(struct log_fmt));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->field = field;

    return l;
}

static void destroy_log_fmt(void *p)
{
    struct log_fmt *l = (struct log_fmt *)p;

    if (NULL == l)
        return;

    free(l);
}

static void __destroy_clog_fmt_s(const struct ref_s *ref)
{
    struct clog_fmt_s *l = container_of(ref, struct clog_fmt_s, ref);

    if (NULL == l)
        return;

    if (l->fmts != NULL)
        cdll_free(l->fmts, destroy_log_fmt);

    free(l);
}

static struct clog_fmt_s *new_clog_fmt_s(void)
{
    struct clog_fmt_s *l = NULL;

    l = calloc(1, sizeof(struct clog_fmt_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->ref.count = 1;
    l->ref.free = __destroy_clog_fmt_s;
    l->fmts = NULL;

    return l;
}

/*
 *
 * Log messages format API
 *
 */

clog_fmt_t *clog_fmt_create(void)
{
    clog_fmt_t *fmt = NULL;

    fmt = new_clog_fmt_s();

    if (NULL == fmt)
        return NULL;

    return fmt;
}

int clog_fmt_unref(clog_fmt_t *fmt)
{
    struct clog_fmt_s *l = (struct clog_fmt_s *)fmt;

    cerrno_clear();

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ref_dec(&l->ref);

    return 0;
}

void clog_fmt_add(void)
{
}

/*
 *
 * Log messages API
 *
 */

clog_t *clog_init(const char *pathname, enum clog_mode mode,
    enum clog_level start_level, unsigned int max_repeat, clog_fmt_t *fmt)
{
}

void clog_uninit(void)
{
}

void clog_update_log_level(void)
{
}

void clog_vprintf(void)
{
}

void clog_printf(void)
{
}

void clog_bprint(void)
{
}

void clog_rprint(void)
{
}

