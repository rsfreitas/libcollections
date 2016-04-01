
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

#ifndef _COLLECTIONS_CL_VALUE_H
#define _COLLECTIONS_CL_VALUE_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_value.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cvalue_create
 * @brief Creates a new cvalue_t value.
 *
 * If value is of CL_VALUE_VOIDP type the variadic arguments should be
 * the binary content and its size. The variadic arguments should end with
 * NULL.
 *
 * @param [in] type: The value type of the new value.
 * @param [in] ...: The item value.
 *
 * @return On success returns a cvalue_t value or NULL othewise.
 */
cvalue_t *cvalue_create(enum cl_type type, ...);

/**
 * @name cvalue_create_with_spec
 * @brief Creates a new cvalue_t with parameters specification support.
 *
 * @param [in] type: The value type of the new value.
 * @param [in] spec: The parameters specification.
 *
 * @return On success returns a cvalue_t value or NULL othewise.
 */
cvalue_t *cvalue_create_with_spec(enum cl_type type, cspec_t *spec);

/**
 * @name cvalue_destroy
 * @brief Releases a cvalue_t from memory.
 *
 * @param [in,out] value: The cvalue_t which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cvalue_destroy(cvalue_t *value);

/**
 * @name cvalue_sizeof
 * @brief Gets the sizeof of a cvalue_t.
 *
 * @param [in] value: The cvalue_t value.
 *
 * @return On success returns the sizeof of the cvalue_t or -1 otherwise.
 */
int cvalue_sizeof(const cvalue_t *value);

/**
 * @name cvalue_type
 * @brief Gets the type of a cvalue_t.
 *
 * @param [in] value: The cvalue_t value.
 *
 * @return On success returns the cvalue_t value type or -1 otherwise.
 */
enum cl_type cvalue_type(const cvalue_t *value);

/**
 * @name cvalue_set
 * @brief Changes the value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t value.
 * @param [in] ...: The new value of the value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cvalue_set(cvalue_t *value, ...);

/**
 * @name cvalue_get
 * @brief Gets the real value of a cvalue_t object.
 *
 * @param [in] value: The cvalue_t.
 * @param [in] fmt: The output format.
 * @param [out] ...: The output values.
 *
<<<<<<< HEAD
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
int cvalue_get_int(const cvalue_t *value);

/**
 * @name cvalue_get_uint
 * @brief Gets the 'unsigned int' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or 0 otherwise.
 */
unsigned int cvalue_get_uint(const cvalue_t *value);

/**
 * @name cvalue_get_sint
 * @brief Gets the 'short int' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
short int cvalue_get_sint(const cvalue_t *value);

/**
 * @name cvalue_get_usint
 * @brief Gets the 'unsigned short int' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or 0 otherwise.
 */
unsigned short int cvalue_get_usint(const cvalue_t *value);

/**
 * @name cvalue_get_float
 * @brief Gets the 'float' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
float cvalue_get_float(const cvalue_t *value);

/**
 * @name cvalue_get_double
 * @brief Gets the 'double' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
double cvalue_get_double(const cvalue_t *value);

/**
 * @name cvalue_get_llong
 * @brief Gets the 'long long' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
long long cvalue_get_llong(const cvalue_t *value);

/**
 * @name cvalue_get_ullong
 * @brief Gets the 'unsigned long long' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or 0 otherwise.
 */
unsigned long long cvalue_get_ullong(const cvalue_t *value);

/**
 * @name cvalue_get_string
 * @brief Gets the string (char *) value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or NULL otherwise.
 */
const char *cvalue_get_string(const cvalue_t *value);

/**
 * @name cvalue_get_cstring
 * @brief Gets the 'cstring_t' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or NULL otherwise.
 */
cstring_t *cvalue_get_cstring(const cvalue_t *value);

/**
 * @name cvalue_get_long
 * @brief Gets the 'long' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
long cvalue_get_long(const cvalue_t *value);

/**
 * @name cvalue_get_ulong
 * @brief Gets the 'unsigned long' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or 0 otherwise.
 */
unsigned long cvalue_get_ulong(const cvalue_t *value);

/**
 * @name cvalue_get_char
 * @brief Gets the 'char' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
 */
char cvalue_get_char(const cvalue_t *value);

/**
 * @name cvalue_get_uchar
 * @brief Gets the 'unsigned char' value of a cvalue_t.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or 0 otherwise.
 */
unsigned char cvalue_get_uchar(const cvalue_t *value);

/**
 * @name cvalue_get_pointer
 * @brief Gets the value of a cvalue_t as 'void *'.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or NULL otherwise.
 */
void *cvalue_get_pointer(const cvalue_t *value, unsigned int *size);

/**
 * @name cvalue_get_boolean
 * @brief Gets the value of a cvalue_t as 'bool'.
 *
 * @param [in] value: The cvalue_t.
 *
 * @return On success returns the cvalue_t value or -1 otherwise.
=======
 * @return On success returns 0 or -1 otherwise.
>>>>>>> 8af625c22b34aeb3a7745f3d2bd4b9e91b6afa64
 */
int cvalue_get(const cvalue_t *value, const char *fmt, ...);

/**
 * @name cvalue_is_of_type
 * @brief Checks if a cvalue_t is from a specific type.
 *
 * @param [in] value: The cvalue_t.
 * @param [in] type: The type to check.
 *
 * @return Returns true if \a value is of \a type type or false
 *         otherwise.
 */
bool cvalue_is_of_type(const cvalue_t *value, unsigned int type);

/**
 * @name cvalue_to_cstring
 * @brief Converts a cvalue_t value into a human readable string (cstring_t).
 *
 * @param [in] value: The cvalue_t value.
 *
 * @return On success returns a cstring_t containing the cvalue_t or NULL
 *         otherwise.
 */
cstring_t *cvalue_to_cstring(const cvalue_t *value);

/**
 * @name cvalue_from_cstring
 * @brief Creates a cvalue_t from a cstring_t value.
 *
 * @param [in] value: The cstring_t value.
 *
 * @return On success returns the new cvalue_t or NULL otherwise.
 */
cvalue_t *cvalue_from_cstring(const cstring_t *value);

/**
 * @name cvalue_ref
 * @brief Increases the reference count for a cvalue_t item.
 *
 * @param [in,out] value: The cvalue_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cvalue_t *cvalue_ref(cvalue_t *value);

/**
 * @name cvalue_unref
 * @brief Decreases the reference count for a cvalue_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] value: The cvalue_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cvalue_unref(cvalue_t *value);

/**
 * @name cvalue_get
 * @brief Gets the content from a cvalue_t object.
 *
 * @param [in] value: The cvalue_t object.
 * @param [in] fmt: The output format of the object.
 * @param [out] ...: The output value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cvalue_get(const cvalue_t *value, const char *fmt, ...);

/** Macros to get the item value */
#define CVALUE_AS_INT(v)       \
    ({ int __x; cvalue_get(v, CVALUE_INT, &__x, NULL); __x; })

#define CVALUE_AS_UINT(v)       \
    ({ unsigned int __x; cvalue_get(v, CVALUE_UINT, &__x, NULL); __x; })

#define CVALUE_AS_LONG(v)       \
    ({ long __x; cvalue_get(v, CVALUE_LONG, &__x, NULL); __x; })

#define CVALUE_AS_ULONG(v)       \
    ({ unsigned long __x; cvalue_get(v, CVALUE_ULONG, &__x, NULL); __x; })

#define CVALUE_AS_LLONG(v)       \
    ({ long long __x; cvalue_get(v, CVALUE_LLONG, &__x, NULL); __x; })

#define CVALUE_AS_ULLONG(v)       \
    ({ unsigned long long __x; cvalue_get(v, CVALUE_ULLONG, &__x, NULL); __x; })

#define CVALUE_AS_SINT(v)       \
    ({ short int __x; cvalue_get(v, CVALUE_SINT, &__x, NULL); __x; })

#define CVALUE_AS_USINT(v)       \
    ({ unsigned short int __x; cvalue_get(v, CVALUE_USINT, &__x, NULL); __x; })

#define CVALUE_AS_CHAR(v)       \
    ({ char __x; cvalue_get(v, CVALUE_CHAR, &__x, NULL); __x; })

#define CVALUE_AS_UCHAR(v)       \
    ({ unsigned char __x; cvalue_get(v, CVALUE_UCHAR, &__x, NULL); __x; })

#define CVALUE_AS_FLOAT(v)      \
    ({ float __x; cvalue_get(v, CVALUE_FLOAT, &__x, NULL); __x; })

#define CVALUE_AS_DOUBLE(v)     \
    ({ double __x; cvalue_get(v, CVALUE_DOUBLE, &__x, NULL); __x; })

#define CVALUE_AS_BOOLEAN(v)    \
    ({ bool __x; cvalue_get(v, CVALUE_BOOLEAN, &__x, NULL); __x; })

#define CVALUE_AS_STRING(v)     \
    ({ char *__x; cvalue_get(v, CVALUE_STRING, &__x, NULL); __x; })

#define CVALUE_AS_CSTRING(v)    \
    ({ cstring_t *__x; cvalue_get(v, CVALUE_CSTRING, &__x, NULL); __x; })

#define CVALUE_AS_POINTER(v)    \
    ({ int __s; void *__x; cvalue_get(v, CVALUE_POINTER, &__x, &__s, NULL); __x; })

/** Macros to check if an item correspond to a specific type */
#define CVALUE_isvoid(o)        cvalue_is_of_type((o), CL_VOID)
#define CVALUE_ischar(o)        cvalue_is_of_type((o), CL_CHAR)
#define CVALUE_isuchar(o)       cvalue_is_of_type((o), CL_UCHAR)
#define CVALUE_isint(o)         cvalue_is_of_type((o), CL_INT)
#define CVALUE_isuint(o)        cvalue_is_of_type((o), CL_UINT)
#define CVALUE_issint(o)        cvalue_is_of_type((o), CL_SINT)
#define CVALUE_isusint(o)       cvalue_is_of_type((o), CL_USINT)
#define CVALUE_isfloat(o)       cvalue_is_of_type((o), CL_FLOAT)
#define CVALUE_isdouble(o)      cvalue_is_of_type((o), CL_DOUBLE)
#define CVALUE_islong(o)        cvalue_is_of_type((o), CL_LONG)
#define CVALUE_isulong(o)       cvalue_is_of_type((o), CL_ULONG)
#define CVALUE_isllong(o)       cvalue_is_of_type((o), CL_LLONG)
#define CVALUE_isullong(o)      cvalue_is_of_type((o), CL_ULLONG)
#define CVALUE_isstring(o)      cvalue_is_of_type((o), CL_STRING)
#define CVALUE_iscstring(o)     cvalue_is_of_type((o), CL_CSTRING)
#define CVALUE_isboolean(o)     cvalue_is_of_type((o), CL_BOOLEAN)

#endif

