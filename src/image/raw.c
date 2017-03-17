
/*
 * Description: Functions to handle RAW image.
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Feb 19 07:36:05 2017
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

#include <stdio.h>
#include <sys/types.h>
#include <setjmp.h>

#include <jpeglib.h>
#include <jerror.h>

#include "collections.h"
#include "image.h"

struct my_error_mgr {
    struct jpeg_error_mgr   pub;
    jmp_buf                 setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

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
    /* Are we receiving a RAW image with header? */
    if (is_header_from_raw_image(buffer))
        memcpy(&image->raw.hdr, buffer, sizeof(struct raw_header));
    else {
        /*
         * We can't load a RAW image without its header, since we need to
         * know a few informations about it.
         */
        cset_errno(CL_UNSUPPORTED_RAW_IMAGE);
        return -1;
    }

    /*
     * We need to save the original pointer, so we can free it correctly later.
     */
    image->raw.original = (unsigned char *)buffer;
    image->raw.headless = (unsigned char *)buffer + sizeof(struct raw_header);
    image->type = CIMAGE_RAW;
    image->format = image->raw.hdr.format;
    image->fill_format = CIMAGE_FILL_OWNER;

    return 0;
}

int raw_load(const char *filename, cimage_s *image)
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

int raw_save_to_mem(const cimage_s *image, unsigned char **buffer,
    unsigned int *bsize)
{
    struct raw_header hdr;
    unsigned char *b = NULL;

    /* Create the RAW image header, so we can save it correctly */
    hdr.id = RAW_ID;
    hdr.width = image->raw.hdr.width;
    hdr.height = image->raw.hdr.height;
    hdr.size = image->raw.hdr.size;
    hdr.format = image->format;

    *bsize = hdr.size + sizeof(struct raw_header);
    b = calloc(*bsize, sizeof(unsigned char));

    if (NULL == b) {
        cset_errno(CL_NO_MEM);
        return -1;
    }

    memcpy(b, &hdr, sizeof(struct raw_header));
    memcpy(b + sizeof(struct raw_header), image->raw.headless,
           hdr.size);

    *buffer = b;

    return 0;
}

int raw_save(const cimage_s *image, const char *filename)
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
 * Conversion between RAW and JPG images.
 *
 */

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

METHODDEF(void)comp_init(j_compress_ptr cinfo __attribute((unused)))
{
}

METHODDEF(int)comp_empty(j_compress_ptr cinfo __attribute((unused)))
{
    return 1;
}

METHODDEF(void)comp_term(j_compress_ptr cinfo __attribute((unused)))
{
}

unsigned char *RAW_to_jpg_mem(const unsigned char *buffer, int width,
    int height, bool color, int quality, unsigned int *jsize)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_ptr[1];
    int row_stride;
    unsigned char *bjpg = NULL;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    cinfo.dest = malloc(sizeof(struct jpeg_destination_mgr));
    cinfo.image_width = width;
    cinfo.image_height = height;

    if (color == true) {
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
    } else {
        cinfo.input_components = 1;
        cinfo.in_color_space = JCS_GRAYSCALE;
    }

    /*
     * FIXME: we need to change this to a real allocation, this size is much
     *        bigger than we need.
     */
    bjpg = calloc(width * height, sizeof(char));

    cinfo.dest->next_output_byte = bjpg;
    cinfo.dest->free_in_buffer = width * height;
    cinfo.dest->init_destination = comp_init;
    cinfo.dest->empty_output_buffer = comp_empty;
    cinfo.dest->term_destination = comp_term;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = cinfo.image_width * cinfo.input_components;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_ptr[0] = (unsigned char *)buffer + (cinfo.next_scanline *
            row_stride);

        jpeg_write_scanlines(&cinfo, row_ptr, 1);
    }

    jpeg_finish_compress(&cinfo);
    *jsize = (width * height) - cinfo.dest->free_in_buffer;
    free(cinfo.dest);
    jpeg_destroy_compress(&cinfo);

    return bjpg;
}

static void init_source(j_decompress_ptr cinfo __attribute__((unused)))
{
    return;
}

static int fill_input_buffer(j_decompress_ptr cinfo)
{
    struct jpeg_source_mgr *src = cinfo->src;
    static JOCTET FakeEOI[] = { 0xFF, JPEG_EOI };

    /* Generate warning */
    WARNMS(cinfo, JWRN_JPEG_EOF);

    /* Insert a fake EOI marker */
    src->next_input_byte = FakeEOI;
    src->bytes_in_buffer = 2;

    return TRUE;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr * src = cinfo->src;

    if (num_bytes >= (long)src->bytes_in_buffer) {
       fill_input_buffer(cinfo);
       return;
    }

    src->bytes_in_buffer -= num_bytes;
    src->next_input_byte += num_bytes;
}

static void term_source(j_decompress_ptr cinfo __attribute__((unused)))
{
    return;
}

static void __jpeg_mem_src(j_decompress_ptr cinfo, JOCTET *pData, int FileSize)
{
    struct jpeg_source_mgr * src;

    if (cinfo->src == NULL) {   /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
          (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                     sizeof(struct jpeg_source_mgr));
    }

    src = cinfo->src;

    /* Set up function pointers */
    src->init_source = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_source;

    /* Set up data pointer */
    src->bytes_in_buffer = FileSize;
    src->next_input_byte = pData;
}

unsigned char *jpg_to_RAW_mem(const unsigned char *buffer, unsigned int jsize,
    int *width, int *height, bool *color)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY jsbuffer;
    int row_stride;
    int i, j=0;
    unsigned char *bout;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return NULL;
    }

    jpeg_create_decompress(&cinfo);
    __jpeg_mem_src(&cinfo, (JOCTET *)buffer, jsize);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;

    if (cinfo.output_components == 3)
        *color = true;
    else if (cinfo.output_components == 1)
        *color = false;
    else {
        jpeg_destroy_decompress(&cinfo);
        return NULL;
    }

    bout = malloc(cinfo.output_width * cinfo.output_height *
                  cinfo.output_components);

    if (NULL == bout) {
        jpeg_destroy_decompress(&cinfo);
        return NULL;
    }

    row_stride = cinfo.output_width * cinfo.output_components;
    jsbuffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE,
                                          row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, jsbuffer, 1);

        for (i = 0; i < row_stride; i++)
            bout[j++] = jsbuffer[0][i];
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return bout;
}

static int get_raw_size(enum cimage_color_format format, unsigned int width,
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
 * Fills the cimage_t object with a raw image buffer.
 */
int fill_raw_image(cimage_s *image, const unsigned char *buffer,
    enum cimage_color_format format, unsigned int width, unsigned int height,
    enum cimage_fill_format fill_format)
{
    unsigned int offset = 0;

    /* Are we receiving a RAW image with header? */
    if (is_header_from_raw_image(buffer)) {
        offset = sizeof(struct raw_header);
        memcpy(&image->raw.hdr, buffer, sizeof(struct raw_header));
    } else {
        /* Sets the RAW header informations */
        image->raw.hdr.id = RAW_ID;
        image->raw.hdr.width = width;
        image->raw.hdr.height = height;
        image->raw.hdr.format = format;
        image->raw.hdr.size = width * height * get_channels_by_format(format);
    }

    /*
     * We just point to the raw buffer of the image, discarding the previously
     * saved RAW header.
     */
    switch (fill_format) {
        case CIMAGE_FILL_REFERENCE:
        case CIMAGE_FILL_OWNER:
            image->raw.original = (unsigned char *)buffer;
            break;

        case CIMAGE_FILL_COPY:
            image->raw.original = cmemdup((unsigned char *)buffer,
                                          image->raw.hdr.size);

            break;
    }

    image->fill_format = fill_format;
    image->raw.headless = (unsigned char *)buffer + offset;

    return 0;
}

int raw_resize(cimage_s *out, cimage_s *in, unsigned int new_width,
    unsigned int new_height)
{
    enum PixelFormat sws_fmt_in, sws_fmt_out;
    struct SwsContext *ctx;
    unsigned char *b = NULL;
    uint8_t *data_in[4], *data_out[4];
    int linesize[4], out_linesize[4];
    unsigned int bsize = 0;

    /* Resize the image */
    sws_fmt_in = cimage_format_to_PixelFormat(in->format);
    sws_fmt_out = cimage_format_to_PixelFormat(in->format);
    bsize = get_raw_size(in->format, new_width, new_height);
    b = calloc(bsize, sizeof(unsigned char));

    if (NULL == b)
        return -1;

    ctx = sws_getContext(in->raw.hdr.width, in->raw.hdr.height, sws_fmt_in,
                         new_width, new_height, sws_fmt_out,
                         SWS_BICUBIC, 0, 0, 0);

    if (NULL == ctx)
        return -1;

    av_image_fill_linesizes(linesize, sws_fmt_in, in->raw.hdr.width);
    av_image_fill_linesizes(out_linesize, sws_fmt_out, new_width);
    av_image_fill_pointers(data_in, sws_fmt_in, in->raw.hdr.height,
                           (uint8_t *)in->raw.headless, linesize);

    av_image_fill_pointers(data_out, sws_fmt_out, new_height, b, out_linesize);
    sws_scale(ctx, (const uint8_t * const *)data_in, linesize, 0, new_height,
              data_out, out_linesize);

    sws_freeContext(ctx);

    /* And fill the cimage_t with its new content */
    if (fill_raw_image(out, b, in->raw.hdr.format, new_width, new_height,
                       CIMAGE_FILL_OWNER) < 0)
    {
        return -1;
    }

    return 0;
}

int raw_extract(cimage_s *out, cimage_s *in, int x, int y, int w, int h)
{
    unsigned char *b = NULL;
    unsigned int bsize = 0;

    /* Extract the RAW image area */

    /* And fill the cimage_t with its new content */
    if (fill_raw_image(out, b, in->raw.hdr.format, w, h,
                       CIMAGE_FILL_OWNER) < 0)
    {
        return -1;
    }

    return 0;
}

/*
 *
 * Public API
 *
 */

/*
 * Our "real" image format conversion routine. ;-)
 */
__PUB_API__ unsigned char *craw_cvt_format(const unsigned char *buffer,
    enum cimage_color_format fmt_in, unsigned int width, unsigned int height,
    enum cimage_color_format fmt_out, unsigned int *bsize)
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

