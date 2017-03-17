
/*
 * Description: Example showing the 'image' API from libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Dec 20 20:05:48 2015
 * Project: examples
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <collections.h>

static void help(void)
{
    fprintf(stdout, "Usage: image[OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "configuration file API.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -f [file name]\tIndicate the image file to "
                    "be filled inside a cimage_t.\n");

    fprintf(stdout, "  -l [file name]\tIndicate the image file name "
                    "to be loaded.\n");

    fprintf(stdout, "  -e [file name]\tThe exported file name.\n");
    fprintf(stdout, "  -T [type]\tThe exported image type.\n");
    fprintf(stdout, "\n");
}

static void print_image_info(cimage_t *image)
{
    fprintf(stdout, "Image type: %d\n", cimage_type(image));
    fprintf(stdout, "Image format: %d\n", cimage_color_format(image));
    fprintf(stdout, "Resolution: %dx%d\n", cimage_width(image),
            cimage_height(image));

    fprintf(stdout, "Number of channels: %d\n", cimage_channels(image));
    fprintf(stdout, "Size in bytes: %d\n", cimage_size(image));
}

static cimage_t *load_image_file(const char *filename)
{
    cimage_t *image;

    image = cimage_load_from_file(filename);

    if (NULL == image) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cstrerror(cget_last_error()));
        return NULL;
    }

    /* print image info */
    print_image_info(image);

    return image;
}

static cimage_t *fill_image_with_file(const char *filename)
{
    cimage_t *image;
    unsigned char *buffer;
    unsigned int bsize;

    image = cimage_create();

    if (NULL == image) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cstrerror(cget_last_error()));
        return NULL;
    }

    /* load the RAW image */
    buffer = cfload(filename, &bsize);

    if (NULL == buffer) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cstrerror(cget_last_error()));
        return NULL;
    }

    cimage_fill(image, buffer, bsize, CIMAGE_FMT_YUYV, 640, 480,
                CIMAGE_FILL_OWNER);

    print_image_info(image);

    return image;
}

static void export_image(cimage_t *image, const char *filename,
    enum cl_image_type type)
{
    enum cl_image_color_format fmt;

    if (NULL == image)
        return;

    fmt = cimage_color_format(image);

    if ((fmt != CIMAGE_FMT_BGR) || (fmt != CIMAGE_FMT_RGB) ||
        (fmt != CIMAGE_FMT_GRAY))
    {
        /* Needs to convert */
    }

    if (cimage_save_to_file(image, filename, type) < 0) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cstrerror(cget_last_error()));
        return;
    }
}

int main(int argc, char **argv)
{
    const char *opt = "f:hl:T:e:";
    int option;
    bool load = true, export = false;
    cimage_t *image = NULL;
    char *filename = NULL, *out_filename = NULL;
    enum cl_image_type type = -1;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'e':
                export = true;
                out_filename = strdup(optarg);
                break;

            case 'f':
                load = false;
                filename = strdup(optarg);
                break;

            case 'l':
                load = true;
                filename = strdup(optarg);
                break;

            case 'T':
                type = atoi(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == filename) {
        fprintf(stderr, "There must be a valid cfg file name.\n");
        return -1;
    }

    collections_init(NULL);

    if (load == true)
        image = load_image_file(filename);
    else
        image = fill_image_with_file(filename);

    if (export == true)
        export_image(image, out_filename, type);

    if (filename != NULL)
        free(filename);

    if (out_filename != NULL)
        free(out_filename);

    if (image != NULL)
        cimage_unref(image);

    collections_uninit();

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

