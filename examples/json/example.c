
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Mar 17 09:52:00 2016
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
    fprintf(stdout, "Usage: json [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "JSON API.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -f [file name]\tIndicate the JSON file to handle.\n");
    fprintf(stdout, "\n");
}

int main(int argc, char **argv)
{
    const char *opt = "f:h";
    int option;
    char *filename = NULL;
    cjson_t *j = NULL;
    cstring_t *s = NULL;

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
        fprintf(stderr, "There must be a valid JSON file name.\n");
        return -1;
    }

    j = cjson_read_file(filename);

    if (NULL == j) {
        fprintf(stderr, "Error: %s\n", cstrerror(cget_last_error()));
        return -1;
    }

    s = cjson_to_cstring(j, true);

    if (s != NULL) {
        fprintf(stdout, "%s\n", cstring_valueof(s));
        cstring_destroy(s);
    }

    cjson_delete(j);

    if (filename != NULL)
        free(filename);

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

