
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Mar 15 00:21:32 2017
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "collections.h"

int main(int argc, char **argv)
{
    const char *opt = "hf:\0";
    int option;
    char *filename = NULL;
    cl_image_t *in, *out;
    cl_caption_t *cap;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                return 1;

            case 'f':
                filename = strdup(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    cl_init(NULL);
    in = cl_image_load_from_file(filename);

    if (NULL == in) {
        printf("Error: %s\n", cl_strerror(cl_get_last_error()));
        return -1;
    }

    cap = cl_caption_configure("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf",
                            15, CL_IMAGE_COLOR_BLACK, CL_IMAGE_COLOR_WHITE);
                           // 15, CL_IMAGE_COLOR_WHITE, CL_IMAGE_COLOR_BLACK);

    cl_caption_add(cap, in, true, "Only a minor test");
    cl_caption_add(cap, in, true, "Only a minor test 2");
    cl_caption_add(cap, in, false, "Only a minor test 3");
    cl_caption_add(cap, in, false, "Only a minor test 4: special chars éáçíúãõêàó");
    cl_caption_destroy(cap);
    cl_image_save_to_file(in, "teste.jpg", CL_IMAGE_JPG);
    cl_image_destroy(in);

    if (filename != NULL)
        free(filename);

    cl_uninit();

    return 0;
}

