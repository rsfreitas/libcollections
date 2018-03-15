
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

#ifndef _COLLECTIONS_API_STRINGLIST_H
#define _COLLECTIONS_API_STRINGLIST_H	1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <stringlist.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_stringlist_create
 * @brief Creates a new cl_stringlist_t object.
 *
 * @return On success returns the cl_stringlist_t object or NULL otherwise.
 */
cl_stringlist_t *cl_stringlist_create(void);

/**
 * @name cl_stringlist_destroy
 * @brief Releases memory from a cl_stringlist_t object.
 *
 * @param [in] l: The cl_stringlist_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stringlist_destroy(cl_stringlist_t *l);

/**
 * @name cl_stringlist_size
 * @brief Gets the size of a cl_stringlist_t object.
 *
 * @param [in] l: The cl_stringlist_t object.
 *
 * @return On success returns the size of \a l or -1 otherwise.
 */
int cl_stringlist_size(const cl_stringlist_t *l);

/**
 * @name cl_stringlist_add
 * @brief Add an item of cl_string_t type into a cl_stringlist_t object.
 *
 * @param [in,out] l: The cl_stringlist_t object.
 * @param [in] s: The cl_string_t which will be inserted on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stringlist_add(cl_stringlist_t *l, cl_string_t *s);

/**
 * @name cl_stringlist_get
 * @brief Gets a pointer to a specific item inside a cl_stringlist_t object.
 *
 * On a successful call the returned string reference must be 'unreferenced'.
 *
 * @param [in] l: The cl_stringlist_t object.
 * @param [in] index: The index of the item.
 *
 * @return On success returns a reference to an item or NULL otherwise.
 */
cl_string_t *cl_stringlist_get(const cl_stringlist_t *l, unsigned int index);

/**
 * @name cl_stringlist_flat
 * @brief Gives a cl_string_t object with all strings from a stringlist.
 *
 * @param [in] l: The cl_stringlist_t object.
 * @param [in] delimiter: A delimiter character to put between every string
 *                        list entry.
 *
 * @return On success returns a cl_string_t object with the stringlist content
 *         or NULL otherwise.
 */
cl_string_t *cl_stringlist_flat(const cl_stringlist_t *l, const char delimiter);

/**
 * @name cl_stringlist_dup
 * @brief Duplicates a cl_stringlist_t object.
 *
 * @param [in] list: The cl_stringlist_t object.
 *
 * @return On success returns a new cl_stringlist_t object or NULL otherwise.
 */
cl_stringlist_t *cl_stringlist_dup(const cl_stringlist_t *list);

/**
 * @name cl_string_contains
 * @brief Searches for a string inside a string list.
 *
 * @param [in] list: The cl_stringlist_t object.
 * @param [in] needle: The wanted.
 *
 * @param Returns true if \a needle is found inside the list or false
 *        otherwise.
 */
bool cl_stringlist_contains(const cl_stringlist_t *list,
                            const cl_string_t *needle);

#endif

