
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 14 11:20:37 2017
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

union file_header {
    char data[10];
    struct header {
        char b1;
        char b2;
        char b3;
        char b4;
        char b5;
        char b6;
        char b7;
        char b8;
        char b9;
        char b10;
    } header;
};

/*
 *
 * Internal functions
 *
 */

static bool header_from_BMP(union file_header hdr)
{
    bool valid = false;

    if (((unsigned char)hdr.header.b1 == 0x42) &&
        ((unsigned char)hdr.header.b2 == 0x40))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_TIFF(union file_header hdr)
{
    bool valid = false;

    if ((((unsigned char)hdr.header.b1 == 0x49) &&
         ((unsigned char)hdr.header.b2 == 0x20) &&
         ((unsigned char)hdr.header.b3 == 0x49)) ||
        (((unsigned char)hdr.header.b1 == 0x49) &&
         ((unsigned char)hdr.header.b2 == 0x49) &&
         ((unsigned char)hdr.header.b3 == 0x2A) &&
         ((unsigned char)hdr.header.b4 == 0x00)) ||
        (((unsigned char)hdr.header.b1 == 0x4D) &&
         ((unsigned char)hdr.header.b2 == 0x4D) &&
         ((unsigned char)hdr.header.b3 == 0x00) &&
         ((unsigned char)hdr.header.b4 == 0x2A)) ||
        (((unsigned char)hdr.header.b1 == 0x4D) &&
         ((unsigned char)hdr.header.b2 == 0x4D) &&
         ((unsigned char)hdr.header.b3 == 0x00) &&
         ((unsigned char)hdr.header.b4 == 0x2B)))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_PPM(union file_header hdr)
{
    bool valid = false;

    if (((unsigned char)hdr.header.b1 == 'P') &&
        (((unsigned char)hdr.header.b2 == '1') ||
         ((unsigned char)hdr.header.b2 == '2') ||
         ((unsigned char)hdr.header.b2 == '3')))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_JPG2K(union file_header hdr)
{
    bool valid = false;

    if (((unsigned char)hdr.header.b1 == 0x00) &&
        ((unsigned char)hdr.header.b2 == 0x00) &&
        ((unsigned char)hdr.header.b3 == 0x00) &&
        ((unsigned char)hdr.header.b4 == 0x0C) &&
        ((unsigned char)hdr.header.b5 == 0x6A) &&
        ((unsigned char)hdr.header.b6 == 0x50) &&
        ((unsigned char)hdr.header.b7 == 0x20) &&
        ((unsigned char)hdr.header.b8 == 0x20) &&
        ((unsigned char)hdr.header.b9 == 0x0D) &&
        ((unsigned char)hdr.header.b10 == 0x0A))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_PNG(union file_header hdr)
{
    bool valid = false;

    if (((unsigned char)hdr.header.b1 == 0x89) &&
        ((unsigned char)hdr.header.b2 == 0x50) &&
        ((unsigned char)hdr.header.b3 == 0x4E) &&
        ((unsigned char)hdr.header.b4 == 0x47) &&
        ((unsigned char)hdr.header.b5 == 0x0D) &&
        ((unsigned char)hdr.header.b6 == 0x0A) &&
        ((unsigned char)hdr.header.b7 == 0x1A) &&
        ((unsigned char)hdr.header.b8 == 0x0A))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_JPG(union file_header hdr)
{
    bool valid = false;

    if ((((unsigned char)hdr.header.b1 == 0xFF) &&
         ((unsigned char)hdr.header.b2 == 0xD8) &&
         ((unsigned char)hdr.header.b3 == 0xFF) &&
         ((unsigned char)hdr.header.b4 == 0xE0)) ||
        (((unsigned char)hdr.header.b1 == 0xFF) &&
         ((unsigned char)hdr.header.b2 == 0xD8) &&
         ((unsigned char)hdr.header.b3 == 0xFF) &&
         ((unsigned char)hdr.header.b4 == 0xE1)) ||
        (((unsigned char)hdr.header.b1 == 0xFF) &&
         ((unsigned char)hdr.header.b2 == 0xD8) &&
         ((unsigned char)hdr.header.b3 == 0xFF) &&
         ((unsigned char)hdr.header.b4 == 0xE8)))
    {
        valid = true;
    }

    return valid;
}

/*
 *
 * Internal API
 *
 */

/*
 * Uses libmagic to return the image type from a image buffer.
 */
CL_INTERNAL_API
enum cl_image_type cl_image_detect_type(const unsigned char *buffer,
    unsigned int bsize __attribute__((unused)))
{
    enum cl_image_type type = CL_IMAGE_RAW;
    union file_header hdr;

    memcpy(&hdr.data, buffer, sizeof(union file_header));

    if (header_from_BMP(hdr))
        type = CL_IMAGE_BMP;
    else if (header_from_TIFF(hdr))
        type = CL_IMAGE_TIFF;
    else if (header_from_PPM(hdr))
        type = CL_IMAGE_PPM;
    else if (header_from_JPG2K(hdr))
        type = CL_IMAGE_JPG2K;
    else if (header_from_JPG(hdr))
        type = CL_IMAGE_JPG;
    else if (header_from_PNG(hdr))
        type = CL_IMAGE_PNG;

    return type;
}

/*
 * This function tries to detect the image file type from the file itself.
 */
CL_INTERNAL_API
enum cl_image_type cl_image_detect_type_from_file(const char *filename)
{
    enum cl_image_type type = CL_IMAGE_RAW;
    FILE *f;
    union file_header hdr;

    f = fopen(filename, "r");

    if (NULL == f)
        goto end_block;

    fread(&hdr.data, sizeof(union file_header), 1, f);
    fclose(f);

    if (header_from_BMP(hdr))
        type = CL_IMAGE_BMP;
    else if (header_from_TIFF(hdr))
        type = CL_IMAGE_TIFF;
    else if (header_from_PPM(hdr))
        type = CL_IMAGE_PPM;
    else if (header_from_JPG2K(hdr))
        type = CL_IMAGE_JPG2K;
    else if (header_from_JPG(hdr))
        type = CL_IMAGE_JPG;
    else if (header_from_PNG(hdr))
        type = CL_IMAGE_PNG;

end_block:
    return type;
}

CL_INTERNAL_API
enum PixelFormat cl_image_format_to_PixelFormat(enum cl_image_color_format fmt)
{
    enum PixelFormat px_fmt = PIX_FMT_GRAY8;

    switch (fmt) {
    case CL_IMAGE_FMT_BGR:
        px_fmt = PIX_FMT_BGR24;
        break;

    case CL_IMAGE_FMT_RGB:
        px_fmt = PIX_FMT_RGB24;
        break;

    case CL_IMAGE_FMT_YUV422:
        px_fmt = PIX_FMT_YUV422P;
        break;

    case CL_IMAGE_FMT_YUV420:
        px_fmt = PIX_FMT_YUV420P;
        break;

    case CL_IMAGE_FMT_YUYV:
        px_fmt = PIX_FMT_YUYV422;
        break;

    case CL_IMAGE_FMT_GRAY:
    default:
        break;
    }

    return px_fmt;
}

CL_INTERNAL_API
bool has_internal_image(cl_image_s *image)
{
    if (((image->type == CL_IMAGE_RAW) && (image->raw.original != NULL)) ||
        ((image->type != CL_IMAGE_RAW) && (image->image != NULL)))
    {
        return true;
    }

    return false;
}

CL_INTERNAL_API
int get_channels_by_format(enum cl_image_color_format format)
{
    int channels = 0;

    switch (format) {
    case CL_IMAGE_FMT_UNKNOWN:
    case CL_IMAGE_FMT_GRAY:
        channels = 1;
        break;

    case CL_IMAGE_FMT_BGR:
    case CL_IMAGE_FMT_RGB:
        channels = 3;
        break;

    case CL_IMAGE_FMT_YUV422:
    case CL_IMAGE_FMT_YUV420:
    case CL_IMAGE_FMT_YUYV:
        channels = 2;
        break;
    }

    return channels;
}

CL_INTERNAL_API
char *cl_image_type_to_extension(enum cl_image_type type)
{
    switch (type) {
    case CL_IMAGE_JPG:
        return strdup(EXT_JPG);

    case CL_IMAGE_BMP:
        return strdup(EXT_BMP);

    case CL_IMAGE_PNG:
        return strdup(EXT_PNG);

    case CL_IMAGE_JPG2K:
        return strdup(EXT_JPG2K);

    case CL_IMAGE_TIFF:
        return strdup(EXT_TIFF);

    case CL_IMAGE_PPM:
        return strdup(EXT_PPM);

    case CL_IMAGE_RAW:
        return strdup(EXT_RAW);

    default:
        break;
    }

    return NULL;
}

CL_INTERNAL_API
bool is_known_extension(const char *filename)
{
    char *ext = NULL;
    bool known = false;

    ext = file_extension(filename);

    if (NULL == ext)
        return false;

    if ((strcmp(ext, EXT_JPG) == 0) ||
        (strcmp(ext, EXT_BMP) == 0) ||
        (strcmp(ext, EXT_PNG) == 0) ||
        (strcmp(ext, EXT_JPG2K) == 0) ||
        (strcmp(ext, EXT_TIFF) == 0) ||
        (strcmp(ext, EXT_PPM) == 0) ||
        (strcmp(ext, EXT_RAW) == 0))
    {
        known = true;
    }

    if (ext != NULL)
        free(ext);

    return known;
}

CL_INTERNAL_API
bool is_supported_image_type(enum cl_image_type type)
{
    switch (type) {
    case CL_IMAGE_RAW:
    case CL_IMAGE_JPG:
    case CL_IMAGE_BMP:
    case CL_IMAGE_PNG:
    case CL_IMAGE_JPG2K:
    case CL_IMAGE_TIFF:
    case CL_IMAGE_PPM:
        return true;

    default:
        break;
    }

    return false;
}

CL_INTERNAL_API
bool is_supported_color_format(enum cl_image_color_format fmt)
{
    switch (fmt) {
    case CL_IMAGE_FMT_GRAY:
    case CL_IMAGE_FMT_BGR:
    case CL_IMAGE_FMT_RGB:
    case CL_IMAGE_FMT_YUV422:
    case CL_IMAGE_FMT_YUV420:
    case CL_IMAGE_FMT_YUYV:
        return true;

    default:
        break;
    }

    return false;
}

CL_INTERNAL_API
bool is_supported_fill_format(enum cl_image_fill_format fmt)
{
    switch (fmt) {
    case CL_IMAGE_FILL_REFERENCE:
    case CL_IMAGE_FILL_OWNER:
    case CL_IMAGE_FILL_COPY:
        return true;

    default:
        break;
    }

    return false;
}

