
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan 15 09:31:15 2016
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

#ifndef _COLLECTIONS_CL_UTILS_H
#define _COLLECTIONS_CL_UTILS_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_utils.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cbool_to_cstring
 * @brief Converts a bool type to a cstring_t text object.
 *
 * @param [in] flag: The bool value.
 *
 * @return On success returns a cstring_t object containing a string with 'true'
 *         or 'false' according to \a flag value or NULL otherwise.
 */
cstring_t *cbool_to_cstring(bool flag);

/**
 * @name cbool_to_string
 * @brief Converts a bool type to a C string type (char *).
 *
 * @param [in] flag: The bool value.
 *
 * @return On success returns a char * containing a string with 'true' or 'false'
 *         according to \a flag value or NULL otherwise.
 */
char *cbool_to_string(bool flag);

/**
 * @name collections_version
 * @brief Gets a string with the library version.
 *
 * @return On success returns a char * containing the library version or NULL
 *         otherwise.
 */
char *collections_version(void);

/**
 * @name cl_type_to_cstring
 * @brief Converts an enum cl_type value into a cstring_t object.
 *
 * @param [in] value: The enum cl_type value.
 *
 * @return On success returns a cstring_t object with the value as a text or
 *         NULL otherwise.
 */
cstring_t *cl_type_to_cstring(enum cl_type value);

#endif

