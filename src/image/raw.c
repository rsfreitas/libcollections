
/*
 * Description: Functions to convert between RAW and JPG image format.
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

