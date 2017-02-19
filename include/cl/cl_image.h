
/*
 * Description:
 * Author: Rodrigo Freitas
 * Created at: Fri Sep 23 14:56:09 2016
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

#ifndef _COLLECTIONS_CL_IMAGE_H
#define _COLLECTIONS_CL_IMAGE_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_image.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef __OPENCV_CORE_C_H__
# include <opencv2/core/core_c.h>
#endif

/** Supported image formats */
enum cimage_format {
    CIMAGE_FMT_UNKNOWN,
    CIMAGE_FMT_GRAY,
    CIMAGE_FMT_BGR,
    CIMAGE_FMT_RGB,
    CIMAGE_FMT_YUV422,
    CIMAGE_FMT_YUV420,
    CIMAGE_FMT_YUYV
};

/** Supported image types */
enum cimage_type {
    CIMAGE_RAW,
    CIMAGE_JPG,
    CIMAGE_BMP,
    CIMAGE_PNG,
    CIMAGE_JPG2K,
    CIMAGE_TIFF,
    CIMAGE_PPM
};

/**
 * @name cimage_ref
 * @brief Increases the reference count for a cimage_t item.
 *
 * @param [in,out] image: The cimage_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cimage_t *cimage_ref(cimage_t *image);

/**
 * @name cimage_unref
 * @brief Decreases the reference count for a cimage_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] image: The cimage_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_unref(cimage_t *image);

/**
 * @name cimage_create
 * @brief Creates an empty cimage_t object.
 *
 * This function creates an empty cimage_t object so the user may fill it with
 * any kind of image.
 *
 * @return On success returns a cimage_t object or NULL otherwise.
 */
cimage_t *cimage_create(void);

/**
 * @name cimage_destroy
 * @brief Releases a cimage_t object from the memory.
 *
 * This function will release the memory holded by a cimage_t object.
 *
 * @param [in,out] image: The cimage_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_destroy(cimage_t *image);

/**
 * @name cimage_fill
 * @brief Fills a cimage_t object with a memory image.
 *
 * This function will put a previously loaded image from a buffer into a
 * cimage_t object. It will try to guess what kind of image is been pointed
 * by \a buffer. If we're passing a CIMAGE_RAW image we may use a pure RAW
 * image, without our internal header. To do this, we must correctly inform
 * \a format, \a width and \a height from the RAW image.
 *
 * After doing this the \a buffer the user does not have to free it, the library
 * will do so.
 *
 * @param [in,out] image: The cimage_t object.
 * @param [in] buffer: The image buffer.
 * @param [in] bsize: The image buffer size.
 * @param [in] format: The image buffer color format, if we're passing a
 *                     CIMAGE_RAW.
 * @param [in] width: The image width.
 * @param [in] height: The image height.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_fill(cimage_t *image, const unsigned char *buffer,
                unsigned int bsize, enum cimage_format format,
                unsigned int width, unsigned int height);

/**
 * @name cimage_load
 * @brief Loads a memory image to a cimage_t object.
 *
 * This functions supports the following kind of images: JPG, JPG2000, PNG,
 * TIFF, PPM and RAW. And, just like cimage_fill, if we're passing a CIMAGE_RAW
 * buffer, we must pass a few more info about it.
 *
 * @param [in] buffer: The image buffer.
 * @param [in] bsize: The image buffer size.
 * @param [in] format The image buffer color format, if we're passing a
 *                    CIMAGE_RAW.
 * @param [in] width: The image width.
 * @param [in] height: The image height.
 *
 * @return On success returns a cimage_t object with the image or NULL otherwise.
 */
cimage_t *cimage_load(const unsigned char *buffer, unsigned int bsize,
                      enum cimage_format format, unsigned int width,
                      unsigned int height);

/**
 * @name cimage_load_from_file
 * @brief Loads a file to a cimage_t object.
 *
 * This functions supports the following kind of images: JPG, JPG2000, PNG,
 * TIFF, PPM and RAW.
 *
 * @param [in] filename: The file name which will be loaded.
 *
 * @return On success returns a cimage_t object with the image or NULL otherwise.
 */
cimage_t *cimage_load_from_file(const char *filename);

/**
 * @name cimage_save
 * @brief Saves a cimage_t to memory.
 *
 * This function may be used by the user if one wants to write the file itself
 * and not use the library function to it.
 *
 * The \a buffer is internally allocated and in case of a success execution the
 * user must free it.
 *
 * @param [in] image: The cimage_t object.
 * @param [out] buffer: The output buffer.
 * @param [out] bsize: The output buffer size.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_save(const cimage_t *image, unsigned char **buffer,
                unsigned int *bsize);

/**
 * @name cimage_save_to_file
 * @brief Saves a cimage_t to a file.
 *
 * This function uses the \a file_type as the real output type. With this, the
 * user is able to pass any kind of extension with the name.
 *
 * No type conversion is made by this function.
 *
 * @param [in] image: The cimage_t object.
 * @param [in] filename: The output file name.
 * @param [in] file_type: The real output file type.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_save_to_file(const cimage_t *image, const char *filename,
                        enum cimage_type file_type);

/**
 * @name cimage_dup
 * @brief Duplicates a cimage_t object.
 *
 * @param [in] image: The original cimage_t object.
 *
 * @return On success returns a new cimage_t object or NULL otherwise.
 */
cimage_t *cimage_dup(const cimage_t *image);

/**
 * @name cimage_resize
 * @brief Resizes a cimage_t image.
 *
 * @param [in] image: The original cimage_t object.
 * @param [in] width: The new width of the image.
 * @param [in] height: The new height of the image.
 *
 * @return On success returns a new cimage_t object with the resized image or
 *         NULL otherwise.
 */
cimage_t *cimage_resize(const cimage_t *image, unsigned int width,
                        unsigned int height);

/**
 * @name cimage_extract
 * @brief Extracts part of an image from a cimage_t object.
 *
 * This function will extract a rect from the image, with X and Y coordinates
 * with W width and H height.
 *
 * @param [in] image: The cimage_t object.
 * @param [in] x: The X position inside the image.
 * @param [in] y: The Y position inside the image.
 * @param [in] w: The W width of the extracted image.
 * @param [in] h: The H height of the extracted image.
 *
 * @return On success returns a new cimage_t object with the extracted image or
 *         NULL otherwise.
 */
cimage_t *cimage_extract(const cimage_t *image, unsigned int x,
                         unsigned int y, unsigned int w, unsigned int h);

/**
 * @name cimage_raw_export
 * @brief Exports the real image from a cimage_t object.
 *
 * This function export the real image helded by a cimage_t object. The user may
 * choose to do any kind of conversion with it, such as an image type conversion,
 * like a JPG to a PNG image or an image color format conversion, like a RGB
 * image to a grayscale one.
 *
 * @param [in] image: The cimage_t object.
 * @param [in] type: The desired image type.
 * @param [in] format: The desired image color format.
 * @param [out] bsize: The exported image buffer size.
 * @param [out] width: The exported image width.
 * @param [out] height: The exported image height.
 *
 * @return On success returns a pointer the real image, that the user must free
 *         it later, or NULL otherwise.
 */
unsigned char *cimage_raw_export(const cimage_t *image, enum cimage_type type,
                                 enum cimage_format format, unsigned int *bsize,
                                 unsigned int *width, unsigned int *height);

/**
 * @name cimage_raw_import
 * @brief Imports an image buffer to a cimage_t object.
 *
 * This is just another name to the cimage_fill function.
 *
 * If \a image already have an image inside it will be released before importing
 * the new one.
 *
 * @param [in] buffer: The image buffer.
 * @param [in] bsize: The image buffer size.
 * @param [in] format The image buffer color format, if we're passing a
 *                    CIMAGE_RAW.
 * @param [in] width: The image width.
 * @param [in] height: The image height.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_raw_import(cimage_t *image, const unsigned char *buffer,
                      unsigned int bsize, enum cimage_format format,
                      unsigned int width, unsigned int height);

/**
 * @name cimage_cv_export
 * @brief Exports the OpenCv image pointer.
 *
 * This function exports the OpenCv image pointer so the user is able to do
 * anything he may want with it.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the OpenCv image pointer or NULL otherwise.
 */
IplImage *cimage_cv_export(const cimage_t *image);

/**
 * @name cimage_cv_import
 * @brief Imports an OpenCv image pointer to the cimage_t object.
 *
 * This function allows replace the internal image pointer with an external
 * OpenCv image pointer.
 *
 * If \a image already have an image inside it will be released before importing
 * the new one.
 *
 * @param [in,out] image: The original cimage_t object.
 * @param [in] cv_image: The new OpenCv image.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cimage_cv_import(cimage_t *image, IplImage *cv_image);

/**
 * @name cimage_size
 * @brief Gets the image size in bytes.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the image size in bytes or -1 otherwise.
 */
int cimage_size(const cimage_t *image);

/**
 * @name cimage_width
 * @brief Gets the image width.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the image width or -1 otherwise.
 */
int cimage_width(const cimage_t *image);

/**
 * @name cimage_height
 * @brief Gets the image height.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the image height or -1 otherwise.
 */
int cimage_height(const cimage_t *image);

/**
 * @name cimage_type
 * @brief Gets the image type.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the image type or -1 otherwise.
 */
enum cimage_type cimage_type(const cimage_t *image);

/**
 * @name cimage_format
 * @brief Gets the image color format.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the image color format or -1 otherwise.
 */
enum cimage_format cimage_format(const cimage_t *image);

/**
 * @name cimage_channels
 * @brief Gets the number of channels of the image.
 *
 * @param [in] image: The cimage_t object.
 *
 * @return On success returns the number of channels or -1 otherwise.
 */
int cimage_channels(const cimage_t *image);

#endif

