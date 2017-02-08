
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Feb  8 10:44:54 2017
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

#ifndef _COLLECTIONS_CL_CAPTION_H
#define _COLLECTIONS_CL_CAPTION_H          1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_caption.h> directly; include <collections.h> instead."
# endif
#endif

enum caption_color {
    CAPTION_BLACK,
    CAPTION_WHITE,
    CAPTION_RED,
    CAPTION_GREEN,
    CAPTION_BLUE,
    CAPTION_YELLOW,
    CAPTION_GREY
};

cimage_caption_t *caption_ref(cimage_caption_t *caption);
int caption_unref(cimage_caption_t *caption);
cimage_caption_t *caption_configure(const char *ttf_pathname,
    unsigned int font_size);
int caption_destroy(cimage_caption_t *caption);
int cimage_add_caption_vf(cimage_caption_t *caption,
    cimage_t *image, unsigned int x, unsigned int y, enum caption_color color,
    const char *fmt, va_list ap);
int cimage_add_caption_f(cimage_caption_t *caption, cimage_t *image,
    unsigned int x, unsigned int y, enum caption_color color,
    const char *fmt, ...);
int cimage_add_caption(cimage_caption_t *caption, cimage_t *image,
    unsigned int x, unsigned int y, enum caption_color color, const char *text);

#endif

