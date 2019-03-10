
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

/*
 *
 * API
 *
 */

int cl_image_size(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;
    int s = -1;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CL_IMAGE_RAW)
        s = i->raw.hdr.size;
    else
        s = i->image->imageSize;

    return s;
}

int cl_image_width(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;
    int w = -1;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CL_IMAGE_RAW)
        w = i->raw.hdr.width;
    else
        w = i->image->width;

    return w;
}

int cl_image_height(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;
    int h = -1;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (i->type == CL_IMAGE_RAW)
        h = i->raw.hdr.height;
    else
        h = i->image->height;

    return h;
}

enum cl_image_type cl_image_type(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->type;
}

enum cl_image_color_format cl_image_color_format(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->format;
}

int cl_image_channels(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return get_channels_by_format(i->format);
}

