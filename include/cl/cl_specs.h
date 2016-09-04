
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

#ifndef _COLLECTIONS_CL_SPECS_H
#define _COLLECTIONS_CL_SPECS_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_specs.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cspec_create
 * @brief Creates a cspec_t object.
 *
 * The cspec_t is an object to add parameters specification to other objects,
 * such as object limits (maximum and minimum), flags indicating if the object
 * may be readable or writable, etc.
 *
 * @param [in] properties: Object properties.
 * @param [in] min: A cobject_t indicating the minimum object that an object can
 *                  have (this is optional, NULL may be used).
 * @param [in] max: A cobject_t indicating the maximum object that an object can
 *                  have (this is optional, NULL may be used).
 * @param [in] max_length: An integer value indicating the maximum size that an
 *                         object of string type can have (this is optional).
 *
 * @return On success returns an object of cspec_t type or NULL otherwise.
 */
cspec_t *cspec_create(enum cl_param_flags properties, cobject_t *min,
                      cobject_t *max, unsigned int max_length);

/**
 * @name cspec_destroy
 * @brief Releases a cspec_t object from memory.
 *
 * @param [in,out] spec: The cspec_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cspec_destroy(cspec_t *spec);

/**
 * @name cspec_validate
 * @brief Performs validations on a cobject_t using a cspec_t content.
 *
 * The validations are executed according to the cobject_t type and, if the
 * return is true, the cobject_t internal object is updated.
 *
 * @param [in] spec: The cspec_t object parameters.
 * @param [in,out] value: The cobject_t object.
 * @param [in] set_value: Flag indicating what will be validated, if we can
 *                        read or write the object.
 * @param [in] ap: Values that the cobject_t may have if validations were true.
 *
 * @return On sucess returns true or false otherwise.
 */
bool cspec_validate(const cspec_t *spec, cobject_t *value, bool set_value,
                    va_list ap);

#endif

