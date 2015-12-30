
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 17:11:33 2015
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

#ifndef _COLLECTIONS_CL_OBJECT_H
#define _COLLECTIONS_CL_OBJECT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_object.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cobject_new
 * @brief Creates a new cobject_t object.
 *
 * If object is of CL_OBJECT_VOIDP type the variadic arguments should be
 * the binary content and its size. The variadic arguments should end with
 * NULL.
 *
 * @param [in] type: The object type of the new object.
 * @param [in] ...: The object value.
 *
 * @return On success returns a cobject_t object or NULL othewise.
 */
cobject_t *cobject_new(enum cl_type type, ...);

/**
 * @name cobject_destroy
 * @brief Releases a cobject_t from memory.
 *
 * @param [in,out] object: The cobject_t which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cobject_destroy(cobject_t *object);

/**
 * @name cobject_sizeof
 * @brief Gets the sizeof of a cobject_t.
 *
 * @param [in] object: The cobject_t value.
 *
 * @return On success returns the sizeof of the cobject_t or -1 otherwise.
 */
int cobject_sizeof(const cobject_t *object);

/**
 * @name cobject_type
 * @brief Gets the type of a cobject_t.
 *
 * @param [in] object: The cobject_t value.
 *
 * @return On success returns the cobject_t value type or -1 otherwise.
 */
enum cl_type cl_type(const cobject_t *object);

/**
 * @name cobject_set
 * @brief Changes the value of a cobject_t.
 *
 * @param [in] object: The cobject_t value.
 * @param [in] ...: The new value of the object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cobject_set(cobject_t *object, ...);

/**
 * @name cobject_get_int
 * @brief Gets the 'int' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
int cobject_get_int(const cobject_t *object);

/**
 * @name cobject_get_float
 * @brief Gets the 'float' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
float cobject_get_float(const cobject_t *object);

/**
 * @name cobject_get_double
 * @brief Gets the 'double' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
double cobject_get_double(const cobject_t *object);

/**
 * @name cobject_get_llong
 * @brief Gets the 'long long' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
long long cobject_get_llong(const cobject_t *object);

/**
 * @name cobject_get_string
 * @brief Gets the 'cstring_t' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or NULL otherwise.
 */
const cstring_t *cobject_get_string(const cobject_t *object);

/**
 * @name cobject_get_long
 * @brief Gets the 'long' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
long cobject_get_long(const cobject_t *object);

/**
 * @name cobject_get_char
 * @brief Gets the 'char' value of a cobject_t.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or -1 otherwise.
 */
char cobject_get_char(const cobject_t *object);

/**
 * @name cobject_get
 * @brief Gets the value of a cobject_t as 'void *'.
 *
 * @param [in] object: The cobject_t.
 *
 * @return On success returns the cobject_t value or NULL otherwise.
 */
void *cobject_get(const cobject_t *object, unsigned int *size);

#define COBJECT_INT(o)                  cobject_get_int((o))
#define COBJECT_FLOAT(o)                cobject_get_float((o))
#define COBJECT_DOUBLE(o)               cobject_get_double((o))
#define COBJECT_LONG(o)                 cobject_get_long((o))
#define COBJECT_LLONG(o)                cobject_get_llong((o))
#define COBJECT_STRING(o)               cobject_get_string((o))
#define COBJECT_CHAR(o)                 cobject_get_char((o))
#define COBJECT_PTR(o,s)                cobject_get((o),(s))

#endif

