
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 24 15:44:16 2015
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

#ifndef _COLLECTIONS_CL_MEM_H
#define _COLLECTIONS_CL_MEM_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_mem.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cmemdup
 * @brief Duplicates a specific buffer.
 *
 * @param [in] src: Buffer that will be duplicated.
 * @param [in] len: Size of buffer content.
 *
 * @return On success returns the new buffer or NULL otherwise.
 */
void *cmemdup(const void *src, unsigned int len);

#endif

