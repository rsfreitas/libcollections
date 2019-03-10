
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

/*
 *
 * Internal functions
 *
 */
static unsigned char *raw_to_jpg(cl_image_s *image, unsigned int *bsize)
{
    unsigned char *data = NULL, *jpg = NULL;
    bool color, free_data = false;

    /*
     * We still don't known how to convert a YUV RAW image to JPG so we
     * need to convert.
     */
    switch (image->raw.hdr.format) {
    case CL_IMAGE_FMT_YUV422:
    case CL_IMAGE_FMT_YUV420:
    case CL_IMAGE_FMT_YUYV:
        free_data = true;
        data = cl_raw_cvt_format((const unsigned char *)image->raw.headless,
                                 image->raw.hdr.format, image->raw.hdr.width,
                                 image->raw.hdr.height, CL_IMAGE_FMT_RGB,
                                 bsize);

        break;

    default:
        data = (unsigned char *)image->raw.headless;
        break;
    }

    if (image->raw.hdr.format == CL_IMAGE_FMT_GRAY)
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

static unsigned char *raw_to_other(cl_image_s *image, unsigned int *bsize,
    const char *other_extension)
{
    unsigned char *bjpg = NULL, *b = NULL;
    unsigned int bjpg_size = 0;
    CvMat *mat;
    IplImage *d, *ipl_jpg;
    int color = 0;

    /* First convert to JPG */
    bjpg = raw_to_jpg(image, &bjpg_size);

    if (NULL == bjpg)
        return NULL;

    if (image->format != CL_IMAGE_FMT_GRAY)
        color = 1;

    /* Load the bpjg to IplImage */
    ipl_jpg = jpg_buffer_to_ocv(bjpg, image->raw.hdr.width,
                                image->raw.hdr.height, color);

    /* Then we convert to the desired format */
    mat = cvEncodeImage(other_extension, ipl_jpg, 0);
    d = cvDecodeImage(mat, color);
    *bsize = d->imageSize;
    b = (unsigned char *)d->imageData;
    cvReleaseImageHeader(&d);
    cvReleaseImage(&ipl_jpg);

    if (bjpg != NULL)
        free(bjpg);

    return b;
}

static unsigned char *convert_raw_image(cl_image_s *image,
    enum cl_image_type type, unsigned int *bsize)
{
    unsigned char *b;

    switch (type) {
    case CL_IMAGE_JPG:
        b = raw_to_jpg(image, bsize);
        break;

    case CL_IMAGE_BMP:
        b = raw_to_other(image, bsize, "."EXT_BMP);
        break;

    case CL_IMAGE_PNG:
        b = raw_to_other(image, bsize, "."EXT_PNG);
        break;

    case CL_IMAGE_JPG2K:
        b = raw_to_other(image, bsize, "."EXT_JPG2K);
        break;

    case CL_IMAGE_TIFF:
        b = raw_to_other(image, bsize, "."EXT_TIFF);
        break;

    case CL_IMAGE_PPM:
        b = raw_to_other(image, bsize, "."EXT_PPM);
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
static unsigned char *cv_cvt_image_type(cl_image_s *image,
    const char *out_extension, unsigned int *bsize)
{
    IplImage *d = NULL;
    CvMat *mat;
    unsigned char *b = NULL;
    int color = 0;

    if (image->format != CL_IMAGE_FMT_GRAY)
        color = 1;

    mat = cvEncodeImage(out_extension, image->image, 0);
    d = cvDecodeImage(mat, color);
    b = (unsigned char *)d->imageData;
    *bsize = d->imageSize;
    cvReleaseImageHeader(&d);

    return b;
}

static unsigned char *jpg_to_raw(cl_image_s *image, unsigned int *bsize)
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

static unsigned char *other_to_raw(cl_image_s *image, unsigned int *bsize)
{
    unsigned char *b = NULL;
    CvMat *mat;
    IplImage *d;
    int color = 0;
    cl_image_t *i;

    /* First we need to convert to JPG */
    if (image->format != CL_IMAGE_FMT_GRAY)
        color = 1;

    mat = cvEncodeImage("."EXT_JPG, image->image, 0);
    d = cvDecodeImage(mat, color);

    /* Create a temporary cl_image_t */
    i = cl_image_create();

    if (NULL == i)
        return NULL;

    cl_image_cv_import(i, d, CL_IMAGE_JPG);

    /* Then we convert to RAW */
    b = jpg_to_raw(i, bsize);
    cl_image_destroy(i);

    return b;
}

static unsigned char *convert_image_to_raw(cl_image_s *image,
    unsigned int *bsize)
{
    unsigned char *b = NULL;

    switch (image->type) {
    case CL_IMAGE_JPG:
        b = jpg_to_raw(image, bsize);
        break;

    case CL_IMAGE_BMP:
    case CL_IMAGE_PNG:
    case CL_IMAGE_JPG2K:
    case CL_IMAGE_TIFF:
    case CL_IMAGE_PPM:
        b = other_to_raw(image, bsize);
        break;

    default:
        break;
    }

    return b;
}

/*
 *
 * Internal API
 *
 */

CL_INTERNAL_API
IplImage *jpg_buffer_to_ocv(const unsigned char *buffer, unsigned int width,
    unsigned int height, int color)
{
    CvMat mat;

    mat = cvMat(width, height, CV_8UC1, (void *)buffer);

    return cvDecodeImage(&mat, color);
}

CL_INTERNAL_API
IplImage *cl_image_to_ocv(cl_image_s *image)
{
    unsigned char *bjpg = NULL;
    unsigned int bjpg_size = 0;
    int color = 0;
    IplImage *ipl;
    CvMat mat;

    if (image->type != CL_IMAGE_RAW)
        return image->image;

    bjpg = raw_to_jpg(image, &bjpg_size);

    if (NULL == bjpg)
        return NULL;

    if (image->format != CL_IMAGE_FMT_GRAY)
        color = 1;

    mat = cvMat(image->raw.hdr.width, image->raw.hdr.height, CV_8UC1,
                (void *)bjpg);

    ipl = cvDecodeImage(&mat, color);

    if (bjpg != NULL)
        free(bjpg);

    return ipl;
}

CL_INTERNAL_API
IplImage *convert_image_to_cv_image(cl_image_s *image)
{
    if (image->type != CL_IMAGE_RAW)
        return image->image;

    return NULL;
}

CL_INTERNAL_API
unsigned char *convert_image_formats(cl_image_s *image, enum cl_image_type type,
    enum cl_image_color_format format __attribute__((unused)),
    unsigned int *bsize)
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
    if (image->type == CL_IMAGE_RAW) {
        buffer = convert_raw_image(image, type, bsize);
    } else {
        /*
         * If the desired output is a RAW image and we're holding another type
         * we're going to need to "manually" convert it.
         */
        if (type == CL_IMAGE_RAW)
            buffer = convert_image_to_raw(image, bsize);
        else {
            /* Otherwise we just use OpenCv */
            ext = cl_image_type_to_extension(type);
            asprintf(&cv_ext, ".%s", ext);
            buffer = cv_cvt_image_type(image, cv_ext, bsize);
            free(cv_ext);
            free(ext);
        }
    }

    return buffer;
}

CL_INTERNAL_API
unsigned char *convert_raw_formats(cl_image_s *image,
    enum cl_image_color_format format, unsigned int *bsize)
{
    /* With these choices we don't need to convert the image */
    if ((format == image->format) ||
        image->format == CL_IMAGE_FMT_GRAY)
    {
        *bsize = image->raw.hdr.size;
        return (unsigned char *)image->raw.headless;
    }

    return cl_raw_cvt_format((const unsigned char *)image->raw.headless,
                             image->format, image->raw.hdr.width,
                             image->raw.hdr.height, format, bsize);
}

