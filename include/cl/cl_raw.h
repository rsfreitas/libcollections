
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Feb 19 17:39:06 2017
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

#ifndef _COLLECTIONS_CL_RAW_H
#define _COLLECTIONS_CL_RAW_H          1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_raw.h> directly; include <collections.h> instead."
# endif
#endif

unsigned char *RAW_to_jpg_mem(const unsigned char *buffer, int width,
                              int height, bool color, int quality,
                              unsigned int *jsize);

unsigned char *jpg_to_RAW_mem(const unsigned char *buffer, unsigned int jsize,
                              int *width, int *height, bool *color);

#endif

