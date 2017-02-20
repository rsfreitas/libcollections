
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

#include <ft2build.h>
#include FT_FREETYPE_H

#include "collections.h"

#define caption_members                                         \
    cl_struct_member(unsigned int, size)                        \
    cl_struct_member(unsigned int, max_horizontal_bearing_y)    \
    cl_struct_member(FT_Library, library)                       \
    cl_struct_member(FT_Face, face)                             \
    cl_struct_member(FT_GlyphSlot, slot)                        \
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

static CvScalar caption_color_to_CvScalar(enum caption_color color,
    enum cimage_format format)
{
    double r, g, b;

    r = (color & 0xFF0000) >> 16;
    g = (color & 0xFF00) >> 8;
    b = color & 0xFF;

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

static bool validate_text_position(cimage_t *image, unsigned int x,
    unsigned int y)
{
    if ((x >= (unsigned int)cimage_width(image)) ||
        (y >= (unsigned int)cimage_height(image)))
    {
        return false;
    }

    return true;
}

/* FIXME */
static void draw_char(caption_s *caption, unsigned int x, unsigned int y,
    CvScalar color, cimage_t *image)
{
    FT_Bitmap *bitmap = &caption->slot->bitmap;

    if (validate_text_position(image, x, y) == false)
        return;

    x += caption->slot->bitmap_left;
    y += caption->max_horizontal_bearing_y / 64 -
                caption->slot->metrics.horiBearingY / 64;

    draw_onto_the_image(image, x, y, bitmap->buffer, 0,
                        bitmap->width, bitmap->rows, color);
}

/* FIXME */
static int add_caption(caption_s *caption, cimage_t *image, unsigned int x,
    unsigned int y, enum caption_color color, const char *text)
{
    CvScalar sc_color;
    FT_ULong c;
    size_t len = strlen(text);
    unsigned int i;

    sc_color = caption_color_to_CvScalar(color, cimage_format(image));

    for (i = 0; i < len; i++) {
        c = utf8_to_unicode(text, (int *)&i);

        if (FT_Load_Char(caption->face, c, FT_LOAD_RENDER))
            continue;

        draw_char(caption, x, y, sc_color, image);
        x += caption->slot->advance.x >> 8;
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

/*
 *
 * Public API
 *
 */

__PUB_API__ cimage_caption_t *caption_ref(cimage_caption_t *caption)
{
    caption_s *c = (caption_s *)caption;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, NULL);
    cref_inc(&c->ref);

    return caption;
}

__PUB_API__ int caption_unref(cimage_caption_t *caption)
{
    caption_s *c = (caption_s *)caption;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    cref_dec(&c->ref);

    return 0;
}

__PUB_API__ cimage_caption_t *caption_configure(const char *ttf_pathname,
    unsigned int font_size)
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

    return c;
}

__PUB_API__ int caption_destroy(cimage_caption_t *caption)
{
    return caption_unref(caption);
}

__PUB_API__ int cimage_add_caption_vf(cimage_caption_t *caption,
    cimage_t *image, unsigned int x, unsigned int y, enum caption_color color,
    const char *fmt, va_list ap)
{
    char *text = NULL;
    int ret;

    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    vasprintf(&text, fmt, ap);
    ret = add_caption(caption, image, x, y, color, text);

    if (text != NULL)
        free(text);

    return ret;
}

__PUB_API__ int cimage_add_caption_f(cimage_caption_t *caption, cimage_t *image,
    unsigned int x, unsigned int y, enum caption_color color,
    const char *fmt, ...)
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
    ret = cimage_add_caption_vf(caption, image, x, y, color, fmt, ap);
    va_end(ap);

    return ret;
}

__PUB_API__ int cimage_add_caption(cimage_caption_t *caption, cimage_t *image,
    unsigned int x, unsigned int y, enum caption_color color, const char *text)
{
    __clib_function_init__(true, caption, CIMAGE_CAPTION, -1);
    __clib_function_init__(true, image, CIMAGE, -1);

    if (NULL == text) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return cimage_add_caption_f(caption, image, x, y, color, "%s", text);
}

