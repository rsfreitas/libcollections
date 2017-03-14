
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 14 11:05:27 2017
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

#ifndef _COLLECTIONS_IMAGE_H
#define _COLLECTIONS_IMAGE_H        1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <image.h> directly; include <collections.h> instead."
# endif
#endif

/* A RAW image file header */
struct raw_header {
    unsigned int        id;
    unsigned int        size;
    unsigned int        width;
    unsigned int        height;
    unsigned int        format;
};

#define cimage_members                                      \
    cl_struct_member(IplImage *, image)                     \
    cl_struct_member(unsigned char *, raw_original_ptr)     \
    cl_struct_member(unsigned char *, headless_raw)         \
    cl_struct_member(enum cimage_format, format)            \
    cl_struct_member(enum cimage_type, type)                \
    cl_struct_member(enum cimage_fill_format, fill_format)  \
    cl_struct_member(struct raw_header, raw_hdr)            \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(cimage_s, cimage_members);

#define cimage_s                    cl_struct(cimage_s)
#define RAW_ID                      cl_fourcc('R', 'A', 'W', '1')

/* raw.c */
unsigned char *RAW_to_jpg_mem(const unsigned char *buffer, int width,
                              int height, bool color, int quality,
                              unsigned int *jsize);

unsigned char *jpg_to_RAW_mem(const unsigned char *buffer, unsigned int jsize,
                              int *width, int *height, bool *color);

/* utils.c */
enum cimage_type cimage_detect_type(const unsigned char *buffer,
                                    unsigned int bsize);

enum cimage_type cimage_detect_type_from_file(const char *filename);

#endif

