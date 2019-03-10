
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

#define CL_LOG_SEPARATOR        ';'
#define CL_LOG_COUNTER_MSG      cl_tr_noop("Last message repeated %d times\n")

/* Structure used to store the last message written in the log file */
struct last_msg {
    cl_string_t     *msg;
    unsigned int    count;
};

#define cl_log_members                                      \
    cl_struct_member(FILE *, f)                             \
    cl_struct_member(char *, pathname)                      \
    cl_struct_member(enum cl_log_mode, mode)                \
    cl_struct_member(enum cl_log_level, level)              \
    cl_struct_member(enum cl_log_prefix_field, prefixes)    \
    cl_struct_member(unsigned int, max_repeat)              \
    cl_struct_member(char, separator)                       \
    cl_struct_member(struct last_msg, lmsg)                 \
    cl_struct_member(pthread_mutex_t, lock)                 \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_log_s, cl_log_members);
#define cl_log_s          cl_struct(cl_log_s)

static void close_log_file(cl_log_s *log);

/*
 *
 * Internal functions
 *
 */

static bool is_mode_valid(enum cl_log_mode mode)
{
    switch (mode) {
    case CL_LOG_SYNC_ALL_MSGS:
    case CL_LOG_KEEP_FILE_OPEN:
        return true;

    default:
        break;
    }

    return false;
}

static bool is_level_valid(enum cl_log_level level)
{
    switch (level) {
    case CL_LOG_OFF:
    case CL_LOG_EMERG:
    case CL_LOG_ALERT:
    case CL_LOG_CRITI:
    case CL_LOG_ERROR:
    case CL_LOG_WARNG:
    case CL_LOG_NOTICE:
    case CL_LOG_INFO:
    case CL_LOG_DEBUG:
        return true;
    }

    return false;
}

static const char *level_to_string(enum cl_log_level level)
{
    switch (level) {
    case CL_LOG_OFF:
        return NULL;

    case CL_LOG_EMERG:
        return "EMERG";

    case CL_LOG_ALERT:
        return "ALERT";

    case CL_LOG_CRITI:
        return "CRITI";

    case CL_LOG_ERROR:
        return "ERROR";

    case CL_LOG_WARNG:
        return "WARNG";

    case CL_LOG_NOTICE:
        return "NOTIC";

    case CL_LOG_INFO:
        return "INFOM";

    case CL_LOG_DEBUG:
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

static bool may_write_message(cl_log_s *log, enum cl_log_level level)
{
    if (level <= log->level)
        return true;

    return false;
}

static void save_written_message(cl_log_s *log, const char *msg)
{
    if (compare_message(cl_string_valueof(log->lmsg.msg), msg) == true)
        log->lmsg.count++;
    else {
        if (log->lmsg.msg != NULL)
            cl_string_destroy(log->lmsg.msg);

        log->lmsg.msg = cl_string_create("%s", msg);
        log->lmsg.count = 1;
    }
}

/*
 * Releases a 'cl_log_s' from memory. Function to be called when its
 * reference count drops to 0.
 */
static void __destroy_cl_log_s(const struct cl_ref_s *ref)
{
    cl_log_s *l = cl_container_of(ref, cl_log_s, ref);

    if (NULL == l)
        return;

    if (l->pathname != NULL)
        free(l->pathname);

    if (l->mode == CL_LOG_KEEP_FILE_OPEN)
        close_log_file(l);

    if (l->lmsg.msg != NULL)
        cl_string_destroy(l->lmsg.msg);

    free(l);
    l = NULL;
}

/*
 * Creates a new 'cl_log_s'.
 */
static cl_log_s *new_cl_log_s(void)
{
    cl_log_s *l = NULL;

    l = calloc(1, sizeof(cl_log_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->ref.count = 1;
    l->ref.free = __destroy_cl_log_s;
    pthread_mutex_init(&l->lock, NULL);

    typeof_set(CL_OBJ_LOG, l);

    return l;
}

static int open_log_file(cl_log_s *log)
{
    log->f = fopen(log->pathname, "a");

    if (NULL == log->f) {
        cset_errno(CL_FILE_OPEN_ERROR);
        return -1;
    }

    return 0;
}

static void close_log_file(cl_log_s *log)
{
    if (log->f != NULL)
        fclose(log->f);
}

static void sync_log_data(cl_log_s *log)
{
    if (log->f != NULL)
        fflush(log->f);
}

static void lock_log_file(cl_log_s *log)
{
    pthread_mutex_lock(&log->lock);
}

static void unlock_log_file(cl_log_s *log)
{
    pthread_mutex_unlock(&log->lock);
}

/*
 * Sets the way the log file will be handled.
 *
 * - CL_LOG_SYNC_ALL_MSGS: every message will cause the log file to be opened,
 *                         the message will be written and then the file closed.
 *
 * - CL_LOG_KEEP_FILE_OPEN: the file will be opened as long as the @log is active
 *                          inside the application.
 */
static int set_logfile_handle_mode(cl_log_s *log, enum cl_log_mode mode,
    const char *pathname)
{
    log->f = NULL;
    log->pathname = strdup(pathname);
    log->mode = mode;

    if (mode == CL_LOG_KEEP_FILE_OPEN)
        return open_log_file(log);

    return 0;
}

static cl_string_t *message_prefix(cl_log_s *log, enum cl_log_level level)
{
    cl_string_t *p = NULL, *tmp = NULL;
    cl_datetime_t *dt = NULL;

    if (log->prefixes == 0)
        return NULL;

    dt = cl_dt_localtime();
    p = cl_string_create_empty(0);

    if (log->prefixes & CL_LOG_FIELD_DATE) {
        tmp = cl_dt_to_cstring(dt, "%F");
        cl_string_cat(p, "%s%c", cl_string_valueof(tmp), log->separator);
        cl_string_destroy(tmp);
    }

    if (log->prefixes & CL_LOG_FIELD_TIME) {
        tmp = cl_dt_to_cstring(dt, "%T.%1");
        cl_string_cat(p, "%s%c", cl_string_valueof(tmp), log->separator);
        cl_string_destroy(tmp);
    }

    if (log->prefixes & CL_LOG_FIELD_TIMEZONE) {
        tmp = cl_dt_to_cstring(dt, "%Z");
        cl_string_cat(p, "%s%c", cl_string_valueof(tmp), log->separator);
        cl_string_destroy(tmp);
    }

    cl_dt_destroy(dt);

    if (log->prefixes & CL_LOG_FIELD_PID)
        cl_string_cat(p, "%d%c", getpid(), log->separator);

    if (log->prefixes & CL_LOG_FIELD_LEVEL)
        cl_string_cat(p, "%s%c", level_to_string(level), log->separator);

    return p;
}

/*
 * Check if we have a last message saved that may be written to the log file.
 */
static bool has_last_message_to_write(cl_log_s *log)
{
    if (log->max_repeat == 0)
        return false;

    if (log->lmsg.count > 1)
        return true;

    return false;
}

/*
 * Check if we can write the last message counter to the log file.
 */
static bool needs_to_write_last_message(cl_log_s *log)
{
    if (log->max_repeat == 0)
        return false;

    if ((log->lmsg.count % log->max_repeat) == 0)
        return true;

    return false;
}

static void write_last_message_counter(cl_log_s *log, enum cl_log_level level)
{
    cl_string_t *p = message_prefix(log, level);

    if (p != NULL) {
        fprintf(log->f, "%s", cl_string_valueof(p));
        cl_string_destroy(p);
    }

    /* TODO: Allow the user change this message */
    fprintf(log->f, CL_LOG_COUNTER_MSG, log->lmsg.count);
}

static void write_message(cl_log_s *log, enum cl_log_level level,
    const char *msg)
{
    cl_string_t *p = message_prefix(log, level);
    bool write = false;

    if (log->max_repeat == 0)
        write = true;
    else {
        if (compare_message(cl_string_valueof(log->lmsg.msg), msg) == true) {
            if (needs_to_write_last_message(log)) {
                write_last_message_counter(log, level);
                return;
            } else
                write = true;
        } else
            write = true;
    }

    if (write) {
        if (has_last_message_to_write(log))
            write_last_message_counter(log, level);

        /* Write the current message */
        if (p != NULL) {
            fprintf(log->f, "%s%s\n", cl_string_valueof(p), msg);
            cl_string_destroy(p);
        } else
            fprintf(log->f, "%s\n", msg);
    }
}

static void write_hex_message(cl_log_s *log, enum cl_log_level level,
    const void *data, unsigned int dsize)
{
    unsigned int i;
    char *ptr = (char *)data;
    cl_string_t *p = message_prefix(log, level);

    /*
     * TODO: break lines in 80 columns
     */

    if (p != NULL) {
        fprintf(log->f, "%s%c", cl_string_valueof(p), log->separator);
        cl_string_destroy(p);
    }

    for (i = 0; i < dsize; i++)
        fprintf(log->f, "%02x", ptr[i]);

    fprintf(log->f, "\n");
}

static bool check_log_mode(cl_log_s *log, enum cl_log_mode mode)
{
    if (log->mode == mode)
        return true;

    return false;
}

/*
 *
 * API
 *
 */

cl_log_t *cl_log_open_ex(const char *pathname, enum cl_log_mode mode,
    enum cl_log_level start_level, unsigned int max_repeat, char separator,
    enum cl_log_prefix_field prefixes)
{
    cl_log_s *log = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

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

    log = new_cl_log_s();

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
    cl_ref_dec(&log->ref);
    return NULL;
}

cl_log_t *cl_log_open(const char *pathname, enum cl_log_mode mode,
    enum cl_log_level start_level, unsigned int max_repeat)
{
    /* Creates the default log format: DATE; TIME; PID; LEVEL; msg */
    return cl_log_open_ex(pathname, mode, start_level, max_repeat,
                          CL_LOG_SEPARATOR,
                          CL_LOG_FIELD_DATE | CL_LOG_FIELD_TIME |
                          CL_LOG_FIELD_PID | CL_LOG_FIELD_LEVEL);
}

int cl_log_close(cl_log_t *log)
{
    cl_log_s *l = (cl_log_s *)log;

    __clib_function_init__(true, log, CL_OBJ_LOG, -1);
    cl_ref_dec(&l->ref);

    return 0;
}

int cl_log_vprintf(cl_log_t *log, enum cl_log_level level,
    const char *fmt, va_list args)
{
    char *msg = NULL;

    __clib_function_init__(true, log, CL_OBJ_LOG, -1);

    if (is_level_valid(level) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    if (may_write_message(log, level) == false)
        /* It's not an error */
        return 1;

    lock_log_file(log);

    if (check_log_mode(log, CL_LOG_SYNC_ALL_MSGS))
        if (open_log_file(log) < 0) {
            unlock_log_file(log);
            return -1;
        }

    vasprintf(&msg, fmt, args);
    write_message(log, level, msg);
    save_written_message(log, msg);
    free(msg);

    if (check_log_mode(log, CL_LOG_SYNC_ALL_MSGS)) {
        sync_log_data(log);
        close_log_file(log);
    }

    unlock_log_file(log);

    return 0;
}

int cl_log_printf(cl_log_t *log, enum cl_log_level level,
    const char *fmt, ...)
{
    va_list ap;
    int ret = -1;

    __clib_function_init__(true, log, CL_OBJ_LOG, -1);

    va_start(ap, fmt);
    ret = cl_log_vprintf(log, level, fmt, ap);
    va_end(ap);

    return ret;
}

int cl_log_bprint(cl_log_t *log, enum cl_log_level level,
    const void *data, unsigned int dsize)
{
    __clib_function_init__(true, log, CL_OBJ_LOG, -1);

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

    if (check_log_mode(log, CL_LOG_SYNC_ALL_MSGS))
        if (open_log_file(log) < 0) {
            unlock_log_file(log);
            return -1;
        }

    write_hex_message(log, level, data, dsize);

    if (check_log_mode(log, CL_LOG_SYNC_ALL_MSGS)) {
        sync_log_data(log);
        close_log_file(log);
    }

    unlock_log_file(log);

    return 0;
}

int cl_log_set_log_level(cl_log_t *log, enum cl_log_level level)
{
    cl_log_s *l = (cl_log_s *)log;

    __clib_function_init__(true, log, CL_OBJ_LOG, -1);

    if (is_level_valid(level) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    l->level = level;

    return 0;
}

int cl_log_set_separator(cl_log_t *log, char separator)
{
    cl_log_s *l = (cl_log_s *)log;

    __clib_function_init__(true, log, CL_OBJ_LOG, -1);

    if (is_separator_valid(separator) == false) {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    l->separator = separator;

    return 0;
}

