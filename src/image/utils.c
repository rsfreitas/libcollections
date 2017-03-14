
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
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
    } header;
};

static bool header_from_BMP(union file_header hdr)
{
    bool valid = false;

    if ((hdr.header.b1 == 0x42) && (hdr.header.b2 == 0x40))
        valid = true;

    return valid;
}

static bool header_from_TIFF(union file_header hdr)
{
    bool valid = false;

    if (((hdr.header.b1 == 0x49) && (hdr.header.b2 == 0x20) &&
         (hdr.header.b3 == 0x49)) ||
        ((hdr.header.b1 == 0x49) && (hdr.header.b2 == 0x49) &&
         (hdr.header.b3 == 0x2A) && (hdr.header.b4 == 0x00)) ||
        ((hdr.header.b1 == 0x4D) && (hdr.header.b2 == 0x4D) &&
         (hdr.header.b3 == 0x00) && (hdr.header.b4 == 0x2A)) ||
        ((hdr.header.b1 == 0x4D) && (hdr.header.b2 == 0x4D) &&
         (hdr.header.b3 == 0x00) && (hdr.header.b4 == 0x2B)))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_PPM(union file_header hdr)
{
    bool valid = false;

    if ((hdr.header.b1 == 'P') &&
        ((hdr.header.b2 == '1') ||
         (hdr.header.b2 == '2') ||
         (hdr.header.b2 == '3')))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_JPG2K(union file_header hdr)
{
    bool valid = false;

    if ((hdr.header.b1 == 0x00) && (hdr.header.b2 == 0x00) &&
        (hdr.header.b3 == 0x00) && (hdr.header.b4 == 0x0C) &&
        (hdr.header.b5 == 0x6A) && (hdr.header.b6 == 0x50) &&
        (hdr.header.b7 == 0x20) && (hdr.header.b8 == 0x20) &&
        (hdr.header.b9 == 0x0D) && (hdr.header.b10 == 0x0A))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_PNG(union file_header hdr)
{
    bool valid = false;

    if ((hdr.header.b1 == 0x89) && (hdr.header.b2 == 0x50) &&
        (hdr.header.b3 == 0x4E) && (hdr.header.b4 == 0x47) &&
        (hdr.header.b5 == 0x0D) && (hdr.header.b6 == 0x0A) &&
        (hdr.header.b7 == 0x1A) && (hdr.header.b8 == 0x0A))
    {
        valid = true;
    }

    return valid;
}

static bool header_from_JPG(union file_header hdr)
{
    bool valid = false;

    if (((hdr.header.b1 == 0xFF) && (hdr.header.b2 == 0xD8) &&
         (hdr.header.b3 == 0xFF) && (hdr.header.b4 == 0xE0)) ||
        ((hdr.header.b1 == 0xFF) && (hdr.header.b2 == 0xD8) &&
         (hdr.header.b3 == 0xFF) && (hdr.header.b4 == 0xE1)) ||
        ((hdr.header.b1 == 0xFF) && (hdr.header.b2 == 0xD8) &&
         (hdr.header.b3 == 0xFF) && (hdr.header.b4 == 0xE8)))
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

