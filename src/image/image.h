
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 14 11:05:27 2017
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

#ifndef _COLLECTIONS_IMAGE_H
#define _COLLECTIONS_IMAGE_H        1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <image.h> directly; include <collections.h> instead."
# endif
#endif

/* To the main image manipulation we use the OpenCv */
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

/* To do any kind of RAW format conversion we use the libswscale */
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

/* To do image annotations */
#include <ft2build.h>
#include FT_FREETYPE_H

/* Known file extensions */
#define EXT_RAW                     "raw"
#define EXT_PPM                     "ppm"
#define EXT_TIFF                    "tiff"
#define EXT_JPG2K                   "jp2"
#define EXT_PNG                     "png"
#define EXT_BMP                     "bmp"
#define EXT_JPG                     "jpg"

/* A RAW image file header */
struct raw_header {
    unsigned int        id;
    unsigned int        size;
    unsigned int        width;
    unsigned int        height;

    /* Duplicated information, since it goes into the file */
    unsigned int        format;
};

struct raw_image {
    unsigned char       *original;  /** The original RAW image */
    unsigned char       *headless;  /** The RAW image without the header */
    struct raw_header   hdr;
};

#define cimage_members                                      \
    cl_struct_member(IplImage *, image)                     \
    cl_struct_member(struct raw_image, raw)                 \
    cl_struct_member(enum cimage_format, format)            \
    cl_struct_member(enum cimage_type, type)                \
    cl_struct_member(enum cimage_fill_format, fill_format)  \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(cimage_s, cimage_members);

#define cimage_s                    cl_struct(cimage_s)
#define RAW_ID                      cl_fourcc('R', 'A', 'W', '1')

/* cvt.c */
unsigned char *convert_image_formats(cimage_s *image, enum cimage_type type,
                                     enum cimage_format format,
                                     unsigned int *bsize);

unsigned char *convert_raw_formats(cimage_s *image, enum cimage_format format,
                                   unsigned int *bsize);

/* raw.c */
int raw_load(const char *filename, cimage_s *image);
int raw_save_to_mem(const cimage_s *image, unsigned char **buffer,
                    unsigned int *bsize);

int raw_save(const cimage_s *image, const char *filename);
unsigned char *RAW_to_jpg_mem(const unsigned char *buffer, int width,
                              int height, bool color, int quality,
                              unsigned int *jsize);

unsigned char *jpg_to_RAW_mem(const unsigned char *buffer, unsigned int jsize,
                              int *width, int *height, bool *color);

int fill_raw_image(cimage_s *image, const unsigned char *buffer,
                   enum cimage_format format, unsigned int width,
                   unsigned int height, enum cimage_fill_format fill_format);

/* utils.c */
enum cimage_type cimage_detect_type(const unsigned char *buffer,
                                    unsigned int bsize);

enum cimage_type cimage_detect_type_from_file(const char *filename);
enum PixelFormat cimage_format_to_PixelFormat(enum cimage_format fmt);
bool has_internal_image(cimage_s *image);
int get_channels_by_format(enum cimage_format format);
char *cimage_type_to_extension(enum cimage_type type);
bool is_known_extension(const char *filename);

#endif

