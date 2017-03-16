
/*
 * Description: API do handle images, with a "little" help from the OpenCv
 *              library. Also here we have an internal API to handle raw
 *              images.
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Sep  4 16:39:46 2016
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

#include <limits.h>
#include <libgen.h>

#include "collections.h"
#include "image.h"

/*
 * Destroy a cimage_t object.
 */
static void destroy_cimage(const struct cref_s *ref)
{
    cimage_s *image = cl_container_of(ref, cimage_s, ref);

    if (NULL == image)
        return;

    if (image->type == CIMAGE_RAW) {
        if ((image->fill_format == CIMAGE_FILL_COPY) ||
            (image->fill_format == CIMAGE_FILL_OWNER))
        {
            free(image->raw.original);
        }
    } else {
        if (image->image != NULL)
            cvReleaseImage(&image->image);
    }

    free(image);
    image = NULL;
}

/*
 * Creates an empty cimage_t object.
 */
static cimage_s *new_cimage(void)
{
    cimage_s *i = NULL;

    i = calloc(1, sizeof(cimage_s));

    if (NULL == i) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    i->image = NULL;
    i->raw.original = NULL;
    i->raw.headless = NULL;

    /* Initialize the reference count */
    i->ref.free = destroy_cimage;
    i->ref.count = 1;

    set_typeof(CIMAGE, i);

    return i;
}

static char *build_disc_filename(enum cimage_type file_type,
    const char *original_filename)
{
    char *output_ext = NULL, *p = NULL, *dirc, *dname;
    cstring_t *filename = NULL, *rnd = NULL;

    dirc = strdup(original_filename);
    dname = dirname(dirc);
    filename = cstring_create("%s/", dname);
    free(dirc);

    rnd = cstring_create_random(16);

    if (NULL == filename)
        return NULL;

    cstring_cat(filename, "%s", cstring_valueof(rnd));
    cstring_unref(rnd);
    output_ext = cimage_type_to_extension(file_type);

    if (output_ext != NULL) {
        cstring_cat(filename, ".%s", output_ext);
        free(output_ext);

        p = strdup(cstring_valueof(filename));
        cstring_unref(filename);
    }

    return p;
}

static void rename_image(const char *old, const char *new)
{
    rename(old, new);
}

/*
 * Sets all internal cimage_t object information.
 */
static void cimage_set_info(cimage_s *image, enum cimage_type type,
    enum cimage_format format)
{
    image->type = type;
    image->format = format;
}

/*
 * Releases the old OpenCv image.
 */
static void release_old_image(cimage_s *image)
{
    if (NULL == image)
        return;

    if (image->image != NULL)
        cvReleaseImage(&image->image);
}

/*
 * Fills a cimage_t object with a previously loaded image buffer.
 */
static int fill_buffer_to_cimage(cimage_s *image, const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height, enum cimage_fill_format fill_format)
{
    CvMat mat;
    int color = 0;
    enum cimage_type type;

    release_old_image(image);

    if (format != CIMAGE_FMT_GRAY)
        color = 1;

    /*
     * We try to detect which kind of image buffer was passed to the function.
     * If we don't know its type, it is a CIMAGE_RAW. ;-)
     */
    type = cimage_detect_type(buffer, bsize);

    if (type == CIMAGE_RAW) {
        if (fill_raw_image(image, buffer, format, width, height,
                           fill_format) < 0)
        {
            return -1;
        }
    } else {
        mat = cvMat(width, height, CV_8UC1, (void *)buffer);
        image->image = cvDecodeImage(&mat, color);
    }

    cimage_set_info(image, type, format);

    return 0;
}

/*
 * Return the image format by investigating the OpenCv image structure. Since
 * we're using the number of channels to make this guess, we're not able to
 * differentiate a RGB from a BGR or a YUV422 from a YUV420.
 */
static enum cimage_format cimage_detect_format_by_image(IplImage *image)
{
    switch (image->nChannels) {
        case 1:
            return CIMAGE_FMT_GRAY;

        case 2:
            return CIMAGE_FMT_YUV422;

        case 3:
            return CIMAGE_FMT_RGB;
    }

    return CIMAGE_FMT_UNKNOWN;
}

static cimage_t *duplicate_image(cimage_s *image)
{
    cimage_s *i = NULL;

    if (NULL == image->image) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    i = new_cimage();

    if (NULL == i)
        return NULL;

    if (image->type == CIMAGE_RAW) {
        /* TODO */
    } else {
        i->type = image->type;
        i->format = image->format;
        i->image = cvCloneImage(image->image);
    }

    return i;
}

/*
 *
 * Public API.
 *
 */

__PUB_API__ cimage_t *cimage_ref(cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, NULL);
    cref_inc(&i->ref);

    return image;
}

__PUB_API__ int cimage_unref(cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);
    cref_dec(&i->ref);

    return 0;
}

__PUB_API__ int cimage_destroy(cimage_t *image)
{
    return cimage_unref(image);
}

__PUB_API__ cimage_t *cimage_create(void)
{
    cimage_s *i = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    i = new_cimage();

    if (NULL == i)
        return NULL;

    return i;
}

__PUB_API__ int cimage_fill(cimage_t *image, const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height, enum cimage_fill_format fill_format)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return fill_buffer_to_cimage(i, buffer, bsize, format, width, height,
                                 fill_format);
}

__PUB_API__ cimage_t *cimage_load(const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height, enum cimage_fill_format fill_format)
{
    cimage_s *i = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    i = new_cimage();

    if (NULL == i)
        return NULL;

    if (fill_buffer_to_cimage(i, buffer, bsize, format, width,
                              height, fill_format) < 0)
    {
        cimage_unref(i);
        return NULL;
    }

    return i;
}

__PUB_API__ cimage_t *cimage_load_from_file(const char *filename)
{
    cimage_s *i = NULL;
    enum cimage_type type;
    enum cimage_format format;
    char *tmp = NULL, *ext = NULL;
    bool was_renamed = false;
    int error;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    i = new_cimage();

    if (NULL == i)
        return NULL;

    type = cimage_detect_type_from_file(filename);

    /* Do we need to rename the image so the OpenCv can handle it? */
    if (is_known_extension(filename) == false) {
        ext = cimage_type_to_extension(type);
        asprintf(&tmp, "%s.%s", filename, ext);
        rename_image(filename, tmp);
        was_renamed = true;
        free(ext);
    } else
        tmp = strdup(filename);

    if (type == CIMAGE_RAW) {
        if (raw_load(tmp, i) < 0)
            goto error_block;

        /* We already have this info while loading a raw image */
        format = i->format;
    } else {
        i->image = cvLoadImage(tmp, -1);

        if (NULL == i->image) {
            cset_errno(CL_UNABLE_TO_LOAD_IMAGE);
            goto error_block;
        }

        format = cimage_detect_format_by_image(i->image);
    }

    cimage_set_info(i, type, format);

    if (was_renamed == true)
        rename_image(tmp, filename);

    if (tmp != NULL)
        free(tmp);

    return i;

error_block:
    error = cget_last_error();
    cimage_unref(i);
    cset_errno(error);

    if (tmp != NULL)
        free(tmp);

    return NULL;
}

__PUB_API__ int cimage_save(const cimage_t *image, unsigned char **buffer,
    unsigned int *bsize)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (i->type == CIMAGE_RAW) {
        if (raw_save_to_mem(image, buffer, bsize) < 0)
            return -1;
    } else {
        *buffer = cmemdup(i->image->imageData, i->image->imageSize);
        *bsize = i->image->imageSize;
    }

    return 0;
}

static void save_image_to_file(const char *filename, cimage_s *image,
    enum cimage_type type)
{
    unsigned char *ptr;
    unsigned int bsize;

    /* If we're not holding a RAW image, just use openCv to convert */
    if (image->type != CIMAGE_RAW) {
        cvSaveImage(filename, image->image, 0);
        return;
    }

    /* We have a RAW image inside, maybe we need to convert it */
    ptr = convert_image_formats(image, type, 0, &bsize);
    cfsave(filename, ptr, bsize);
    free(ptr);
}

__PUB_API__ int cimage_save_to_file(const cimage_t *image, const char *filename,
    enum cimage_type file_type)
{
    char *disc_filename = NULL;
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    disc_filename = build_disc_filename(file_type, filename);

    if (NULL == disc_filename) {
        /* TODO: set error code */
        return -1;
    }

    if (file_type != CIMAGE_RAW)
        save_image_to_file(disc_filename, i, file_type);
    else {
        /* Handle the raw image special case */
        if (raw_save(i, disc_filename) < 0) {
            /* TODO: set error code */
            return -1;
        }
    }

    /*
     * The user may pass any extension since the output format is set by
     * @file_type. But, if it is a unknown kind of extension, we rename the
     * file to it.
     */
    rename_image(disc_filename, filename);
    free(disc_filename);

    return 0;
}

__PUB_API__ cimage_t *cimage_dup(const cimage_t *image)
{
    __clib_function_init__(true, image, CIMAGE, NULL);

    return duplicate_image((cimage_s *)image);
}

__PUB_API__ cimage_t *cimage_resize(const cimage_t *image, unsigned int width,
    unsigned int height)
{
    cimage_s *i = (cimage_s *)image, *p = NULL;

    __clib_function_init__(true, image, CIMAGE, NULL);

    if ((width == 0) || (height == 0) ||
        (width >= UINT_MAX) || (height >= UINT_MAX))
    {
        cset_errno(CL_NUMBER_RANGE);
        return NULL;
    }

    p = new_cimage();

    if (NULL == p)
        return NULL;

    p->image = cvCreateImage(cvSize(width, height), i->image->depth,
                             i->image->nChannels);

    if (NULL == p->image) {
        /* set error code */
        return NULL;
    }

    cvResize(i->image, p->image, CV_INTER_LINEAR);

    return p;
}

void cimage_interpolation(void)
{
    /* TODO */
}

__PUB_API__ cimage_t *cimage_extract(const cimage_t *image, unsigned int x,
    unsigned int y, unsigned int w, unsigned int h)
{
    cimage_s *i = (cimage_s *)image, *p;

    __clib_function_init__(true, image, CIMAGE, NULL);

    if ((w == 0) || (h == 0)) {
        cset_errno(CL_NUMBER_RANGE);
        return NULL;
    }

    p = new_cimage();

    if (NULL == p)
        return NULL;

    cvSetImageROI(i->image, cvRect(x, y, w, h));
    p->image = cvCreateImage(cvGetSize(i->image), i->image->depth,
                             i->image->nChannels);

    if (NULL == p->image) {
        /* set error code */
        return NULL;
    }

    cvCopy(i->image, p->image, NULL);
    cvResetImageROI(i->image);

    return p;
}

void cimage_cat(void)
{
    /* TODO */
}

__PUB_API__ unsigned char *cimage_raw_export(const cimage_t *image,
    enum cimage_type type, enum cimage_format format, unsigned int *bsize,
    unsigned int *width, unsigned int *height)
{
    cimage_s *i = (cimage_s *)image;
    unsigned char *buffer = NULL;

    __clib_function_init__(true, image, CIMAGE, NULL);

    /* If is not a RAW image we'll need to convert between image formats */
    if (type != CIMAGE_RAW) {
        buffer = convert_image_formats(i, type, format, bsize);
        *width = i->image->width;
        *height = i->image->height;
    } else {
        /* Or we'll only need to manipulate the RAW and convert the format */
        buffer = convert_raw_formats(i, format, bsize);
        *width = i->raw.hdr.width;
        *height = i->raw.hdr.height;
    }

    return buffer;
}

__PUB_API__ const unsigned char *cimage_raw_content(const cimage_t *image,
    unsigned int *bsize, unsigned int *width, unsigned int *height,
    enum cimage_format *format)
{
    cimage_s *i = (cimage_s *)image;
    unsigned char *ptr = NULL;

    __clib_function_init__(true, image, CIMAGE, NULL);

    if (i->type != CIMAGE_RAW) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    *width = i->raw.hdr.width;
    *height = i->raw.hdr.height;
    *bsize = *width * *height * get_channels_by_format(i->format);
    *format = i->format;
    ptr = i->raw.headless;

    /*
     * For this to work, the user must keep a reference to the cimage_t
     * object for as long as this pointer lives.
     */
    return ptr;
}

__PUB_API__ IplImage *cimage_cv_export(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, NULL);

    if (i->type == CIMAGE_RAW) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    /*
     * Here we're exporting the OpenCv image so the user may used it in any
     * way he want.
     */
    return i->image;
}

__PUB_API__ int cimage_cv_import(cimage_t *image, IplImage *cv_image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == cv_image) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    release_old_image(i);
    i->image = cv_image;

    /* TODO: Need to inform the correctly type and format */
    i->type = CIMAGE_JPG;

    return 0;
}
