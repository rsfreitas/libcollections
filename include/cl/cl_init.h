
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Sep  7 10:13:24 2016
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

#ifndef _COLLECTIONS_CL_INIT_H
#define _COLLECTIONS_CL_INIT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_init.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name collections_init
 * @brief The function to initialize all library internals.
 *
 * This function must be the library first function called inside a code.
 * Otherwise all other functions from it won't work.
 */
void collections_init(void);

/**
 * @name collections_uninit
 * @brief The function to end all library internals.
 */
void collections_uninit(void);

#endif

