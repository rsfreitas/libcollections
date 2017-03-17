
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
 * Uses libmagic to return the image type from a image buffer.
 */
enum cimage_type cimage_detect_type(const unsigned char *buffer,
    unsigned int bsize __attribute__((unused)))
{
    enum cimage_type type = CIMAGE_RAW;
    union file_header hdr;

    memcpy(&hdr.data, buffer, sizeof(union file_header));

    if (header_from_BMP(hdr))
        type = CIMAGE_BMP;
    else if (header_from_TIFF(hdr))
        type = CIMAGE_TIFF;
    else if (header_from_PPM(hdr))
        type = CIMAGE_PPM;
    else if (header_from_JPG2K(hdr))
        type = CIMAGE_JPG2K;
    else if (header_from_JPG(hdr))
        type = CIMAGE_JPG;
    else if (header_from_PNG(hdr))
        type = CIMAGE_PNG;

    return type;
}

/*
 * This function tries to detect the image file type from the file itself.
 */
enum cimage_type cimage_detect_type_from_file(const char *filename)
{
    enum cimage_type type = CIMAGE_RAW;
    FILE *f;
    union file_header hdr;

    f = fopen(filename, "r");

    if (NULL == f)
        goto end_block;

    fread(&hdr.data, sizeof(union file_header), 1, f);
    fclose(f);

    if (header_from_BMP(hdr))
        type = CIMAGE_BMP;
    else if (header_from_TIFF(hdr))
        type = CIMAGE_TIFF;
    else if (header_from_PPM(hdr))
        type = CIMAGE_PPM;
    else if (header_from_JPG2K(hdr))
        type = CIMAGE_JPG2K;
    else if (header_from_JPG(hdr))
        type = CIMAGE_JPG;
    else if (header_from_PNG(hdr))
        type = CIMAGE_PNG;

end_block:
    return type;
}

enum PixelFormat cimage_format_to_PixelFormat(enum cimage_color_format fmt)
{
    enum PixelFormat px_fmt = PIX_FMT_GRAY8;

    switch (fmt) {
        case CIMAGE_FMT_BGR:
            px_fmt = PIX_FMT_BGR24;
            break;

        case CIMAGE_FMT_RGB:
            px_fmt = PIX_FMT_RGB24;
            break;

        case CIMAGE_FMT_YUV422:
            px_fmt = PIX_FMT_YUV422P;
            break;

        case CIMAGE_FMT_YUV420:
            px_fmt = PIX_FMT_YUV420P;
            break;

        case CIMAGE_FMT_YUYV:
            px_fmt = PIX_FMT_YUYV422;
            break;

        case CIMAGE_FMT_GRAY:
        default:
            break;
    }

    return px_fmt;
}

bool has_internal_image(cimage_s *image)
{
    if (((image->type == CIMAGE_RAW) && (image->raw.original != NULL)) ||
        ((image->type != CIMAGE_RAW) && (image->image != NULL)))
    {
        return true;
    }

    return false;
}

int get_channels_by_format(enum cimage_color_format format)
{
    int channels = 0;

    switch (format) {
        case CIMAGE_FMT_UNKNOWN:
        case CIMAGE_FMT_GRAY:
            channels = 1;
            break;

        case CIMAGE_FMT_BGR:
        case CIMAGE_FMT_RGB:
            channels = 3;
            break;

        case CIMAGE_FMT_YUV422:
        case CIMAGE_FMT_YUV420:
        case CIMAGE_FMT_YUYV:
            channels = 2;
            break;
    }

    return channels;
}

char *cimage_type_to_extension(enum cimage_type type)
{
    switch (type) {
        case CIMAGE_JPG:
            return strdup(EXT_JPG);

        case CIMAGE_BMP:
            return strdup(EXT_BMP);

        case CIMAGE_PNG:
            return strdup(EXT_PNG);

        case CIMAGE_JPG2K:
            return strdup(EXT_JPG2K);

        case CIMAGE_TIFF:
            return strdup(EXT_TIFF);

        case CIMAGE_PPM:
            return strdup(EXT_PPM);

        case CIMAGE_RAW:
            return strdup(EXT_RAW);

        default:
            break;
    }

    return NULL;
}

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

bool is_supported_image_type(enum cimage_type type)
{
    switch (type) {
        case CIMAGE_RAW:
        case CIMAGE_JPG:
        case CIMAGE_BMP:
        case CIMAGE_PNG:
        case CIMAGE_JPG2K:
        case CIMAGE_TIFF:
        case CIMAGE_PPM:
            return true;

        default:
            break;
    }

    return false;
}

bool is_supported_color_format(enum cimage_color_format fmt)
{
    switch (fmt) {
        case CIMAGE_FMT_GRAY:
        case CIMAGE_FMT_BGR:
        case CIMAGE_FMT_RGB:
        case CIMAGE_FMT_YUV422:
        case CIMAGE_FMT_YUV420:
        case CIMAGE_FMT_YUYV:
            return true;

        default:
            break;
    }

    return false;
}

bool is_supported_fill_format(enum cimage_fill_format fmt)
{
    switch (fmt) {
        case CIMAGE_FILL_REFERENCE:
        case CIMAGE_FILL_OWNER:
        case CIMAGE_FILL_COPY:
            return true;

        default:
            break;
    }

    return false;
}

