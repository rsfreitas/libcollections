
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
#define _COLLECTIONS_CL_LOG_H     1

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
    CLOG_KEPP_FILE_OPEN
};

enum clog_fmt_field {
    CLOG_FIELD_DATE     = (1 << 0),
    CLOG_FIELD_TIME     = (1 << 1),
    CLOG_FIELD_PID      = (1 << 2),
    CLOG_FIELD_LEVEL    = (1 << 3)
};

#endif

