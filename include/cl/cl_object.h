
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 17:11:33 2015
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

#ifndef _COLLECTIONS_CL_OBJECT_H
#define _COLLECTIONS_CL_OBJECT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_object.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cobject_create
 * @brief Creates a new cobject_t object.
 *
 * If object is of CL_OBJECT_VOIDP type the variadic arguments should be
 * the binary content and its size. The variadic arguments should end with
 * NULL.
 *
 * @param [in] type: The object type of the new object.
 * @param [in] ...: The item object.
 *
 * @return On success returns a cobject_t object or NULL othewise.
 */
cobject_t *cobject_create(enum cl_type type, ...);

/**
 * @name cobject_create_with_spec
 * @brief Creates a new cobject_t with parameters specification support.
 *
 * @param [in] type: The object type of the new object.
 * @param [in] spec: The parameters specification.
 *
 * @return On success returns a cobject_t object or NULL othewise.
 */
cobject_t *cobject_create_with_spec(enum cl_type type, cspec_t *spec);

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
 * @param [in] object: The cobject_t object.
 *
 * @return On success returns the sizeof of the cobject_t or -1 otherwise.
 */
int cobject_sizeof(const cobject_t *object);

/**
 * @name cobject_type
 * @brief Gets the type of a cobject_t.
 *
 * @param [in] object: The cobject_t object.
 *
 * @return On success returns the cobject_t object type or -1 otherwise.
 */
enum cl_type cobject_type(const cobject_t *object);

/**
 * @name cobject_set
 * @brief Changes the object of a cobject_t.
 *
 * @param [in] object: The cobject_t object.
 * @param [in] ...: The new object of the object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cobject_set(cobject_t *object, ...);

/**
 * @name cobject_get
 * @brief Gets the content from a cobject_t object.
 *
 * @param [in] object: The cobject_t object.
 * @param [in] fmt: The output format of the object.
 * @param [out] ...: The output object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cobject_get(const cobject_t *object, const char *fmt, ...);

/**
 * @name cobject_is_of_type
 * @brief Checks if a cobject_t is from a specific type.
 *
 * @param [in] object: The cobject_t.
 * @param [in] type: The type to check.
 *
 * @return Returns true if \a object is of \a type type or false
 *         otherwise.
 */
bool cobject_is_of_type(const cobject_t *object, unsigned int type);

/**
 * @name cobject_to_cstring
 * @brief Converts a cobject_t object into a human readable string (cstring_t).
 *
 * @param [in] object: The cobject_t object.
 *
 * @return On success returns a cstring_t containing the cobject_t or NULL
 *         otherwise.
 */
cstring_t *cobject_to_cstring(const cobject_t *object);

/**
 * @name cobject_from_cstring
 * @brief Creates a cobject_t from a cstring_t object.
 *
 * @param [in] object: The cstring_t object.
 *
 * @return On success returns the new cobject_t or NULL otherwise.
 */
cobject_t *cobject_from_cstring(const cstring_t *object);

/**
 * @name cobject_ref
 * @brief Increases the reference count for a cobject_t item.
 *
 * @param [in,out] object: The cobject_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cobject_t *cobject_ref(cobject_t *object);

/**
 * @name cobject_unref
 * @brief Decreases the reference count for a cobject_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] object: The cobject_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cobject_unref(cobject_t *object);

/** Macros to get the cobject_t item object */
#define COBJECT_AS_INT(v)       \
    ({ int __x; cobject_get(v, COBJECT_INT, &__x, NULL); __x; })

#define COBJECT_AS_UINT(v)       \
    ({ unsigned int __x; cobject_get(v, COBJECT_UINT, &__x, NULL); __x; })

#define COBJECT_AS_LONG(v)       \
    ({ long __x; cobject_get(v, COBJECT_LONG, &__x, NULL); __x; })

#define COBJECT_AS_ULONG(v)       \
    ({ unsigned long __x; cobject_get(v, COBJECT_ULONG, &__x, NULL); __x; })

#define COBJECT_AS_LLONG(v)       \
    ({ long long __x; cobject_get(v, COBJECT_LLONG, &__x, NULL); __x; })

#define COBJECT_AS_ULLONG(v)       \
    ({ unsigned long long __x; cobject_get(v, COBJECT_ULLONG, &__x, NULL); __x; })

#define COBJECT_AS_SINT(v)       \
    ({ short int __x; cobject_get(v, COBJECT_SINT, &__x, NULL); __x; })

#define COBJECT_AS_USINT(v)       \
    ({ unsigned short int __x; cobject_get(v, COBJECT_USINT, &__x, NULL); __x; })

#define COBJECT_AS_CHAR(v)       \
    ({ char __x; cobject_get(v, COBJECT_CHAR, &__x, NULL); __x; })

#define COBJECT_AS_UCHAR(v)       \
    ({ unsigned char __x; cobject_get(v, COBJECT_UCHAR, &__x, NULL); __x; })

#define COBJECT_AS_FLOAT(v)      \
    ({ float __x; cobject_get(v, COBJECT_FLOAT, &__x, NULL); __x; })

#define COBJECT_AS_DOUBLE(v)     \
    ({ double __x; cobject_get(v, COBJECT_DOUBLE, &__x, NULL); __x; })

#define COBJECT_AS_BOOLEAN(v)    \
    ({ bool __x; cobject_get(v, COBJECT_BOOLEAN, &__x, NULL); __x; })

#define COBJECT_AS_STRING(v)     \
    ({ char *__x; cobject_get(v, COBJECT_STRING, &__x, NULL); __x; })

#define COBJECT_AS_CSTRING(v)    \
    ({ cstring_t *__x; cobject_get(v, COBJECT_CSTRING, &__x, NULL); __x; })

#define COBJECT_AS_POINTER(v)    \
    ({ int __s; void *__x; cobject_get(v, COBJECT_POINTER, &__x, &__s, NULL); __x; })

/** Macros to check if an item correspond to a specific type */
#define COBJECT_isvoid(o)        cobject_is_of_type((o), CL_VOID)
#define COBJECT_ischar(o)        cobject_is_of_type((o), CL_CHAR)
#define COBJECT_isuchar(o)       cobject_is_of_type((o), CL_UCHAR)
#define COBJECT_isint(o)         cobject_is_of_type((o), CL_INT)
#define COBJECT_isuint(o)        cobject_is_of_type((o), CL_UINT)
#define COBJECT_issint(o)        cobject_is_of_type((o), CL_SINT)
#define COBJECT_isusint(o)       cobject_is_of_type((o), CL_USINT)
#define COBJECT_isfloat(o)       cobject_is_of_type((o), CL_FLOAT)
#define COBJECT_isdouble(o)      cobject_is_of_type((o), CL_DOUBLE)
#define COBJECT_islong(o)        cobject_is_of_type((o), CL_LONG)
#define COBJECT_isulong(o)       cobject_is_of_type((o), CL_ULONG)
#define COBJECT_isllong(o)       cobject_is_of_type((o), CL_LLONG)
#define COBJECT_isullong(o)      cobject_is_of_type((o), CL_ULLONG)
#define COBJECT_isstring(o)      cobject_is_of_type((o), CL_STRING)
#define COBJECT_iscstring(o)     cobject_is_of_type((o), CL_CSTRING)
#define COBJECT_isboolean(o)     cobject_is_of_type((o), CL_BOOLEAN)

#endif

