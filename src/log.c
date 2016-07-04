
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
#include <ctype.h>
#include <unistd.h>

#include <pthread.h>

#include "collections.h"

#define CLOG_SEPARATOR              ';'
#define CLOG_COUNTER_MSG            "Last message repeated %d times\n"

/* Structure used to store the last message written in the log file */
struct last_msg {
    cstring_t       *msg;
    unsigned int    count;
};

#define clog_members                                    \
    cl_struct_member(FILE *, f)                         \
    cl_struct_member(char *, pathname)                  \
    cl_struct_member(enum clog_mode, mode)              \
    cl_struct_member(enum clog_level, level)            \
    cl_struct_member(enum clog_prefix_field, prefixes)  \
    cl_struct_member(unsigned int, max_repeat)          \
    cl_struct_member(char, separator)                   \
    cl_struct_member(struct last_msg, lmsg)             \
    cl_struct_member(pthread_mutex_t, lock)             \
    cl_struct_member(struct ref_s, ref)

cl_struct_declare(clog_s, clog_members);

#define clog_s          cl_struct(clog_s)

static void close_log_file(clog_s *log);

static bool is_mode_valid(enum clog_mode mode)
{
    switch (mode) {
        case CLOG_SYNC_ALL_MSGS:
        case CLOG_KEEP_FILE_OPEN:
            return true;

        default:
            break;
    }

    return false;
}

static bool is_level_valid(enum clog_level level)
{
    switch (level) {
        case CLOG_OFF:
        case CLOG_EMERG:
        case CLOG_ALERT:
        case CLOG_CRITI:
        case CLOG_ERROR:
        case CLOG_WARNG:
        case CLOG_NOTICE:
        case CLOG_INFO:
        case CLOG_DEBUG:
            return true;
    }

    return false;
}

static const char *level_to_string(enum clog_level level)
{
    switch (level) {
        case CLOG_OFF:
            return NULL;

        case CLOG_EMERG:
            return "EMERG";

        case CLOG_ALERT:
            return "ALERT";

        case CLOG_CRITI:
            return "CRITI";

        case CLOG_ERROR:
            return "ERROR";

        case CLOG_WARNG:
            return "WARNG";

        case CLOG_NOTICE:
            return "NOTIC";

        case CLOG_INFO:
            return "INFOM";

        case CLOG_DEBUG:
            return "DEBUG";
    }

    return NULL;
}

static bool is_separator_valid(char separator)
{
    if (ispunct(separator))
        return true;

    return false;
}

static bool compare_message(const char *m1, const char *m2)
{
    if ((NULL == m1) || (NULL == m2))
        return false;

    if (strcmp(m1, m2) == 0)
        return true;

    return false;
}

static bool may_write_message(clog_s *log, enum clog_level level)
{
    if (level <= log->level)
        return true;

    return false;
}

static void save_written_message(clog_s *log, const char *msg)
{
    if (compare_message(cstring_valueof(log->lmsg.msg), msg) == true)
        log->lmsg.count++;
    else {
        if (log->lmsg.msg != NULL)
            cstring_destroy(log->lmsg.msg);

        log->lmsg.msg = cstring_create("%s", msg);
        log->lmsg.count = 1;
    }
}

/*
 * Releases a 'clog_s' from memory. Function to be called when its
 * reference count drops to 0.
 */
static void __destroy_clog_s(const struct ref_s *ref)
{
    clog_s *l = container_of(ref, clog_s, ref);

    if (NULL == l)
        return;

    if (l->pathname != NULL)
        free(l->pathname);

    if (l->mode == CLOG_KEEP_FILE_OPEN)
        close_log_file(l);

    if (l->lmsg.msg != NULL)
        cstring_destroy(l->lmsg.msg);

    free(l);
}

/*
 * Creates a new 'clog_s'.
 */
static clog_s *new_clog_s(void)
{
    clog_s *l = NULL;

    l = calloc(1, sizeof(clog_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->ref.count = 1;
    l->ref.free = __destroy_clog_s;
    pthread_mutex_init(&l->lock, NULL);

    set_typeof(CLOG, l);

    return l;
}

static int open_log_file(clog_s *log)
{
    log->f = fopen(log->pathname, "a");

    if (NULL == log->f) {
        cset_errno(CL_FILE_OPEN_ERROR);
        return -1;
    }

    return 0;
}

static void close_log_file(clog_s *log)
{
    if (log->f != NULL)
        fclose(log->f);
}

static void sync_log_data(clog_s *log)
{
    if (log->f != NULL)
        fflush(log->f);
}

static void lock_log_file(clog_s *log)
{
    pthread_mutex_lock(&log->lock);
}

static void unlock_log_file(clog_s *log)
{
    pthread_mutex_unlock(&log->lock);
}

/*
 * Sets the way the log file will be handled.
 *
 * - CLOG_SYNC_ALL_MSGS: every message will cause the log file to be opened,
 *                       the message will be written and then the file closed.
 *
 * - CLOG_KEEP_FILE_OPEN: the file will be opened as long as the @log is active
 *                        inside the application.
 */
static int set_logfile_handle_mode(clog_s *log, enum clog_mode mode,
    const char *pathname)
{
    log->f = NULL;
    log->pathname = strdup(pathname);
    log->mode = mode;

    if (mode == CLOG_KEEP_FILE_OPEN)
        return open_log_file(log);

    return 0;
}

static cstring_t *message_prefix(clog_s *log, enum clog_level level)
{
    cstring_t *p = NULL, *tmp = NULL;
    cdatetime_t *dt = NULL;

    if (log->prefixes == 0)
        return NULL;

    dt = cdt_localtime();
    p = cstring_create_empty(0);

    if (log->prefixes & CLOG_FIELD_DATE) {
        tmp = cdt_to_cstring(dt, "%F");
        cstring_cat(p, "%s%c", cstring_valueof(tmp), log->separator);
        cstring_destroy(tmp);
    }

    if (log->prefixes & CLOG_FIELD_TIME) {
        tmp = cdt_to_cstring(dt, "%T.%1");
        cstring_cat(p, "%s%c", cstring_valueof(tmp), log->separator);
        cstring_destroy(tmp);
    }

    if (log->prefixes & CLOG_FIELD_TIMEZONE) {
        tmp = cdt_to_cstring(dt, "%Z");
        cstring_cat(p, "%s%c", cstring_valueof(tmp), log->separator);
        cstring_destroy(tmp);
    }

    cdt_destroy(dt);

    if (log->prefixes & CLOG_FIELD_PID)
        cstring_cat(p, "%d%c", getpid(), log->separator);

    if (log->prefixes & CLOG_FIELD_LEVEL)
        cstring_cat(p, "%s%c", level_to_string(level), log->separator);

    return p;
}

/*
 * Check if we have a last message saved that may be written to the log file.
 */
static bool has_last_message_to_write(clog_s *log)
{
    if (log->lmsg.count > 1)
        return true;

    return false;
}

/*
 * Check if we can write the last message counter to the log file.
 */
static bool needs_to_write_last_message(clog_s *log)
{
    if ((log->lmsg.count % log->max_repeat) == 0)
        return true;

    return false;
}

static void write_last_message_counter(clog_s *log, enum clog_level level)
{
    cstring_t *p = message_prefix(log, level);

    if (p != NULL) {
        fprintf(log->f, "%s", cstring_valueof(p));
        cstring_destroy(p);
    }

    /* TODO: Allow the user change this message */
    fprintf(log->f, CLOG_COUNTER_MSG, log->lmsg.count);
}

static void write_message(clog_s *log, enum clog_level level,
    const char *msg)
{
    cstring_t *p = message_prefix(log, level);

    if (compare_message(cstring_valueof(log->lmsg.msg), msg) == false) {
        if (has_last_message_to_write(log))
            write_last_message_counter(log, level);

        /* Write the current message */
        if (p != NULL) {
            fprintf(log->f, "%s%s\n", cstring_valueof(p), msg);
            cstring_destroy(p);
        } else
            fprintf(log->f, "%s\n", msg);
    } else
        if (needs_to_write_last_message(log))
            write_last_message_counter(log, level);
}

static void write_hex_message(clog_s *log, enum clog_level level,
    const void *data, unsigned int dsize)
{
    unsigned int i;
    char *ptr = (char *)data;
    cstring_t *p = message_prefix(log, level);

    /*
     * TODO: break lines in 80 columns
     */

    if (p != NULL) {
        fprintf(log->f, "%s%c", cstring_valueof(p), log->separator);
        cstring_destroy(p);
    }

    for (i = 0; i < dsize; i++)
        fprintf(log->f, "%02x", ptr[i]);

    fprintf(log->f, "\n");
}

static bool check_log_mode(clog_s *log, enum clog_mode mode)
{
    if (log->mode == mode)
        return true;

    return false;
}

/*
 *
 * Log messages API
 *
 */

clog_t LIBEXPORT *clog_open_ex(const char *pathname, enum clog_mode mode,
    enum clog_level start_level, unsigned int max_repeat, char separator,
    enum clog_prefix_field prefixes)
{
    clog_s *log = NULL;

    cerrno_clear();

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if ((is_mode_valid(mode) == false) ||
        (is_level_valid(start_level) == false) ||
        (is_separator_valid(separator) == false))
    {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    log = new_clog_s();

    if (NULL == log)
        return NULL;

    if (set_logfile_handle_mode(log, mode, pathname) < 0)
        goto error_block;

    log->level = start_level;
    log->max_repeat = max_repeat;
    log->separator = separator;
    log->prefixes = prefixes;

    return log;

error_block:
    ref_dec(&log->ref);
    return NULL;
}

clog_t LIBEXPORT *clog_open(const char *pathname, enum clog_mode mode,
    enum clog_level start_level, unsigned int max_repeat)
{
    /* Creates the default log format: DATE; TIME; PID; LEVEL; msg */
    return clog_open_ex(pathname, mode, start_level, max_repeat, CLOG_SEPARATOR,
                        CLOG_FIELD_DATE | CLOG_FIELD_TIME | CLOG_FIELD_PID |
                        CLOG_FIELD_LEVEL);
}

int LIBEXPORT clog_close(clog_t *log)
{
    clog_s *l = (clog_s *)log;

    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    ref_dec(&l->ref);

    return 0;
}

int LIBEXPORT clog_vprintf(clog_t *log, enum clog_level level, const char *fmt,
    va_list args)
{
    char *msg = NULL;

    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    if (is_level_valid(level) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    if (may_write_message(log, level) == false)
        /* It's not an error */
        return 1;

    lock_log_file(log);

    if (check_log_mode(log, CLOG_SYNC_ALL_MSGS))
        if (open_log_file(log) < 0) {
            unlock_log_file(log);
            return -1;
        }

    vasprintf(&msg, fmt, args);
    write_message(log, level, msg);
    save_written_message(log, msg);
    free(msg);

    if (check_log_mode(log, CLOG_SYNC_ALL_MSGS)) {
        sync_log_data(log);
        close_log_file(log);
    }

    unlock_log_file(log);

    return 0;
}

int LIBEXPORT clog_printf(clog_t *log, enum clog_level level,
    const char *fmt, ...)
{
    va_list ap;
    int ret = -1;

    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    va_start(ap, fmt);
    ret = clog_vprintf(log, level, fmt, ap);
    va_end(ap);

    return ret;
}

int LIBEXPORT clog_bprint(clog_t *log, enum clog_level level, const void *data,
    unsigned int dsize)
{
    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    if ((NULL == data) || (dsize == 0)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (is_level_valid(level) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    if (may_write_message(log, level) == false)
        /* It's not an error */
        return 1;

    lock_log_file(log);

    if (check_log_mode(log, CLOG_SYNC_ALL_MSGS))
        if (open_log_file(log) < 0) {
            unlock_log_file(log);
            return -1;
        }

    write_hex_message(log, level, data, dsize);

    if (check_log_mode(log, CLOG_SYNC_ALL_MSGS)) {
        sync_log_data(log);
        close_log_file(log);
    }

    unlock_log_file(log);

    return 0;
}

/* XXX: rprint? */
void clog_rprint(void)
{
}

int LIBEXPORT clog_set_log_level(clog_t *log, enum clog_level level)
{
    clog_s *l = (clog_s *)log;

    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    if (is_level_valid(level) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    l->level = level;

    return 0;
}

int LIBEXPORT clog_set_separator(clog_t *log, char separator)
{
    clog_s *l = (clog_s *)log;

    cerrno_clear();

    if (validate_object(log, CLOG) == false)
        return -1;

    if (is_separator_valid(separator) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    l->separator = separator;

    return 0;
}

