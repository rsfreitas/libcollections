
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

/* To the main image manipulation we use the OpenCv */
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

/* To do any kind of RAW format conversion we use the libswscale */
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include "collections.h"

/* Known file extensions */
#define EXT_RAW         "raw"
#define EXT_PPM         "ppm"
#define EXT_TIFF        "tiff"
#define EXT_JPG2K       "jp2"
#define EXT_PNG         "png"
#define EXT_BMP         "bmp"
#define EXT_JPG         "jpg"

/* A RAW image file header */
struct raw_header {
    unsigned int        id;
    unsigned int        size;
    unsigned int        width;
    unsigned int        height;
    unsigned int        format;
};

#define cimage_members                                      \
    cl_struct_member(IplImage *, image)                     \
    cl_struct_member(unsigned char *, raw_original_ptr)     \
    cl_struct_member(enum cimage_format, format)            \
    cl_struct_member(enum cimage_type, type)                \
    cl_struct_member(struct raw_header, raw_hdr)            \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(cimage_s, cimage_members);

#define cimage_s    cl_struct(cimage_s)

static int get_channels_by_format(enum cimage_format format)
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

/*
 *
 * Raw image format API.
 *
 */

#define RAW_ID              cl_fourcc('R', 'A', 'W', '1')

static bool is_header_from_raw_image(const unsigned char *buffer)
{
    struct raw_header hdr;

    memset(&hdr, 0, sizeof(struct raw_header));
    memcpy(&hdr, buffer, sizeof(struct raw_header));

    if (hdr.id == RAW_ID)
        return true;

    return false;
}

static int raw_load_from_mem(const unsigned char *buffer, cimage_s *image)
{
    struct raw_header hdr;

    memcpy(&hdr, buffer, sizeof(struct raw_header));

    if (hdr.id != RAW_ID)
        return -1;

    image->image = cvCreateImage(cvSize(hdr.width, hdr.height), IPL_DEPTH_8U,
                                 get_channels_by_format(hdr.format));

    if (NULL == image->image)
        return -1;

    /*
     * We need to save the original pointer, so we can free it correctly later.
     */
    image->raw_original_ptr = (unsigned char *)buffer;
    image->image->imageData = (char *)buffer + sizeof(struct raw_header);
    image->type = CIMAGE_RAW;
    image->format = hdr.format;

    return 0;
}

static int raw_load(const char *filename, cimage_s *image)
{
    unsigned char *buffer = NULL;
    unsigned int bsize = 0;
    int ret;

    buffer = cfload(filename, &bsize);

    if (NULL == buffer)
        return -1;

    ret = raw_load_from_mem(buffer, image);

    if (ret < 0)
        if (buffer != NULL)
            free(buffer);

    return ret;
}

static int raw_save_to_mem(const cimage_s *image, unsigned char **buffer,
    unsigned int *bsize)
{
    struct raw_header hdr;
    unsigned char *b = NULL;

    /* Create the RAW image header, so we can save it correctly */
    hdr.id = RAW_ID;
    hdr.width = cimage_width(image);
    hdr.height = cimage_height(image);
    hdr.size = cimage_size(image);
    hdr.format = cimage_format(image);

    *bsize = hdr.size + sizeof(struct raw_header);
    b = calloc(*bsize, sizeof(unsigned char));

    if (NULL == b)
        return -1;

    memcpy(b, &hdr, sizeof(struct raw_header));
    memcpy(b + sizeof(struct raw_header), image->image->imageData,
           hdr.size);

    *buffer = b;

    return 0;
}

static int raw_save(const cimage_s *image, const char *filename)
{
    unsigned char *buffer = NULL;
    unsigned int bsize = 0;

    if (raw_save_to_mem(image, &buffer, &bsize) < 0)
        return -1;

    cfsave(filename, buffer, bsize);
    free(buffer);

    return 0;
}

/*
 *
 * Internal image API functions.
 *
 */

/*
 * Destroy a cimage_t object.
 */
static void destroy_cimage(const struct cref_s *ref)
{
    cimage_s *image = cl_container_of(ref, cimage_s, ref);

    if (NULL == image)
        return;

    if (image->raw_original_ptr != NULL) {
        free(image->raw_original_ptr);
        cvReleaseImageHeader(&image->image);
    } else {
        if (image->image != NULL)
            cvReleaseImage(&image->image);
    }

    free(image);
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
    i->raw_original_ptr = NULL;

    /* Initialize the reference count */
    i->ref.free = destroy_cimage;
    i->ref.count = 1;

    set_typeof(CIMAGE, i);

    return i;
}

static char *cimage_type_to_extension(enum cimage_type type)
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

static bool is_known_extension(const char *filename)
{
    char *ext = NULL;

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
        return true;
    }

    return false;
}

static char *build_disc_filename(enum cimage_type file_type)
{
    char *output_ext = NULL, *p = NULL;
    cstring_t *filename = NULL;

    filename = cstring_create_random(16);

    if (NULL == filename)
        return NULL;

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
 * Translate a libmagic mime type to a supported file type.
 */
static enum cimage_type mime_to_type(const char *mime)
{
    enum cimage_type type = CIMAGE_RAW;

    if ((strcmp(mime, "image/jpg") == 0) || (strcmp(mime, "image/jpeg") == 0))
        type = CIMAGE_JPG;
    else if (strcmp(mime, "image/x-ms-bmp") == 0)
        type = CIMAGE_BMP;
    else if (strcmp(mime, "image/png") == 0)
        type = CIMAGE_PNG;
    else if (strcmp(mime, "image/jp2") == 0)
        type = CIMAGE_JPG2K;
    else if (strcmp(mime, "image/tiff") == 0)
        type = CIMAGE_TIFF;
    else if (strcmp(mime, "image/x-portable-pixmap") == 0)
        type = CIMAGE_PPM;

    return type;
}

/*
 * This function tries to detect the image file type from the file itself.
 */
static enum cimage_type cimage_detect_type_from_file(const char *filename)
{
    magic_t *cookie = NULL;
    enum cimage_type type = CIMAGE_RAW;
    char *mime = NULL;

    cookie = library_get_cookie();

    if (NULL == cookie)
        goto end_block;

    mime = (char *)magic_file(*cookie, filename);

    if (NULL == mime)
        goto end_block;

    type = mime_to_type(mime);

end_block:
    return type;
}

/*
 * Uses libmagic to return the image type from a image buffer.
 */
static enum cimage_type cimage_detect_type(const unsigned char *buffer,
    unsigned int bsize)
{
    magic_t *cookie = NULL;
    enum cimage_type type = CIMAGE_RAW;
    char *mime = NULL;

    cookie = library_get_cookie();

    if (NULL == cookie)
        goto end_block;

    mime = (char *)magic_buffer(*cookie, (void *)buffer, (size_t)bsize);

    if (NULL == mime)
        goto end_block;

    type = mime_to_type(mime);

end_block:
    return type;
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
 * Fills the cimage_t object with a raw image buffer.
 */
static int fill_raw_image(cimage_s *image, const unsigned char *buffer,
    enum cimage_format format, unsigned int width, unsigned int height)
{
    unsigned int offset = 0;

    /* Are we receiving a RAW image with header? */
    if (is_header_from_raw_image(buffer)) {
        offset = sizeof(struct raw_header);
        memcpy(&image->raw_hdr, buffer, sizeof(struct raw_header));
    } else {
        /* Sets the RAW header informations */
        image->raw_hdr.id = RAW_ID;
        image->raw_hdr.width = width;
        image->raw_hdr.height = height;
        image->raw_hdr.format = format;
        image->raw_hdr.size = width * height * get_channels_by_format(format);
    }

    image->image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U,
                                 get_channels_by_format(format));

    if (NULL == image->image)
        return -1;

    /*
     * We just point to the raw buffer of the image, discarding the previously
     * saved RAW header.
     */
    image->image->imageData = (char *)buffer + offset;

    return 0;
}

/*
 * Fills a cimage_t object with a previously loaded image buffer.
 */
static int fill_buffer_to_cimage(cimage_s *image, const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height)
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
        if (fill_raw_image(image, buffer, format, width, height) < 0)
            return -1;
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

    i->image = cvCloneImage(image->image);

    return i;
}

/*
 *
 * Conversion routines.
 *
 */

static enum PixelFormat cimage_format_to_PixelFormat(enum cimage_format fmt)
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

static int get_raw_size(enum cimage_format format, unsigned int width,
    unsigned int height)
{
    int size = -1;

    switch (format) {
        case CIMAGE_FMT_BGR:
        case CIMAGE_FMT_RGB:
            size = width * height * 3;
            break;

        case CIMAGE_FMT_YUV422:
        case CIMAGE_FMT_YUV420:
        case CIMAGE_FMT_YUYV:
            size = width * height * 2;
            break;

        case CIMAGE_FMT_GRAY:
            size = width * height;
            break;

        default:
            break;
    }

    return size;
}

/*
 * Our "real" image format conversion routine. ;-)
 */
__PUB_API__ unsigned char *craw_cvt_format(const unsigned char *buffer,
    enum cimage_format fmt_in, unsigned int width, unsigned int height,
    enum cimage_format fmt_out, unsigned int *bsize)
{
    enum PixelFormat sws_fmt_in, sws_fmt_out;
    struct SwsContext *ctx;
    unsigned char *b = NULL;
    uint8_t *data_in[4], *data_out[4];
    int linesize[4], out_linesize[4];

    sws_fmt_in = cimage_format_to_PixelFormat(fmt_in);
    sws_fmt_out = cimage_format_to_PixelFormat(fmt_out);
    *bsize = get_raw_size(fmt_out, width, height);
    b = calloc(*bsize, sizeof(unsigned char));

    if (NULL == b)
        return NULL;

    ctx = sws_getContext(width, height, sws_fmt_in, width, height, sws_fmt_out,
                         SWS_BICUBIC, 0, 0, 0);

    if (NULL == ctx)
        return NULL;

    av_image_fill_linesizes(linesize, sws_fmt_in, width);
    av_image_fill_linesizes(out_linesize, sws_fmt_out, width);
    av_image_fill_pointers(data_in, sws_fmt_in, height, (uint8_t *)buffer,
                           linesize);

    av_image_fill_pointers(data_out, sws_fmt_out, height, b, out_linesize);
    sws_scale(ctx, (const uint8_t * const *)data_in, linesize, 0, height,
              data_out, out_linesize);

    sws_freeContext(ctx);

    return b;
}

static unsigned char *convert_raw_formats(cimage_s *image,
    enum cimage_format format, unsigned int *bsize)
{
    /* With these choices we don't need to convert the image */
    if ((format == image->format) ||
        image->format == CIMAGE_FMT_GRAY)
    {
        *bsize = image->image->imageSize;
        return (unsigned char *)image->image->imageData;
    }

    return craw_cvt_format((const unsigned char *)image->image->imageData,
                           image->format, image->image->width,
                           image->image->height, format, bsize);
}

static unsigned char *raw_to_jpg(cimage_s *image, unsigned int *bsize)
{
    unsigned char *data = NULL, *jpg = NULL;
    bool color, free_data = false;

    /*
     * We still don't known how to convert a YUV RAW image to JPG so we
     * need to convert.
     */
    switch (image->raw_hdr.format) {
        case CIMAGE_FMT_YUV422:
        case CIMAGE_FMT_YUV420:
        case CIMAGE_FMT_YUYV:
            free_data = true;
            data = craw_cvt_format((const unsigned char *)image->image->imageData,
                                   image->raw_hdr.format, image->image->width,
                                   image->image->height, CIMAGE_FMT_RGB, bsize);

            break;

        default:
            data = (unsigned char *)image->image->imageData;
            break;
    }

    if (image->raw_hdr.format == CIMAGE_FMT_GRAY)
        color = false;
    else
        color = true;

    /* directly convert to jpg */
    jpg = RAW_to_jpg_mem(data, image->raw_hdr.width, image->raw_hdr.height,
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

static unsigned char *convert_image_formats(cimage_s *image,
    enum cimage_type type, enum cimage_format format __attribute__((unused)),
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

void draw_onto_the_image(cimage_t *image, unsigned int x, unsigned int y,
    unsigned char *buffer, int bwidth, unsigned int max_width,
    unsigned int max_height, CvScalar color)
{
    cimage_s *img = (cimage_s *)image;
    unsigned char *ptr;
    unsigned int fp, ip, cp;
    int channels, ptr_width, k;
    unsigned int i, j;

    if (max_width > (img->image->width - x))
        max_width = img->image->width - x;

    if (max_height > (img->image->height - y))
        max_height = img->image->height - y;

    channels = get_channels_by_format(img->format);
    ptr = (unsigned char *)(img->image->imageData + y * img->image->widthStep +
                            x * channels);

    ptr_width = img->image->widthStep - (max_width * channels);

    for (i = 0; i < max_height; i++) {
        for (j = 0; j < max_width; j++) {
            for (k = 0; k < channels; k++) {
                fp = (unsigned char)*buffer;
                cp = (unsigned char)*ptr;
                ip = (unsigned char)color.val[k];

                *ptr = (fp * cp + (255 - fp) * ip) / 255.0f;
                ptr++;
            }

            buffer++;
        }

        ptr += ptr_width;
        buffer += bwidth;
    }
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
    unsigned int height)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return fill_buffer_to_cimage(i, buffer, bsize, format, width, height);
}

__PUB_API__ cimage_t *cimage_load(const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height)
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
                              height) < 0)
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

        if (NULL == i->image)
            goto error_block;

        format = cimage_detect_format_by_image(i->image);
    }

    cimage_set_info(i, type, format);

    if (was_renamed == true)
        rename_image(tmp, filename);

    if (tmp != NULL)
        free(tmp);

    return i;

error_block:
    cimage_unref(i);

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

__PUB_API__ int cimage_save_to_file(const cimage_t *image, const char *filename,
    enum cimage_type file_type)
{
    char *disc_filename = NULL;
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    disc_filename = build_disc_filename(file_type);

    if (NULL == disc_filename) {
        /* set error code */
        return -1;
    }

    if (file_type != CIMAGE_RAW)
        cvSaveImage(disc_filename, i->image, 0);
    else {
        /* Handle the raw image special case */
        if (raw_save(i, disc_filename) < 0) {
            /* set error code */
            return 0;
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

    *width = i->image->width;
    *height = i->image->height;

    /* If is not a RAW image we'll need to convert between image formats */
    if (type != CIMAGE_RAW)
        buffer = convert_image_formats(i, type, format, bsize);
    else
        /* Or we'll only need to manipulate the RAW and convert the format */
        buffer = convert_raw_formats(i, format, bsize);

    return buffer;
}

/*
 * FIXME: Is this a copy from the cimage_fill function?
 */
__PUB_API__ int cimage_raw_import(cimage_t *image, const unsigned char *buffer,
    unsigned int bsize, enum cimage_format format, unsigned int width,
    unsigned int height)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if ((NULL == buffer) || (bsize == 0)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return fill_buffer_to_cimage(i, buffer, bsize, format, width, height);
}

__PUB_API__ IplImage *cimage_cv_export(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, NULL);

    if (NULL == i->image) {
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

    return 0;
}

/*
 *
 * Image information
 *
 */

__PUB_API__ int cimage_size(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->image->imageSize;
}

__PUB_API__ int cimage_width(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->image->width;
}

__PUB_API__ int cimage_height(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->image->height;
}

__PUB_API__ enum cimage_type cimage_type(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->type;
}

__PUB_API__ enum cimage_format cimage_format(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return i->format;
}

__PUB_API__ int cimage_channels(const cimage_t *image)
{
    cimage_s *i = (cimage_s *)image;

    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == i->image) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return get_channels_by_format(i->format);
}

