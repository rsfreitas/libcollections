
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 14 13:17:33 2017
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

#include "collections.h"
#include "image.h"

__PUB_API__ int cimage_size(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;
    int s = -1;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CIMAGE_RAW)
        s = i->raw_hdr.size;
    else
        s = i->image->imageSize;

    return s;
}

__PUB_API__ int cimage_width(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;
    int w = -1;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CIMAGE_RAW)
        w = i->raw_hdr.width;
    else
        w = i->image->width;

    return w;
}

__PUB_API__ int cimage_height(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;
    int h = -1;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CIMAGE_RAW)
        h = i->raw_hdr.height;
    else
        h = i->image->height;

    return h;
}

__PUB_API__ enum cimage_type cimage_type(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->type;
}

__PUB_API__ enum cimage_format cimage_format(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->format;
}

__PUB_API__ int cimage_channels(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return get_channels_by_format(i->format);
}

