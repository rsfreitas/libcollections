
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
                    "be filled inside a cl_image_t.\n");

    fprintf(stdout, "  -l [file name]\tIndicate the image file name "
                    "to be loaded.\n");

    fprintf(stdout, "  -e [file name]\tThe exported file name.\n");
    fprintf(stdout, "  -T [type]\tThe exported image type.\n");
    fprintf(stdout, "\n");
}

static void print_image_info(cl_image_t *image)
{
    fprintf(stdout, "Image type: %d\n", cl_image_type(image));
    fprintf(stdout, "Image format: %d\n", cl_image_color_format(image));
    fprintf(stdout, "Resolution: %dx%d\n", cl_image_width(image),
            cl_image_height(image));

    fprintf(stdout, "Number of channels: %d\n", cl_image_channels(image));
    fprintf(stdout, "Size in bytes: %d\n", cl_image_size(image));
}

static cl_image_t *load_image_file(const char *filename)
{
    cl_image_t *image;

    image = cl_image_load_from_file(filename);

    if (NULL == image) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
        return NULL;
    }

    /* print image info */
    print_image_info(image);

    return image;
}

static cl_image_t *fill_image_with_file(const char *filename)
{
    cl_image_t *image;
    unsigned char *buffer;
    unsigned int bsize;

    image = cl_image_create();

    if (NULL == image) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
        return NULL;
    }

    /* load the RAW image */
    buffer = cl_fload(filename, &bsize);

    if (NULL == buffer) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
        return NULL;
    }

    cl_image_fill(image, buffer, bsize, CL_IMAGE_FMT_YUYV, 640, 480,
                  CL_IMAGE_FILL_OWNER);

    print_image_info(image);

    return image;
}

static void export_image(cl_image_t *image, const char *filename,
    enum cl_image_type type)
{
    enum cl_image_color_format fmt;

    if (NULL == image)
        return;

    fmt = cl_image_color_format(image);

    if ((fmt != CL_IMAGE_FMT_BGR) || (fmt != CL_IMAGE_FMT_RGB) ||
        (fmt != CL_IMAGE_FMT_GRAY))
    {
        /* Needs to convert */
    }

    if (cl_image_save_to_file(image, filename, type) < 0) {
        fprintf(stderr, "%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
        return;
    }
}

int main(int argc, char **argv)
{
    const char *opt = "f:hl:T:e:";
    int option;
    bool load = true, export = false;
    cl_image_t *image = NULL;
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

    cl_init(NULL);

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
        cl_image_unref(image);

    cl_uninit();

    /* This makes valgrind report no memory leaks. */
    cl_exit();

    return 0;
}

