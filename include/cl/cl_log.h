
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

enum cl_log_level {
    CL_LOG_OFF,
    CL_LOG_EMERG,
    CL_LOG_ALERT,
    CL_LOG_CRITI,
    CL_LOG_ERROR,
    CL_LOG_WARNG,
    CL_LOG_NOTICE,
    CL_LOG_INFO,
    CL_LOG_DEBUG
};

enum cl_log_mode {
    CL_LOG_SYNC_ALL_MSGS  = (1 << 0),
    CL_LOG_KEEP_FILE_OPEN = (1 << 1),
    CL_LOG_ROTATE_DAILY   = (1 << 2)  /* TODO */
};

enum cl_log_prefix_field {
    CL_LOG_FIELD_DATE     = (1 << 0),
    CL_LOG_FIELD_TIME     = (1 << 1),
    CL_LOG_FIELD_PID      = (1 << 2),
    CL_LOG_FIELD_LEVEL    = (1 << 3),
    CL_LOG_FIELD_TIMEZONE = (1 << 4)
};

/**
 * @name cl_log_open_ex
 * @brief Initialize a log file to write messages.
 *
 * Open or create if it does not exist, a file to write text messages. To be
 * used as a log file from applications.
 *
 * Two modes are available to manipulate the file: CL_LOG_SYNC_ALL_MSGS, where
 * every message will cause the log file to be opened and closed after write
 * the message or CL_LOG_KEEP_FILE_OPEN, where the file file will be opened and
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
 * @return On success returns a cl_log_t object or NULL otherwise.
 */
cl_log_t *cl_log_open_ex(const char *pathname, enum cl_log_mode mode,
                         enum cl_log_level start_level, unsigned int max_repeat,
                         char separator, enum cl_log_prefix_field prefixes);

/**
 * @name cl_log_open
 * @brief Initialize a log file to write messages.
 *
 * Open or create if it does not exist, a file to write text messages. To be
 * used as a log file from applications.
 *
 * Two modes are available to manipulate the file: CL_LOG_SYNC_ALL_MSGS, where
 * every message will cause the log file to be opened and closed after write
 * the message or CL_LOG_KEEP_FILE_OPEN, where the file file will be opened and
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
 * @return On success returns a cl_log_t object or NULL otherwise.
 */
cl_log_t *cl_log_open(const char *pathname, enum cl_log_mode mode,
                      enum cl_log_level start_level, unsigned int max_repeat);

/**
 * @name cl_log_close
 * @brief Close a log file.
 *
 * @param [in] log: The cl_log_t object which will be closed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_close(cl_log_t *log);

/**
 * @name cl_log_vprintf
 * @brief Write a message in a log file using variadic arguments.
 *
 * @param [in] log: The cl_log_t object.
 * @param [in] level: The message level.
 * @param [in] fmt: The message format.
 * @param [in] args: The message arguments.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_vprintf(cl_log_t *log, enum cl_log_level level, const char *fmt,
                   va_list args);

/**
 * @name cl_log_printf
 * @brief Write a message in a log file.
 *
 * @param [in] log: The cl_log_t object.
 * @param [in] level: The message level.
 * @param [in] fmt: The message format.
 * @param [in] ...: The message arguments.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_printf(cl_log_t *log, enum cl_log_level level, const char *fmt, ...);

/**
 * @name cl_log_bprint
 * @brief Write a binary message in a log file in hexadecimal format.
 *
 * @param [in] log: The cl_log_t object.
 * @param [in] level: The message level.
 * @param [in] data: The message content.
 * @param [in] dsize: The message size.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_bprint(cl_log_t *log, enum cl_log_level level, const void *data,
                  unsigned int dsize);

/**
 * @name cl_log_set_log_level
 * @brief Change the messages level allowed to be written in the log file.
 *
 * @param [in] log: The cl_log_t object.
 * @param [in] level: The new message level.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_set_log_level(cl_log_t *log, enum cl_log_level level);

/**
 * @name cl_log_set_separator
 * @brief Change the messages fields separator.
 *
 * @param [in] log: The cl_log_t object.
 * @param [in] separator: The new messages separator.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_log_set_separator(cl_log_t *log, char separator);

#endif

