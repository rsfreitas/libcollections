
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Apr 22 11:43:46 2016
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

#ifndef _COLLECTIONS_CL_LOG_H
#define _COLLECTIONS_CL_LOG_H       1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_log.h> directly; include <collections.h> instead."
# endif
#endif

enum clog_level {
    CLOG_OFF,
    CLOG_EMERG,
    CLOG_ALERT,
    CLOG_CRITI,
    CLOG_ERROR,
    CLOG_WARNG,
    CLOG_NOTICE,
    CLOG_INFO,
    CLOG_DEBUG
};

enum clog_mode {
    CLOG_SYNC_ALL_MSGS,
    CLOG_KEEP_FILE_OPEN
};

enum clog_prefix_field {
    CLOG_FIELD_DATE     = (1 << 0),
    CLOG_FIELD_TIME     = (1 << 1),
    CLOG_FIELD_PID      = (1 << 2),
    CLOG_FIELD_LEVEL    = (1 << 3)
};

/**
 * @name clog_open_ex
 * @brief Initialize a log file to write messages.
 *
 * Open or create if it does not exist, a file to write text messages. To be
 * used as a log file from applications.
 *
 * Two modes are available to manipulate the file: CLOG_SYNC_ALL_MSGS, where
 * every message will cause the log file to be opened and closed after write
 * the message or CLOG_KEEP_FILE_OPEN, where the file file will be opened and
 * keep opened as long as the log is active.
 *
 * @param [in] pathname: The log (complete) filename.
 * @param [in] mode: The file manipulation mode.
 * @param [in] start_level: A start level to the messages which will be written.
 * @param [in] max_repeat: An integer value to prevent repeated messages from
 *                         been written into the log file.
 * @param [in] separator: A character that will be used as a delimiter between
 *                        values in each line from the file.
 * @param [in] prefixes: A bitwise field to set which prefixes will be written
 *                       with all messages.
 *
 * @return On success returns a clog_t object or NULL otherwise.
 */
clog_t *clog_open_ex(const char *pathname, enum clog_mode mode,
                     enum clog_level start_level, unsigned int max_repeat,
                     char separator, enum clog_prefix_field prefixes);

/**
 * @name clog_open
 * @brief Initialize a log file to write messages.
 *
 * Open or create if it does not exist, a file to write text messages. To be
 * used as a log file from applications.
 *
 * Two modes are available to manipulate the file: CLOG_SYNC_ALL_MSGS, where
 * every message will cause the log file to be opened and closed after write
 * the message or CLOG_KEEP_FILE_OPEN, where the file file will be opened and
 * keep opened as long as the log is active.
 *
 * This function assumes a default separator and prefixes for the messages.
 *
 * @param [in] pathname: The log (complete) filename.
 * @param [in] mode: The file manipulation mode.
 * @param [in] start_level: A start level to the messages which will be written.
 * @param [in] max_repeat: An integer value to prevent repeated messages from
 *                         been written into the log file.
 *
 * @return On success returns a clog_t object or NULL otherwise.
 */
clog_t *clog_open(const char *pathname, enum clog_mode mode,
                  enum clog_level start_level, unsigned int max_repeat);

/**
 * @name clog_close
 * @brief Close a log file.
 *
 * @param [in] log: The clog_t object which will be closed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_close(clog_t *log);

/**
 * @name clog_vprintf
 * @brief Write a message in a log file using variadic arguments.
 *
 * @param [in] log: The clog_t object.
 * @param [in] level: The message level.
 * @param [in] fmt: The message format.
 * @param [in] args: The message arguments.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_vprintf(clog_t *log, enum clog_level level, const char *fmt,
                 va_list args);

/**
 * @name clog_printf
 * @brief Write a message in a log file.
 *
 * @param [in] log: The clog_t object.
 * @param [in] level: The message level.
 * @param [in] fmt: The message format.
 * @param [in] ...: The message arguments.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_printf(clog_t *log, enum clog_level level, const char *fmt, ...);

/**
 * @name clog_bprint
 * @brief Write a binary message in a log file in hexadecimal format.
 *
 * @param [in] log: The clog_t object.
 * @param [in] level: The message level.
 * @param [in] data: The message content.
 * @param [in] dsize: The message size.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_bprint(clog_t *log, enum clog_level level, const void *data,
                unsigned int dsize);

/**
 * @name clog_set_log_level
 * @brief Change the messages level allowed to be written in the log file.
 *
 * @param [in] log: The clog_t object.
 * @param [in] level: The new message level.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_set_log_level(clog_t *log, enum clog_level level);

/**
 * @name clog_set_separator
 * @brief Change the messages fields separator.
 *
 * @param [in] log: The clog_t object.
 * @param [in] separator: The new messages separator.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clog_set_separator(clog_t *log, char separator);

#endif

