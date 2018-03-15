
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Feb  5 15:26:58 2017
 * Project: libcollections
 *
 * Copyright (C) 2017 Rodrigo Freitas
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

#ifndef _COLLECTIONS_API_REF_H
#define _COLLECTIONS_API_REF_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <ref.h> directly; include <collections.h> instead."
# endif
#endif

#include <stddef.h>

/** A macro to get the address of a structure through its member. */
#define cl_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/** A reference count structure */
struct cl_ref_s {
    void    (*free)(const struct cl_ref_s *);
    int     count;
};

/**
 * @name cl_ref_inc
 * @brief Increments a reference count by one.
 *
 * @param [in,out] ref: A reference count structure to be increased.
 */
inline void cl_ref_inc(const struct cl_ref_s *ref);

/**
 * @name cl_ref_dec
 * @brief Decrements from a reference count.
 *
 * When this reference count drops to 0, the ref->free function is called.
 *
 * @param [in,out] ref: A reference count structure to be decreased.
 */
inline void cl_ref_dec(const struct cl_ref_s *ref);

/**
 * @name cl_ref_bool_compare
 * @brief Compares the current reference and swap its value.
 *
 * If the current reference value of ref->count is \a old, then write \a new_
 * to it.
 *
 * @param [in,out] ref: A reference count structure to be compared.
 * @param [in] old: The old value from the reference count.
 * @param [in] new_: The new value of the reference count.
 *
 * @return On success, i.e, the comparison is successful and \a new was written,
 *         returns true, otherwise returns false.
 */
inline bool cl_ref_bool_compare(const struct cl_ref_s *ref, int old, int new_);

#endif

