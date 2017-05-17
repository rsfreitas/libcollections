
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
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

#ifndef _COLLECTIONS_CL_STRINGLIST_H
#define _COLLECTIONS_CL_STRINGLIST_H	1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_stringlist.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_string_list_create
 * @brief Creates a new cl_string_list_t object.
 *
 * @return On success returns the cl_string_list_t object or NULL otherwise.
 */
cl_string_list_t *cl_string_list_create(void);

/**
 * @name cl_string_list_destroy
 * @brief Releases memory from a cl_string_list_t object.
 *
 * @param [in] l: The cl_string_list_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_list_destroy(cl_string_list_t *l);

/**
 * @name cl_string_list_size
 * @brief Gets the size of a cl_string_list_t object.
 *
 * @param [in] l: The cl_string_list_t object.
 *
 * @return On success returns the size of \a l or -1 otherwise.
 */
int cl_string_list_size(const cl_string_list_t *l);

/**
 * @name cl_string_list_add
 * @brief Add an item of cl_string_t type into a cl_string_list_t object.
 *
 * @param [in,out] l: The cl_string_list_t object.
 * @param [in] s: The cl_string_t which will be inserted on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_list_add(cl_string_list_t *l, cl_string_t *s);

/**
 * @name cl_string_list_get
 * @brief Gets a pointer to a specific item inside a cl_string_list_t object.
 *
 * On a successful call the returned string reference must be 'unreferenced'.
 *
 * @param [in] l: The cl_string_list_t object.
 * @param [in] index: The index of the item.
 *
 * @return On success returns a reference to an item or NULL otherwise.
 */
cl_string_t *cl_string_list_get(const cl_string_list_t *l, unsigned int index);

/**
 * @name cl_string_list_flat
 * @brief Gives a cl_string_t object with all strings from a stringlist.
 *
 * @param [in] l: The cl_string_list_t object.
 * @param [in] delimiter: A delimiter character to put between every string
 *                        list entry.
 *
 * @return On success returns a cl_string_t object with the stringlist content
 *         or NULL otherwise.
 */
cl_string_t *cl_string_list_flat(const cl_string_list_t *l,
                                 const char delimiter);

#endif

