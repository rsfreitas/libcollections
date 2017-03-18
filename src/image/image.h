
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

#define cl_image_members                                        \
    cl_struct_member(IplImage *, image)                         \
    cl_struct_member(struct raw_image, raw)                     \
    cl_struct_member(enum cl_image_color_format, format)        \
    cl_struct_member(enum cl_image_type, type)                  \
    cl_struct_member(enum cl_image_fill_format, fill_format)    \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_image_s, cl_image_members);

#define cl_image_s                  cl_struct(cl_image_s)
#define RAW_ID                      cl_fourcc('R', 'A', 'W', '1')

/* cvt.c */
unsigned char *convert_image_formats(cl_image_s *image, enum cl_image_type type,
                                     enum cl_image_color_format format,
                                     unsigned int *bsize);

unsigned char *convert_raw_formats(cl_image_s *image,
                                   enum cl_image_color_format format,
                                   unsigned int *bsize);

IplImage *cl_image_to_ocv(cl_image_s *image);

/* raw.c */
int raw_load(const char *filename, cl_image_s *image);
int raw_save_to_mem(const cl_image_s *image, unsigned char **buffer,
                    unsigned int *bsize);

int raw_save(const cl_image_s *image, const char *filename);
unsigned char *RAW_to_jpg_mem(const unsigned char *buffer, int width,
                              int height, bool color, int quality,
                              unsigned int *jsize);

unsigned char *jpg_to_RAW_mem(const unsigned char *buffer, unsigned int jsize,
                              int *width, int *height, bool *color);

int fill_raw_image(cl_image_s *image, const unsigned char *buffer,
                   enum cl_image_color_format format, unsigned int width,
                   unsigned int height, enum cl_image_fill_format fill_format);

int raw_resize(cl_image_s *out, cl_image_s *in, unsigned int new_width,
               unsigned int new_height);

/* utils.c */
enum cl_image_type cl_image_detect_type(const unsigned char *buffer,
                                        unsigned int bsize);

enum cl_image_type cl_image_detect_type_from_file(const char *filename);
enum PixelFormat cl_image_format_to_PixelFormat(enum cl_image_color_format fmt);
bool has_internal_image(cl_image_s *image);
int get_channels_by_format(enum cl_image_color_format format);
char *cl_image_type_to_extension(enum cl_image_type type);
bool is_known_extension(const char *filename);
bool is_supported_image_type(enum cl_image_type type);
bool is_supported_color_format(enum cl_image_color_format fmt);
bool is_supported_fill_format(enum cl_image_fill_format fmt);

#endif

