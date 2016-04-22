
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
#include <string.h>
#include <stdarg.h>

#include <pthread.h>

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
    pthread_mutex_t     lock;
    enum clog_mode      mode;
    enum clog_level     level;
    unsigned int        max_repeat;
    cstring_t           *header;
    char                separator;
    struct last_msg     lmsg;
    struct clog_fmt_s   *lfmt;
    enum clog_fmt_field final_fmt;
    struct ref_s        ref;
};

/*
 * Releases a 'struct clog_s' from memory. Function to be called when its
 * reference count drops to 0.
 */
static void __destroy_clog_s(const struct ref_s *ref)
{
    struct clog_s *l = container_of(ref, struct clog_s, ref);

    if (NULL == l)
        return;

    if (l->lfmt != NULL)
        clog_fmt_unref(l->lfmt);

    if (l->header != NULL)
        cstring_unref(l->header);

    if (l->pathname != NULL)
        free(l->pathname);

    if (l->f != NULL)
        fclose(l->f);

    free(l);
}

/*
 * Creates a new 'struct clog_s'.
 */
static struct clog_s *new_clog_s(void)
{
    struct clog_s *l = NULL;

    l = calloc(1, sizeof(struct clog_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->ref.count = 1;
    l->ref.free = __destroy_clog_s;
    pthread_mutex_init(&l->lock, NULL);

    return l;
}

/*
 * Creates a new 'struct log_fmt'.
 */
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

/*
 * Releases a 'struct log_fmt' from memory. Function to be used with a
 * cdll_free call.
 */
static void destroy_log_fmt(void *p)
{
    struct log_fmt *l = (struct log_fmt *)p;

    if (NULL == l)
        return;

    free(l);
}

/*
 * Releases a 'struct clog_fmt_s' from memory. Function to be called when its
 * reference count drops to 0.
 */
static void __destroy_clog_fmt_s(const struct ref_s *ref)
{
    struct clog_fmt_s *l = container_of(ref, struct clog_fmt_s, ref);

    if (NULL == l)
        return;

    if (l->fmts != NULL)
        cdll_free(l->fmts, destroy_log_fmt);

    free(l);
}

/*
 * Creates a new 'struct clog_fmt_s'.
 */
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
 * Sets the log mode.
 *
 * - CLOG_SYNC_ALL_MSGS: every message will cause the log file to be opened,
 *                       the message will be written and then closed.
 *
 * - CLOG_KEEP_FILE_OPEN: the file will be opened as log as the @log is active
 *                        inside the application.
 */
static int set_log_mode(struct clog_s *log, enum clog_mode mode,
    const char *pathname)
{
    log->f = NULL;
    log->pathname = NULL;

    if (mode == CLOG_SYNC_ALL_MSGS)
        log->pathname = strdup(pathname);
    else if (mode == CLOG_KEEP_FILE_OPEN) {
        log->f = fopen(pathname, "r+");

        if (NULL == log->f) {
            cset_errno(CL_FILE_OPEN_ERROR);
            return -1;
        }
    }

    return 0;
}

/*
 * Get all log format fields within.
 */
static enum clog_fmt_field get_log_fields(clog_fmt_t *fmts)
{
    struct clog_fmt_s *lfmt = (struct clog_fmt_s *)fmts;
    struct log_fmt *p;
    enum clog_fmt_field fields = 0;

    for (p = lfmt->fmts; p; p = p->next)
        fields |= p->field;

    return fields;
}

static cstring_t *set_default_log_header(void)
{
    cstring_t *s = cstring_create("%s", CLOG_HEADER);

    return s;
}

static int write_message(struct clog_s *log, enum clog_level level,
    const char *msg)
{
    return 0;
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

int clog_fmt_add(clog_fmt_t *fmt, enum clog_fmt_field field)
{
    struct clog_fmt_s *l = (struct clog_fmt_s *)fmt;
    struct log_fmt *n;

    cerrno_clear();

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    n = new_log_fmt(field);

    if (NULL == n)
        return -1;

    cdll_unshift(l->fmts, n);

    return 0;
}

/*
 *
 * Log messages API
 *
 */

clog_t *clog_open_ex(const char *pathname, enum clog_mode mode,
    enum clog_level start_level, unsigned int max_repeat, char separator,
    clog_fmt_t *fmt)
{
    struct clog_s *log = NULL;

    cerrno_clear();

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    /* TODO: validate mode, start_level, max_repeat, separator and fmt */

    log = new_clog_s();

    if (NULL == log)
        return NULL;

    if (set_log_mode(log, mode, pathname) < 0)
        goto error_block;

    log->level = start_level;
    log->max_repeat = max_repeat;
    log->separator = separator;
    log->lfmt = fmt;
    log->final_fmt = get_log_fields(fmt);
    log->header = set_default_log_header();

    return log;

error_block:
    ref_dec(&log->ref);
    return NULL;
}

clog_t *clog_open(const char *pathname, enum clog_mode mode,
    enum clog_level start_level, unsigned int max_repeat)
{
    clog_fmt_t *fmt;
    clog_t *log;

    cerrno_clear();
    fmt = clog_fmt_create();

    if (NULL == fmt)
        return NULL;

    /* Creates the default log format: DATE; TIME; PID; LEVEL; msg */
    clog_fmt_add(fmt, CLOG_FIELD_DATE);
    clog_fmt_add(fmt, CLOG_FIELD_TIME);
    clog_fmt_add(fmt, CLOG_FIELD_PID);
    clog_fmt_add(fmt, CLOG_FIELD_LEVEL);

    log = clog_open_ex(pathname, mode, start_level, max_repeat,
                       CLOG_SEPARATOR, fmt);

    clog_fmt_unref(fmt);

    return log;
}

int clog_close(clog_t *log)
{
    struct clog_s *l = (struct clog_s *)log;

    cerrno_clear();

    if (NULL == l) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ref_dec(&l->ref);

    return 0;
}

int clog_vprintf(clog_t *log, enum clog_level level, const char *fmt,
    va_list args)
{
    char *msg = NULL;
    int ret = -1;

    cerrno_clear();

    if (NULL == log) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    vasprintf(&msg, fmt, args);
    ret = write_message(log, level, msg);
    free(msg);

    return ret;
}

int clog_printf(clog_t *log, enum clog_level level, const char *fmt, ...)
{
    va_list ap;
    int ret = -1;

    cerrno_clear();

    if (NULL == log) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    ret = clog_vprintf(log, level, fmt, ap);
    va_end(ap);

    return ret;
}

void clog_bprint(void)
{
}

void clog_rprint(void)
{
}

void clog_set_log_level(void)
{
}

void clog_set_header(void)
{
}

void clog_set_separator(void)
{
}

