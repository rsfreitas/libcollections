
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _COLLECTIONS_CL_IO_H
#define _COLLECTIONS_CL_IO_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_io.h> directly; include <collections.h> instead."
# endif
#endif

#define CL_NO_KEY                    1100

/* Special keys */
#define CL_CURSOR_UP                 300
#define CL_CURSOR_DOWN               301
#define CL_CURSOR_RIGHT              302
#define CL_CURSOR_LEFT               303
#define CL_FUNCTION_1                400
#define CL_FUNCTION_2                401
#define CL_FUNCTION_3                402
#define CL_FUNCTION_4                403
#define CL_FUNCTION_5                404
#define CL_FUNCTION_6                405
#define CL_FUNCTION_7                406
#define CL_FUNCTION_8                407
#define CL_FUNCTION_9                408
#define CL_FUNCTION_10               409
#define CL_FUNCTION_11               410
#define CL_FUNCTION_12               411
#define CL_PAGE_NEXT                 500
#define CL_PAGE_PREV                 501
#define CL_HOME_KEY                  502
#define CL_INSERT_KEY                503
#define CL_END_KEY                   504
#define CL_DELETE_KEY                508 
#define CL_BACKSPACE                 509

/**
 * @name cstdin_getline
 * @brief Reads a string from stdin until it finds a 'new line'.
 *
 * @return On success returns the string or NULL otherwise.
 */
cstring_t *cstdin_getline(void);

/**
 * @name cstdin_select
 * @brief Checks for something to be read from standard input.
 *
 * @return Returns true if is something to be read ou false otherwise.
 */
bool cstdin_select(void);

/**
 * @name cgetkey
 * @brief Reads a key from standard input.
 *
 * @param [in] block: Boolean flag to indicate whether the reading will block
 *                    or not.
 *
 * @return On success returns the ascii value of the captured key or -1
 *         otherwise. If \a block is false it may return CL_NO_KEY if no
 *         input is found.
 */
int cgetkey(bool block);

/**
 * @name cdisable_echo
 * @brief Disable echoing the key captured from the standard input.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cdisable_echo(void);

/**
 * @name cenable_echo
 * @brief Enable echoing the key captured from the standard input.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cenable_echo(void);

#endif

