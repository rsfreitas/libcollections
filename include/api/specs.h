
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan  1 22:49:06 2016
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

#ifndef _COLLECTIONS_API_SPECS_H
#define _COLLECTIONS_API_SPECS_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <specs.h> directly; include <collections.h> instead."
# endif
#endif

/** A cl_spec_t attribute for read/write access */
enum cl_spec_attrib {
    CL_PRIVATE = (1 << 0),
    CL_READABLE = (1 << 1),
    CL_WRITABLE = (1 << 2)
};

/** Validation format of a cl_spec_t with another value */
enum cl_spec_validation_fmt {
    CL_VALIDATE_IGNORED,
    CL_VALIDATE_RANGE,
    CL_VALIDATE_MIN_LE,
    CL_VALIDATE_MIN_LT,
    CL_VALIDATE_MIN_GE,
    CL_VALIDATE_MIN_GT,
    CL_VALIDATE_MIN_EQ,
    CL_VALIDATE_MIN_NE,
    CL_VALIDATE_MAX_LE,
    CL_VALIDATE_MAX_LT,
    CL_VALIDATE_MAX_GE,
    CL_VALIDATE_MAX_GT,
    CL_VALIDATE_MAX_EQ,
    CL_VALIDATE_MAX_NE
};

/**
 * @name cl_spec_create
 * @brief Creates a cl_spec_t object.
 *
 * The cl_spec_t is an object to add parameters specification to other objects,
 * such as object limits (maximum and minimum), flags indicating if the object
 * may be readable or writable, etc.
 *
 * @param [in] properties: Object properties.
 * @param [in] min: A cl_object_t indicating the minimum value that an object can
 *                  have (this is optional, NULL may be used).
 * @param [in] max: A cl_object_t indicating the maximum value that an object can
 *                  have (this is optional, NULL may be used).
 * @param [in] max_length: An integer value indicating the maximum size that an
 *                         object of string type can have (this is optional).
 *
 * @return On success returns an object of cl_spec_t type or NULL otherwise.
 */
cl_spec_t *cl_spec_create(enum cl_spec_attrib properties, cl_object_t *min,
                          cl_object_t *max, unsigned int max_length);

/**
 * @name cl_spec_destroy
 * @brief Releases a cl_spec_t object from memory.
 *
 * @param [in,out] spec: The cl_spec_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_spec_destroy(cl_spec_t *spec);

/**
 * @name cl_spec_validate_va
 * @brief Performs validations on a cl_object_t using a cl_spec_t content.
 *
 * The validations are executed according to the cl_object_t type and, if the
 * return is true, the cl_object_t internal object is updated.
 *
 * @param [in] spec: The cl_spec_t object parameters.
 * @param [in,out] value: The cl_object_t object.
 * @param [in] set_value: Flag indicating what will be validated, if we can
 *                        read or write the object.
 * @param [in] validation: Validation which will be applied.
 * @param [in] ap: Values that the cl_object_t may have if validations were true.
 *
 * @return On success returns true or false otherwise.
 */
bool cl_spec_validate_va(const cl_spec_t *spec, cl_object_t *value, bool set_value,
                         enum cl_spec_validation_fmt validation, va_list ap);

/**
 * @name cl_spec_validate
 * @brief Performs validations on a cl_object_t using a cl_spec_t content.
 *
 * The validations are executed according to the cl_object_t type and, if the
 * return is true, the cl_object_t internal object is updated.
 *
 * @param [in] spec: The cl_spec_t object parameters.
 * @param [in,out] value: The cl_object_t object.
 * @param [in] set_value: Flag indicating what will be validated, if we can
 *                        read or write the object.
 * @param [in] validation: Validation which will be applied.
 * @param [in] ...: Values that the cl_object_t may have if validations were true.
 *
 * @return On success returns true or false otherwise.
 */
bool cl_spec_validate(const cl_spec_t *spec, cl_object_t *value, bool set_value,
                      enum cl_spec_validation_fmt validation, ...);

/**
 * @name cl_spec_set_min
 * @brief Updates the minimum value of a cl_spec_t object.
 *
 * @param [in,out] spec: The cl_spec_t object.
 * @param [in] min: The new minimum value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_spec_set_min(cl_spec_t *spec, cl_object_t *min);

/**
 * @name cl_spec_set_max
 * @brief Updates the maximum value of a cl_spec_t object.
 *
 * @param [in,out] spec: The cl_spec_t object.
 * @param [in] max: The new maximum value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_spec_set_max(cl_spec_t *spec, cl_object_t *max);

/**
 * @name cl_spec_set_max_length
 * @brief Updates the maximum length value of a cl_spec_t object.
 *
 * This function is valid if we're dealing with a CL_STRING or a CL_CSTRING
 * object.
 *
 * @param [in,out] spec: The cl_spec_t object.
 * @param [in] max_length: The new maximum length.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_spec_set_max_length(cl_spec_t *spec, unsigned int max_length);

/**
 * @name cl_spec_set_accessibility
 * @brief Updates the cl_spec_t object accessbility attribute.
 *
 * @param [in,out] spec: The cl_spec_t object.
 * @param [in] attrib: The new cl_spec_t attribute.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_spec_set_accessibility(cl_spec_t *spec, enum cl_spec_attrib attrib);

#endif

