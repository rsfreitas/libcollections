
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
                    "handle.\n");

    fprintf(stdout, "\n");
}

int main(int argc, char **argv)
{
    const char *opt = "f:h";
    int option;
    char *filename = NULL;
    cimage_t *image;
    cimage_caption_t *caption;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'f':
                filename = strdup(optarg);
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

    image = cimage_load_from_file(filename);

    if (NULL == image) {
        printf("%s: %d\n", __FUNCTION__, cget_last_error());
        return -1;
    }

    caption = caption_configure("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf", 10);

    printf("Image type: %d\n", cimage_type(image));

    cimage_add_caption(caption, image, 10, 10, CAPTION_WHITE, "teste teste teste");
    cimage_save_to_file(image, "teste.png", CIMAGE_PNG);

    caption_destroy(caption);
    cimage_destroy(image);

    if (filename != NULL)
        free(filename);

    collections_uninit();

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

