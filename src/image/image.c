
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
 * Destroy a cl_image_t object.
 */
static void destroy_cimage(const struct cl_ref_s *ref)
{
    cl_image_s *image = cl_container_of(ref, cl_image_s, ref);

    if (NULL == image)
        return;

    if (image->type == CL_IMAGE_RAW) {
        if ((image->fill_format == CL_IMAGE_FILL_COPY) ||
            (image->fill_format == CL_IMAGE_FILL_OWNER))
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
 * Creates an empty cl_image_t object.
 */
static cl_image_s *new_cimage(void)
{
    cl_image_s *i = NULL;

    i = calloc(1, sizeof(cl_image_s));

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

    set_typeof(CL_OBJ_IMAGE, i);

    return i;
}

static char *build_disc_filename(enum cl_image_type file_type,
    const char *original_filename)
{
    char *output_ext = NULL, *p = NULL, *dirc, *dname;
    cl_string_t *filename = NULL, *rnd = NULL;

    dirc = strdup(original_filename);
    dname = dirname(dirc);

    if (strlen(dname) > 1)
        filename = cl_string_create("%s/", dname);
    else
        filename = cl_string_create_empty(0);

    free(dirc);
    rnd = cl_string_create_random(16);

    if (NULL == filename)
        return NULL;

    cl_string_cat(filename, "%s", cl_string_valueof(rnd));
    cl_string_unref(rnd);
    output_ext = cl_image_type_to_extension(file_type);

    if (output_ext != NULL) {
        cl_string_cat(filename, ".%s", output_ext);
        free(output_ext);

        p = strdup(cl_string_valueof(filename));
        cl_string_unref(filename);
    } else
        cset_errno(CL_UNSUPPORTED_TYPE);

    return p;
}

static void rename_image(const char *old, const char *new)
{
    rename(old, new);
}

/*
 * Sets all internal cl_image_t object information.
 */
static void cl_image_set_info(cl_image_s *image, enum cl_image_type type,
    enum cl_image_color_format format)
{
    image->type = type;
    image->format = format;
}

/*
 * Releases the old OpenCv image.
 */
static void release_old_image(cl_image_s *image)
{
    if (NULL == image)
        return;

    if (image->image != NULL)
        cvReleaseImage(&image->image);
}

/*
 * Fills a cl_image_t object with a previously loaded image buffer.
 */
static int fill_buffer_to_cimage(cl_image_s *image, const unsigned char *buffer,
    unsigned int bsize, enum cl_image_color_format format, unsigned int width,
    unsigned int height, enum cl_image_fill_format fill_format)
{
    CvMat mat;
    int color = 0;
    enum cl_image_type type;

    release_old_image(image);

    /*
     * We try to detect which kind of image buffer was passed to the function.
     * If we don't know its type, it is a CL_IMAGE_RAW. ;-)
     */
    type = cl_image_detect_type(buffer, bsize);

    if (type == CL_IMAGE_RAW) {
        if (fill_raw_image(image, buffer, format, width, height,
                           fill_format) < 0)
        {
            return -1;
        }
    } else {
        if (format != CL_IMAGE_FMT_GRAY)
            color = 1;

        mat = cvMat(width, height, CV_8UC1, (void *)buffer);
        image->image = cvDecodeImage(&mat, color);
        image->fill_format = CL_IMAGE_FILL_COPY;
    }

    cl_image_set_info(image, type, format);

    return 0;
}

/*
 * Return the image format by investigating the OpenCv image structure. Since
 * we're using the number of channels to make this guess, we're not able to
 * differentiate a RGB from a BGR or a YUV422 from a YUV420.
 */
static enum cl_image_color_format cl_image_detect_format_by_image(IplImage *image)
{
    switch (image->nChannels) {
        case 1:
            return CL_IMAGE_FMT_GRAY;

        case 2:
            return CL_IMAGE_FMT_YUV422;

        case 3:
            return CL_IMAGE_FMT_RGB;
    }

    return CL_IMAGE_FMT_UNKNOWN;
}

static cl_image_t *duplicate_image(cl_image_s *image)
{
    cl_image_s *i = NULL;

    if (NULL == image->image) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    i = new_cimage();

    if (NULL == i)
        return NULL;

    if (image->type == CL_IMAGE_RAW) {
        if (fill_raw_image(i, image->raw.original, image->raw.hdr.format,
                           image->raw.hdr.width, image->raw.hdr.height,
                           CL_IMAGE_FILL_COPY) < 0)
        {
            return NULL;
        }
    } else
        i->image = cvCloneImage(image->image);

    i->type = image->type;
    i->format = image->format;

    /*
     * Independently of the original buffer reference type, since we're
     * returning a duplicated image, we fix fill_format as CL_IMAGE_FILL_COPY.
     */
    i->fill_format = CL_IMAGE_FILL_COPY;

    return i;
}

static void save_image_to_file(const char *filename, cl_image_s *image,
    enum cl_image_type type)
{
    unsigned char *ptr;
    unsigned int bsize;

    /* If we're not holding a RAW image, just use openCv to convert */
    if (image->type != CL_IMAGE_RAW) {
        cvSaveImage(filename, image->image, 0);
        return;
    }

    /* We have a RAW image inside, maybe we need to convert it */
    ptr = convert_image_formats(image, type, 0, &bsize);
    cl_fsave(filename, ptr, bsize);
    free(ptr);
}

/*
 *
 * Public API
 *
 */

__PUB_API__ cl_image_t *cl_image_ref(cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);
    cl_ref_inc(&i->ref);

    return image;
}

__PUB_API__ int cl_image_unref(cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);
    cl_ref_dec(&i->ref);

    return 0;
}

__PUB_API__ int cl_image_destroy(cl_image_t *image)
{
    return cl_image_unref(image);
}

__PUB_API__ cl_image_t *cl_image_create(void)
{
    cl_image_s *i = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    i = new_cimage();

    if (NULL == i)
        return NULL;

    return i;
}

__PUB_API__ int cl_image_fill(cl_image_t *image, const unsigned char *buffer,
    unsigned int bsize, enum cl_image_color_format format, unsigned int width,
    unsigned int height, enum cl_image_fill_format fill_format)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if ((is_supported_color_format(format) == false) ||
        (is_supported_fill_format(fill_format) == false))
    {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    return fill_buffer_to_cimage(i, buffer, bsize, format, width, height,
                                 fill_format);
}

__PUB_API__ cl_image_t *cl_image_load(const unsigned char *buffer,
    unsigned int bsize, enum cl_image_color_format format, unsigned int width,
    unsigned int height, enum cl_image_fill_format fill_format)
{
    cl_image_s *i = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if ((is_supported_color_format(format) == false) ||
        (is_supported_fill_format(fill_format) == false))
    {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    i = new_cimage();

    if (NULL == i)
        return NULL;

    if (fill_buffer_to_cimage(i, buffer, bsize, format, width,
                              height, fill_format) < 0)
    {
        cl_image_unref(i);
        return NULL;
    }

    return i;
}

__PUB_API__ cl_image_t *cl_image_load_from_file(const char *filename)
{
    cl_image_s *i = NULL;
    enum cl_image_type type;
    enum cl_image_color_format format;
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

    type = cl_image_detect_type_from_file(filename);

    /* Do we need to rename the image so the OpenCv can handle it? */
    if (is_known_extension(filename) == false) {
        ext = cl_image_type_to_extension(type);
        asprintf(&tmp, "%s.%s", filename, ext);
        rename_image(filename, tmp);
        was_renamed = true;
        free(ext);
    } else
        tmp = strdup(filename);

    if (type == CL_IMAGE_RAW) {
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

        format = cl_image_detect_format_by_image(i->image);
    }

    cl_image_set_info(i, type, format);
    i->fill_format = CL_IMAGE_FILL_COPY;

    if (was_renamed == true)
        rename_image(tmp, filename);

    if (tmp != NULL)
        free(tmp);

    return i;

error_block:
    error = cl_get_last_error();
    cl_image_unref(i);
    cset_errno(error);

    if (tmp != NULL)
        free(tmp);

    return NULL;
}

__PUB_API__ int cl_image_save(const cl_image_t *image, unsigned char **buffer,
    unsigned int *bsize)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (i->type == CL_IMAGE_RAW) {
        if (raw_save_to_mem(image, buffer, bsize) < 0)
            return -1;
    } else {
        *buffer = cl_memdup(i->image->imageData, i->image->imageSize);
        *bsize = i->image->imageSize;
    }

    return 0;
}

__PUB_API__ int cl_image_save_to_file(const cl_image_t *image,
    const char *filename, enum cl_image_type file_type)
{
    char *disc_filename = NULL;
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (has_internal_image(i) == false) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if (is_supported_image_type(file_type) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    disc_filename = build_disc_filename(file_type, filename);

    if (NULL == disc_filename)
        return -1;

    if (file_type != CL_IMAGE_RAW)
        save_image_to_file(disc_filename, i, file_type);
    else {
        /* Handle the raw image special case */
        if (raw_save(i, disc_filename) < 0)
            return -1;
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

__PUB_API__ cl_image_t *cl_image_dup(const cl_image_t *image)
{
    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    return duplicate_image((cl_image_s *)image);
}

__PUB_API__ cl_image_t *cl_image_resize(const cl_image_t *image,
    unsigned int width, unsigned int height)
{
    cl_image_s *i = (cl_image_s *)image, *p = NULL;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    if ((width == 0) || (height == 0) ||
        (width >= UINT_MAX) || (height >= UINT_MAX))
    {
        cset_errno(CL_NUMBER_RANGE);
        return NULL;
    }

    p = new_cimage();

    if (NULL == p)
        return NULL;

    if (i->type == CL_IMAGE_RAW) {
        if (raw_resize(p, i, width, height) < 0)
            return NULL;
    } else {
        p->image = cvCreateImage(cvSize(width, height), i->image->depth,
                                 i->image->nChannels);

        if (NULL == p->image) {
            cset_errno(CL_UNABLE_TO_CREATE_TMP_IMAGE);
            return NULL;
        }

        cvResize(i->image, p->image, CV_INTER_LINEAR);
        p->fill_format = CL_IMAGE_FILL_COPY;
    }

    cl_image_set_info(p, i->type, i->format);

    return p;
}

__PUB_API__ cl_image_t *cl_image_extract(const cl_image_t *image,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    cl_image_s *i = (cl_image_s *)image, *p;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    if ((w == 0) || (h == 0)) {
        cset_errno(CL_NUMBER_RANGE);
        return NULL;
    }

    p = new_cimage();

    if (NULL == p)
        return NULL;

    if (i->type == CL_IMAGE_RAW) {
        if (raw_extract(p, i, x, y, w, h) < 0)
            return NULL;
    } else {
        cvSetImageROI(i->image, cvRect(x, y, w, h));
        p->image = cvCreateImage(cvGetSize(i->image), i->image->depth,
                                 i->image->nChannels);

        if (NULL == p->image) {
            cset_errno(CL_UNABLE_TO_CREATE_TMP_IMAGE);
            return NULL;
        }

        cvCopy(i->image, p->image, NULL);
        cvResetImageROI(i->image);
    }

    return p;
}

void cl_image_cat(void)
{
    /* TODO */
}

__PUB_API__ unsigned char *cl_image_bin_export(const cl_image_t *image,
    enum cl_image_type type, enum cl_image_color_format format,
    unsigned int *bsize, unsigned int *width, unsigned int *height)
{
    cl_image_s *i = (cl_image_s *)image;
    unsigned char *buffer = NULL;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    if ((is_supported_image_type(type) == false) ||
        (is_supported_color_format(format) == false))
    {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    /* If is not a RAW image we'll need to convert between image formats */
    if (type != CL_IMAGE_RAW) {
        buffer = convert_image_formats(i, type, format, bsize);
        *width = i->image->width;
        *height = i->image->height;
    } else {
        /*
         * Or we'll only need to manipulate the RAW and convert its color
         * format.
         */
        buffer = convert_raw_formats(i, format, bsize);
        *width = i->raw.hdr.width;
        *height = i->raw.hdr.height;
    }

    return buffer;
}

__PUB_API__ const unsigned char *cl_image_bin_content(const cl_image_t *image,
    unsigned int *bsize, unsigned int *width, unsigned int *height,
    enum cl_image_color_format *format)
{
    cl_image_s *i = (cl_image_s *)image;
    unsigned char *ptr = NULL;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    if (i->type == CL_IMAGE_RAW) {
        *width = i->raw.hdr.width;
        *height = i->raw.hdr.height;
        *bsize = *width * *height * get_channels_by_format(i->format);
        *format = i->format;
        ptr = i->raw.headless;
    } else {
        *width = i->image->width;
        *height = i->image->height;
        *bsize = i->image->imageSize;
        *format = (i->image->nChannels == 1) ? CL_IMAGE_FMT_GRAY
                                             : CL_IMAGE_FMT_RGB;

        ptr = (unsigned char *)i->image->imageData;
    }

    /*
     * For this to work, the user must keep a reference to the cl_image_t
     * object for as long as this pointer lives.
     */
    return ptr;
}

__PUB_API__ IplImage *cl_image_cv_export(const cl_image_t *image)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, NULL);

    if (i->type == CL_IMAGE_RAW) {
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

__PUB_API__ int cl_image_cv_import(cl_image_t *image, IplImage *cv_image,
    enum cl_image_type type)
{
    cl_image_s *i = (cl_image_s *)image;

    __clib_function_init__(true, image, CL_OBJ_IMAGE, -1);

    if (NULL == cv_image) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (is_supported_image_type(type) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    release_old_image(i);
    i->image = cv_image;
    i->type = type;

    return 0;
}

