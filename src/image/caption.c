
/*
 * Description: API to add text to images.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Feb  8 10:41:41 2017
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

#include <stdarg.h>

#include "collections.h"
#include "image.h"

#define CAPTION_HEIGHT_BORDER           3
#define INITIAL_CAPTION_X               0

struct font_color {
    CvScalar        foreground;
    CvScalar        background;
};

#define caption_members                                         \
    cl_struct_member(unsigned int, size)                        \
    cl_struct_member(unsigned int, max_horizontal_bearing_y)    \
    cl_struct_member(FT_Library, library)                       \
    cl_struct_member(FT_Face, face)                             \
    cl_struct_member(FT_GlyphSlot, slot)                        \
    cl_struct_member(struct font_color, gray_font)              \
    cl_struct_member(struct font_color, rgb_font)               \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(caption_s, caption_members);

#define caption_s       cl_struct(caption_s)

/*
 * Calculates the maximum top side bearing for horizontal Y from the
 * valid ASCII characters and some unicode special ones.
 */
static void estimate_font_dimensions(caption_s *caption)
{
    FT_ULong c;
    unsigned int limits[2][2] = {{0x21, 0x7F}, {0xC0, 0xFF}}, i;

    for (i = 0; i < 2; i++)
        for (c = limits[i][0]; c < limits[i][1]; c++) {
            if (FT_Load_Char(caption->face, c, FT_LOAD_RENDER))
                continue;

            if (caption->slot->metrics.horiBearingY >
                (int)caption->max_horizontal_bearing_y)
            {
                caption->max_horizontal_bearing_y =
                        caption->slot->metrics.horiBearingY;
            }
        }
}

/*
 * Releases the freetype objects.
 */
static void ft_uninit(caption_s *caption)
{
    FT_Done_Face(caption->face);
    FT_Done_FreeType(caption->library);
}

/*
 * Do the initialization required by the freetype library.
 */
static int ft_init(caption_s *caption, const char *pathname, unsigned int size)
{
    if (FT_Init_FreeType(&caption->library) != 0)
        return -1;

    if (FT_New_Face(caption->library, pathname, 0, &caption->face) != 0)
        goto library_error_block;

    if (FT_Select_Charmap(caption->face, FT_ENCODING_UNICODE) != 0)
        goto face_error_block;

    if (FT_Set_Pixel_Sizes(caption->face, 0, size) != 0)
        goto face_error_block;

    caption->slot = caption->face->glyph;
    caption->size = size;
    estimate_font_dimensions(caption);

    return 0;

face_error_block:
    FT_Done_Face(caption->face);

library_error_block:
    FT_Done_FreeType(caption->library);

    return -1;
}

/*
 * Convert an internal color representation to its hexadecimal color code.
 */
unsigned int to_hex_color(enum cimage_color color)
{
    switch (color) {
        case CIMAGE_COLOR_BLACK:
            return 0x000000;

        case CIMAGE_COLOR_WHITE:
            return 0xFFFFFF;

        case CIMAGE_COLOR_GREY:
            return 0x7F7F7F;

        case CIMAGE_COLOR_BLUE:
            return 0x00007F;

        case CIMAGE_COLOR_RED:
            return 0x7F0000;

        case CIMAGE_COLOR_GREEN:
            return 0x007F00;

        case CIMAGE_COLOR_YELLOW:
            return 0xFFFF00;

        default:
            break;
    }

    return 0x000000F;
}

/*
 * Converts an internal color representation to a CvScalar type.
 */
static CvScalar caption_color_to_CvScalar(enum cimage_color color,
    enum cimage_color_format format)
{
    unsigned int c;
    double r, g, b;

    c = to_hex_color(color);
    r = (c & 0xFF0000) >> 16;
    g = (c & 0xFF00) >> 8;
    b = c & 0xFF;

    switch (format) {
        case CIMAGE_FMT_GRAY:
            return cvScalar(r * 0.299f + g * 0.587f + b * 0.114f,
                            0.0f, 0.0f, 0.0f);

        case CIMAGE_FMT_RGB:
            return cvScalar(r, g, b, 0.0f);

        default:
            break;
    }

    return cvScalar(0.0f, 0.0f, 0.0f, 0.0f);
}

/*
 * Converts a UTF-8 character to a unicode one. If it's unknown character
 * returns a '?'.
 */
static FT_ULong utf8_to_unicode(const char *text, int *index)
{
    FT_ULong c;

    if ((unsigned char)text[*index] < 0x7F)
        c = text[*index];
    else if (((unsigned char)text[*index] & 0xE0) == 0xC0) {
        c = ((unsigned int)(text[*index] & 0x1F) << 6) +
             (unsigned int)(text[*index + 1] & 0x3F);

        (*index)++;
    } else
        c = '?';

    return c;
}

/*
 * Does the character drawing into the image.
 */
static void draw_onto_the_image(FT_Bitmap *bitmap, CvScalar color,
    cimage_s *image, unsigned int x, unsigned int y, unsigned int max_width,
    unsigned int max_height)
{
    char *dest_image;
    unsigned char *font_buffer;
    unsigned int font_ptr, img_ptr, char_ptr, i, j;
    int font_width_step, image_width_step, k;

    dest_image = image->image->imageData + y * image->image->widthStep + x *
                 image->image->nChannels;

    image_width_step = image->image->widthStep -
                       (max_width * image->image->nChannels);

    font_buffer = bitmap->buffer;
    font_width_step = bitmap->width - max_width;

    for (i = 0; i < max_height; i++) {
        for (j = 0; j < max_width; j++) {
            for (k = 0; k < image->image->nChannels; k++) {
                font_ptr = (unsigned char)*font_buffer;
                img_ptr = (unsigned char)*dest_image;
                char_ptr = (unsigned char)color.val[k];

                *dest_image = (font_ptr * char_ptr +
                               (255 - font_ptr) * img_ptr) / 255.0f;

                dest_image++;
            }

            font_buffer++;
        }

        dest_image += image_width_step;
        font_buffer += font_width_step;
    }
}

/*
 * Prepares the character image limits and call the function to draw it.
 */
static void draw_char(caption_s *caption, unsigned int x, unsigned int y,
    CvScalar color, cimage_s *image)
{
    FT_Bitmap *bitmap = &caption->slot->bitmap;
    int max_width, max_height;

    if ((x >= (unsigned int)cimage_width(image)) ||
        (y >= (unsigned int)cimage_height(image)))
    {
        return;
    }

    if (bitmap->rows > (image->image->height - (int)y))
        max_height = image->image->height -y;
    else
        max_height = bitmap->rows;

    if (bitmap->width > (image->image->width - (int)x))
        max_width = image->image->width - x;
    else
        max_width = bitmap->width;

    draw_onto_the_image(bitmap, color, image, x, y, max_width, max_height);
}

/*
 * Adds a text caption into an image.
 */
static int add_caption(caption_s *caption, cimage_s *image, unsigned int x,
    unsigned int y, CvScalar color, const char *text)
{
    FT_ULong c;
    size_t len = strlen(text);
    unsigned int i;

    for (i = 0; i < len; i++) {
        c = utf8_to_unicode(text, (int *)&i);

        if (FT_Load_Char(caption->face, c, FT_LOAD_RENDER))
            continue;

        draw_char(caption, x + caption->slot->bitmap_left,
                  y + caption->max_horizontal_bearing_y / 64 -
                  caption->slot->metrics.horiBearingY / 64, color, image);

        x += caption->slot->advance.x >> 6;
    }

    return -1;
}

static void destroy_caption(const struct cref_s *ref)
{
    caption_s *caption = cl_container_of(ref, caption_s, ref);

    if (NULL == caption)
        return;

    ft_uninit(caption);
    free(caption);
    caption = NULL;
}

static caption_s *new_caption(void)
{
    caption_s *c = NULL;

    c = calloc(1, sizeof(caption_s));

    if (NULL == c) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    /* Initialize the reference count */
    c->ref.free = destroy_caption;
    c->ref.count = 1;

    set_typeof(CIMAGE_CAPTION, c);

    return c;
}

static cimage_t *create_background_caption(cimage_s *image,
    struct font_color *font, caption_s *caption)
{
    cimage_t *img;
    IplImage *background = NULL;
    int w = 0, h = 0;

    w = cimage_width(image);
    h = caption->size + CAPTION_HEIGHT_BORDER;
    img = cimage_create();
    background = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U,
                               cimage_channels(image));

    cvSet(background, font->background, NULL);
    cimage_cv_import(img, background, CIMAGE_JPG);

    return img;
}

static void join_images(cimage_s *image, cimage_s *caption, bool append)
{
    IplImage *ipl_image, *ipl_caption, *final;

    ipl_image = cimage_to_ocv(image);
    ipl_caption = cimage_cv_export(caption);

    cvResetImageROI(ipl_image);
    final = cvCreateImage(cvSize(ipl_image->width,
                                 ipl_image->height + ipl_caption->height),
                          ipl_image->depth, ipl_image->nChannels);

    cvZero(final);

    if (append) {
        cvSetImageROI(final, cvRect(0, 0, ipl_image->width, ipl_image->height));
        cvCopy(ipl_image, final, NULL);
        cvResetImageROI(final);
        cvSetImageROI(final, cvRect(0, ipl_image->height, ipl_caption->width,
                                    ipl_caption->height));

        cvCopy(ipl_caption, final, NULL);
    } else {
        cvSetImageROI(final, cvRect(0, 0, ipl_caption->width,
                                    ipl_caption->height));

        cvCopy(ipl_caption, final, NULL);
        cvResetImageROI(final);
        cvSetImageROI(final, cvRect(0, ipl_caption->height, ipl_image->width,
                                    ipl_image->height));

        cvCopy(ipl_image, final, NULL);
    }

    cvResetImageROI(final);
    cimage_cv_import(image, final, CIMAGE_JPG);
}

/*
 *
 * Public API
 *
 */

__PUB_API__ caption_t *caption_ref(caption_t *caption)
{
    caption_s *c = (caption_s *)caption;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, NULL);
    cref_inc(&c->ref);

    return caption;
}

__PUB_API__ int caption_unref(caption_t *caption)
{
    caption_s *c = (caption_s *)caption;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    cref_dec(&c->ref);

    return 0;
}

__PUB_API__ caption_t *caption_configure(const char *ttf_pathname,
    unsigned int font_size, enum cimage_color foreground,
    enum cimage_color background)
{
    caption_s *c = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    c = new_caption();

    if (NULL == c)
        return NULL;

    if (ft_init(c, ttf_pathname, font_size) < 0) {
        caption_unref(c);
        return NULL;
    }

    /* Caption colors */
    c->gray_font.foreground = caption_color_to_CvScalar(foreground,
                                                         CIMAGE_FMT_GRAY);

    c->gray_font.background = caption_color_to_CvScalar(background,
                                                         CIMAGE_FMT_GRAY);

    c->rgb_font.foreground = caption_color_to_CvScalar(foreground,
                                                        CIMAGE_FMT_RGB);

    c->rgb_font.background = caption_color_to_CvScalar(background,
                                                        CIMAGE_FMT_RGB);

    return c;
}

__PUB_API__ int caption_destroy(caption_t *caption)
{
    return caption_unref(caption);
}

__PUB_API__ int caption_addvf(caption_t *caption, cimage_t *image,
    bool append, const char *fmt, va_list ap)
{
    caption_s *c = (caption_s *)caption;
    char *text = NULL;
    int ret;
    cimage_t *caption_image;
    struct font_color *font;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (cimage_color_format(image) == CIMAGE_FMT_GRAY)
        font = &c->gray_font;
    else
        font = &c->rgb_font;

    /*
     * Create the background image, choose between gray or rgb according
     * @image->format
     */
    caption_image = create_background_caption(image, font, caption);
    vasprintf(&text, fmt, ap);
    ret = add_caption(caption, caption_image, 0, 0, font->foreground, text);

    if (text != NULL)
        free(text);

    join_images(image, caption_image, append);
    cimage_unref(caption_image);

    return ret;
}

__PUB_API__ int caption_addf(caption_t *caption, cimage_t *image,
    bool append, const char *fmt, ...)
{
    va_list ap;
    int ret;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    ret = caption_addvf(caption, image, append, fmt, ap);
    va_end(ap);

    return ret;
}

__PUB_API__ int caption_add(caption_t *caption, cimage_t *image,
    bool append, const char *text)
{
    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == text) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return caption_addf(caption, image, append, "%s", text);
}

