
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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

#ifndef _COLLECTIONS_CL_ERROR_H
#define _COLLECTIONS_CL_ERROR_H                            1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cerror.h> directly; include <collections.h> instead."
# endif
#endif

/** Error codes from all internal APIs */
enum cerror_code {
    CL_NO_ERROR = 0,

    CL_NULL_ARG,
    CL_WRONG_STRING_INDEX,
    CL_NO_MEM,
    CL_OBJECT_NOT_FOUND,
    CL_NUMBER_RANGE,
    CL_NOT_A_NUMBER,
    CL_PARSE_ERROR,
    CL_NO_TTY,
    CL_TTY_GET_ATTR,
    CL_TTY_SET_ATTR_TCSAFLUSH,
    CL_FILE_OPEN_ERROR,
    CL_UNRECOGNIZED_CFG_LINE,
    CL_FORK_FAILED,
    CL_INVALID_STATE,
    CL_CREATE_FAILED,
    CL_EVENT_CONDITIONS_WRONGLY_INITIALIZED,
    CL_UNSUPPORTED_TYPE,
    CL_NULL_DATA,
    CL_ENDED_WITH_TIMEOUT,
    CL_EXTERNAL_INIT_ERROR,
    CL_EXTERNAL_UNINIT_ERROR,
    CL_SETTIME_FAILED,
    CL_PSELECT_FAILED,
    CL_ACCEPT_FAILED,
    CL_RECV_FAILED,
    CL_SEND_FAILED,
    CL_CONNECT_FAILED,
    CL_IPC_INIT_FAILED,
    CL_IPC_UNINIT_FAILED,
    CL_IPC_SET_UP_FAILED,
    CL_DRIVER_INIT_FAILED,
    CL_DRIVER_UNINIT_FAILED,
    CL_ESTABLISHED_CONNECTION_FAILED,
    CL_CONNECTION_RECEIVED_FAILED,
    CL_PREPARE_TO_SEND_FAILED,
    CL_PROCESS_RECV_DATA_FAILED,
    CL_WRONG_TYPE,
    CL_SET_FAILED,
    CL_INVALID_VALUE,
    CL_FILE_NOT_FOUND,
    CL_DLOPEN,
    CL_DLCLOSE,
    CL_VALUE_NOT_FOUND,
    CL_NO_PLUGIN_INFO,
    CL_PLUGIN_STARTUP,
    CL_PLUGIN_SHUTDOWN,
    CL_PY_IMPORT_FAILED,
    CL_PY_GET_DICT_FAILED,
    CL_CVT_DATA_FAILED,
    CL_LIB_WAS_NOT_STARTED,
    CL_MKDIR_FAILED,
    CL_HASHTABLE_COLLISION,
    CL_UNSUPPORTED_RAW_IMAGE,
    CL_UNABLE_TO_LOAD_IMAGE,
    CL_UNABLE_TO_CREATE_TMP_IMAGE,

    CL_MAX_ERROR_CODE
};

/**
 * @name cerrno_storage
 * @brief Gets a pointer to the global thread specific error variable.
 *
 * @return Returns a pointer to the global error variable.
 */
cerrno *cerrno_storage(void);

/**
 * @name cexit
 * @brief Terminate calling thread.
 *
 * This function be must called at the end of main function if the user wants no
 * memory leak errors reported by the valgrind tool.
 */
void cexit(void);

/**
 * @name cget_last_error
 * @brief Gets the last error code internally occurred.
 *
 * @return Returns the last error code.
 */
enum cerror_code cget_last_error(void);

/**
 * @name cstrerror
 * @brief Converts a numeric error code in a text message.
 *
 * @param [in] error_code: Numeric error code.
 *
 * @return Returns the text corresponding the error code.
 */
const char *cstrerror(enum cerror_code error_code);

#endif

