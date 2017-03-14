
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 14 13:25:42 2017
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

static unsigned char *raw_to_jpg(cimage_s *image, unsigned int *bsize)
{
    unsigned char *data = NULL, *jpg = NULL;
    bool color, free_data = false;

    /*
     * We still don't known how to convert a YUV RAW image to JPG so we
     * need to convert.
     */
    switch (image->raw.hdr.format) {
        case CIMAGE_FMT_YUV422:
        case CIMAGE_FMT_YUV420:
        case CIMAGE_FMT_YUYV:
            free_data = true;
            data = craw_cvt_format((const unsigned char *)image->raw.headless,
                                   image->raw.hdr.format, image->raw.hdr.width,
                                   image->raw.hdr.height, CIMAGE_FMT_RGB, bsize);

            break;

        default:
            data = (unsigned char *)image->raw.headless;
            break;
    }

    if (image->raw.hdr.format == CIMAGE_FMT_GRAY)
        color = false;
    else
        color = true;

    /* directly convert to jpg */
    jpg = RAW_to_jpg_mem(data, image->raw.hdr.width, image->raw.hdr.height,
                         color, 75, bsize);

    if (free_data)
        free(data);

    return jpg;
}

static unsigned char *raw_to_bmp(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO
     * - convert to jpg
     * - convert jpg to bmp
     */

    return NULL;
}

static unsigned char *raw_to_png(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO
     * - convert to jpg
     * - convert jpg to png
     */

    return NULL;
}

static unsigned char *raw_to_jpg2k(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO
     * - convert to jpg
     * - convert jpg to jpg2k
     */

    return NULL;
}

static unsigned char *raw_to_tiff(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO
     * - convert to jpg
     * - convert jpg to tiff
     */

    return NULL;
}

static unsigned char *raw_to_ppm(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO
     * - convert to jpg
     * - convert jpg to ppm
     */

    return NULL;
}

static unsigned char *convert_raw_image(cimage_s *image,
    enum cimage_type type, unsigned int *bsize)
{
    unsigned char *b;

    switch (type) {
        case CIMAGE_JPG:
            b = raw_to_jpg(image, bsize);
            break;

        case CIMAGE_BMP:
            b = raw_to_bmp(image, bsize);
            break;

        case CIMAGE_PNG:
            b = raw_to_png(image, bsize);
            break;

        case CIMAGE_JPG2K:
            b = raw_to_jpg2k(image, bsize);
            break;

        case CIMAGE_TIFF:
            b = raw_to_tiff(image, bsize);
            break;

        case CIMAGE_PPM:
            b = raw_to_ppm(image, bsize);
            break;

        default:
            break;
    }

    return b;
}

/*
 * Convert an image format using OpenCv. We need to receive the @out_extension
 * with the format: .extension, i.e, for a JPG image .jpg, for a PNG .png, etc.
 */
static unsigned char *cv_cvt_image_type(cimage_s *image,
    const char *out_extension, unsigned int *bsize)
{
    IplImage *d = NULL;
    CvMat *mat;
    unsigned char *b = NULL;
    int color = 0;

    if (image->format != CIMAGE_FMT_GRAY)
        color = 1;

    mat = cvEncodeImage(out_extension, image->image, 0);
    d = cvDecodeImage(mat, color);
    b = (unsigned char *)d->imageData;
    *bsize = d->imageSize;
    cvReleaseImageHeader(&d);

    return b;
}

static unsigned char *jpg_to_raw(cimage_s *image, unsigned int *bsize)
{
    int width, height;
    bool color;
    unsigned char *ptr = NULL;

    /* directly convert jpg to raw */
    ptr = jpg_to_RAW_mem((const unsigned char *)image->image->imageData,
                         image->image->imageSize, &width, &height, &color);

    if (ptr != NULL)
        *bsize = width * height * (color == true ? 3 : 1);

    return ptr;
}

static unsigned char *bmp_to_raw(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO:
     * - convert to jpg
     * - convert to raw
     */

    return NULL;
}

static unsigned char *png_to_raw(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO:
     * - convert to jpg
     * - convert to raw
     */

    return NULL;
}

static unsigned char *jpg2k_to_raw(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO:
     * - convert to jpg
     * - convert to raw
     */

    return NULL;
}

static unsigned char *tiff_to_raw(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO:
     * - convert to jpg
     * - convert to raw
     */

    return NULL;
}

static unsigned char *ppm_to_raw(cimage_s *image __attribute__((unused)), unsigned int *bsize __attribute__((unused)))
{
    /*
     * TODO:
     * - convert to jpg
     * - convert to raw
     */

    return NULL;
}

static unsigned char *convert_image_to_raw(cimage_s *image, unsigned int *bsize)
{
    unsigned char *b = NULL;

    switch (image->type) {
        case CIMAGE_JPG:
            b = jpg_to_raw(image, bsize);
            break;

        case CIMAGE_BMP:
            b = bmp_to_raw(image, bsize);
            break;

        case CIMAGE_PNG:
            b = png_to_raw(image, bsize);
            break;

        case CIMAGE_JPG2K:
            b = jpg2k_to_raw(image, bsize);
            break;

        case CIMAGE_TIFF:
            b = tiff_to_raw(image, bsize);
            break;

        case CIMAGE_PPM:
            b = ppm_to_raw(image, bsize);
            break;

        default:
            break;
    }

    return b;
}

unsigned char *convert_image_formats(cimage_s *image, enum cimage_type type,
    enum cimage_format format __attribute__((unused)), unsigned int *bsize)
{
    unsigned char *buffer = NULL;
    char *ext, *cv_ext = NULL;

    /* Don't need to convert the image */
    if (type == image->type) {
        *bsize = image->image->imageSize;
        return (unsigned char *)image->image->imageData;
    }

    /* TODO: use format as destination image format */

    /*
     * If we're holding a RAW image is a special case, and we need to manually
     * convert to the required format.
     */
    if (image->type == CIMAGE_RAW) {
        buffer = convert_raw_image(image, type, bsize);
    } else {
        /*
         * If the desired output is a RAW image and we're holding another type
         * we're going to need to "manually" convert it.
         */
        if (type == CIMAGE_RAW)
            buffer = convert_image_to_raw(image, bsize);
        else {
            /* Otherwise we just use OpenCv */
            ext = cimage_type_to_extension(type);
            asprintf(&cv_ext, ".%s", ext);
            buffer = cv_cvt_image_type(image, cv_ext, bsize);
            free(cv_ext);
            free(ext);
        }
    }

    return buffer;
}

unsigned char *convert_raw_formats(cimage_s *image, enum cimage_format format,
    unsigned int *bsize)
{
    /* With these choices we don't need to convert the image */
    if ((format == image->format) ||
        image->format == CIMAGE_FMT_GRAY)
    {
        *bsize = image->raw.hdr.size;
        return (unsigned char *)image->raw.headless;
    }

    return craw_cvt_format((const unsigned char *)image->raw.headless,
                           image->format, image->raw.hdr.width,
                           image->raw.hdr.height, format, bsize);
}


