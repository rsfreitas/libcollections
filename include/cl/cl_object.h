
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
 * @name cl_object_create
 * @brief Creates a new cl_object_t object.
 *
 * If object is of CL_OBJECT_VOIDP type the variadic arguments should be
 * the binary content and its size. The variadic arguments should end with
 * NULL.
 *
 * @param [in] type: The object type of the new object.
 * @param [in] ...: The item object.
 *
 * @return On success returns a cl_object_t object or NULL othewise.
 */
cl_object_t *cl_object_create(enum cl_type type, ...);

/**
 * @name cl_object_create_empty
 * @brief Creates a new empty cl_object_t object.
 *
 * @param [in] type: The object type of the new object.
 *
 * @return On success returns a cl_object_t object or NULL othewise.
 */
cl_object_t *cl_object_create_empty(enum cl_type type);

/**
 * @name cl_object_create_with_spec
 * @brief Creates a new cl_object_t with parameters specification support.
 *
 * @param [in] type: The object type of the new object.
 * @param [in] spec: The parameters specification.
 *
 * @return On success returns a cl_object_t object or NULL othewise.
 */
cl_object_t *cl_object_create_with_spec(enum cl_type type, cl_spec_t *spec);

/**
 * @name cl_object_destroy
 * @brief Releases a cl_object_t from memory.
 *
 * @param [in,out] object: The cl_object_t which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_destroy(cl_object_t *object);

/**
 * @name cl_object_sizeof
 * @brief Gets the sizeof of a cl_object_t.
 *
 * @param [in] object: The cl_object_t object.
 *
 * @return On success returns the sizeof of the cl_object_t or -1 otherwise.
 */
int cl_object_sizeof(const cl_object_t *object);

/**
 * @name cl_object_type
 * @brief Gets the type of a cl_object_t.
 *
 * @param [in] object: The cl_object_t object.
 *
 * @return On success returns the cl_object_t object type or -1 otherwise.
 */
enum cl_type cl_object_type(const cl_object_t *object);

/**
 * @name cl_object_set
 * @brief Changes the content of a cl_object_t.
 *
 * @param [in] object: The cl_object_t object.
 * @param [in] ...: The new content of the object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_set(cl_object_t *object, ...);

/**
 * @name cl_object_get
 * @brief Gets the content from a cl_object_t object.
 *
 * @param [in] object: The cl_object_t object.
 * @param [in] fmt: The output format of the object.
 * @param [out] ...: The output object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_get(const cl_object_t *object, const char *fmt, ...);

/**
 * @name cl_object_is_of_type
 * @brief Checks if a cl_object_t is from a specific type.
 *
 * @param [in] object: The cl_object_t.
 * @param [in] type: The type to check.
 *
 * @return Returns true if \a object is of \a type type or false
 *         otherwise.
 */
bool cl_object_is_of_type(const cl_object_t *object, unsigned int type);

/**
 * @name cl_object_to_cstring
 * @brief Converts a cl_object_t object into a human readable string (cl_string_t).
 *
 * @param [in] object: The cl_object_t object.
 *
 * @return On success returns a cl_string_t containing the cl_object_t or NULL
 *         otherwise.
 */
cl_string_t *cl_object_to_cstring(const cl_object_t *object);

/**
 * @name cl_object_from_cstring
 * @brief Creates a cl_object_t from a cl_string_t object.
 *
 * @param [in] object: The cl_string_t object.
 *
 * @return On success returns the new cl_object_t or NULL otherwise.
 */
cl_object_t *cl_object_from_cstring(const cl_string_t *object);

/**
 * @name cl_object_ref
 * @brief Increases the reference count for a cl_object_t item.
 *
 * @param [in,out] object: The cl_object_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_object_t *cl_object_ref(cl_object_t *object);

/**
 * @name cl_object_unref
 * @brief Decreases the reference count for a cl_object_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] object: The cl_object_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_unref(cl_object_t *object);

/**
 * @name cl_object_set_equals
 * @brief Sets the object equals function.
 *
 * An object equals function is a function to indicate whether some other
 * object is equal to another.
 *
 * It must return true if the objects are equal or false otherwise.
 *
 * @param [in,out] object: The cl_object_t item.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_set_equals(cl_object_t *object, bool (*equals)(cl_object_t *));

/**
 * @name cl_object_equals
 * @brief A function to indicate whether an object is equal to another.
 *
 * @param [in] ob1: The cl_object_t object.
 * @param [in] ob2: The object which will be compared.
 *
 * @return Returns true is \a ob1 is the same as the \a ob2 or false
 *         otherwise.
 */
bool cl_object_equals(const cl_object_t *ob1, const cl_object_t *ob2);

/**
 * @name cl_object_set_compare_to
 * @brief Sets the object compare_to function.
 *
 * An object compare_to function is a function to compare the object with
 * another one for order.
 *
 * It must return negative, zero or a positive integer if the object is less
 * than, equal to, or greater than the specified object.
 *
 * @param [in,out] object: The cl_object_t item.
 * @param [in] compare_to: The compare_to function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_object_set_compare_to(cl_object_t *object,
                             int (*compare_to)(cl_object_t *));

/**
 * @name cl_object_compare_to
 * @brief A function to compare an object to another to order them.
 *
 * @param [in] ob1: The cl_object_t object.
 * @param [in] ob2: The object which will be compared.
 *
 * @return Returns a negative, zero or a positive integer if the object \a ob1
 *         is less than, equal to, or greater than \a ob2.
 */
int cl_object_compare_to(const cl_object_t *ob1, const cl_object_t *ob2);

/**
 * @name cl_object_dup
 * @brief Duplicates a cl_object_t object.
 *
 * @param [in] object: The cl_object_t object.
 *
 * @return On success returns a new cl_object_t object or NULL otherwise.
 */
cl_object_t *cl_object_dup(const cl_object_t *object);

/**
 * Macros to get the cl_object_t item object. The STRING and CSTRING objects must
 * be released by the user, using 'free' or 'cl_string_unref' calls.
 */
#define CL_OBJECT_AS_INT(v)       \
    ({ int __x; cl_object_get(v, CL_OBJECT_INT, &__x, NULL); __x; })

#define CL_OBJECT_AS_UINT(v)       \
    ({ unsigned int __x; cl_object_get(v, CL_OBJECT_UINT, &__x, NULL); __x; })

#define CL_OBJECT_AS_LONG(v)       \
    ({ long __x; cl_object_get(v, CL_OBJECT_LONG, &__x, NULL); __x; })

#define CL_OBJECT_AS_ULONG(v)       \
    ({ unsigned long __x; cl_object_get(v, CL_OBJECT_ULONG, &__x, NULL); __x; })

#define CL_OBJECT_AS_LLONG(v)       \
    ({ long long __x; cl_object_get(v, CL_OBJECT_LLONG, &__x, NULL); __x; })

#define CL_OBJECT_AS_ULLONG(v)       \
    ({ unsigned long long __x; cl_object_get(v, CL_OBJECT_ULLONG, &__x, NULL); __x; })

#define CL_OBJECT_AS_SINT(v)       \
    ({ short int __x; cl_object_get(v, CL_OBJECT_SINT, &__x, NULL); __x; })

#define CL_OBJECT_AS_USINT(v)       \
    ({ unsigned short int __x; cl_object_get(v, CL_OBJECT_USINT, &__x, NULL); __x; })

#define CL_OBJECT_AS_CHAR(v)       \
    ({ char __x; cl_object_get(v, CL_OBJECT_CHAR, &__x, NULL); __x; })

#define CL_OBJECT_AS_UCHAR(v)       \
    ({ unsigned char __x; cl_object_get(v, CL_OBJECT_UCHAR, &__x, NULL); __x; })

#define CL_OBJECT_AS_FLOAT(v)      \
    ({ float __x; cl_object_get(v, CL_OBJECT_FLOAT, &__x, NULL); __x; })

#define CL_OBJECT_AS_DOUBLE(v)     \
    ({ double __x; cl_object_get(v, CL_OBJECT_DOUBLE, &__x, NULL); __x; })

#define CL_OBJECT_AS_BOOLEAN(v)    \
    ({ bool __x; cl_object_get(v, CL_OBJECT_BOOLEAN, &__x, NULL); __x; })

#define CL_OBJECT_AS_STRING(v)     \
    ({ char *__x; cl_object_get(v, CL_OBJECT_STRING, &__x, NULL); __x; })

#define CL_OBJECT_AS_CSTRING(v)    \
    ({ cl_string_t *__x; cl_object_get(v, CL_OBJECT_CSTRING, &__x, NULL); __x; })

#define CL_OBJECT_AS_POINTER(v)    \
    ({ int __s; void *__x; cl_object_get(v, CL_OBJECT_POINTER, &__x, &__s, NULL); __x; })

/** Macros to check if an item correspond to a specific type */
#define CL_OBJECT_isvoid(o)         cl_object_is_of_type((o), CL_VOID)
#define CL_OBJECT_ischar(o)         cl_object_is_of_type((o), CL_CHAR)
#define CL_OBJECT_isuchar(o)        cl_object_is_of_type((o), CL_UCHAR)
#define CL_OBJECT_isint(o)          cl_object_is_of_type((o), CL_INT)
#define CL_OBJECT_isuint(o)         cl_object_is_of_type((o), CL_UINT)
#define CL_OBJECT_issint(o)         cl_object_is_of_type((o), CL_SINT)
#define CL_OBJECT_isusint(o)        cl_object_is_of_type((o), CL_USINT)
#define CL_OBJECT_isfloat(o)        cl_object_is_of_type((o), CL_FLOAT)
#define CL_OBJECT_isdouble(o)       cl_object_is_of_type((o), CL_DOUBLE)
#define CL_OBJECT_islong(o)         cl_object_is_of_type((o), CL_LONG)
#define CL_OBJECT_isulong(o)        cl_object_is_of_type((o), CL_ULONG)
#define CL_OBJECT_isllong(o)        cl_object_is_of_type((o), CL_LLONG)
#define CL_OBJECT_isullong(o)       cl_object_is_of_type((o), CL_ULLONG)
#define CL_OBJECT_isstring(o)       cl_object_is_of_type((o), CL_STRING)
#define CL_OBJECT_iscstring(o)      cl_object_is_of_type((o), CL_CSTRING)
#define CL_OBJECT_isboolean(o)      cl_object_is_of_type((o), CL_BOOLEAN)

#endif

