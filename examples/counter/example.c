
/*
 * Description: Example showing the 'counter' API on libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Jan 13 11:12:42 2016
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

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "collections.h"

int main(int argc, char **argv)
{
    const char *opt = "v:c:i:a:\0";
    int option;
    cl_counter_t *c;
    char *tmp;
    long long v = 0, cv = 0, min = 0, max = 0;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'c':
                cv = atoll(optarg);
                break;

            case 'v':
                v = atoll(optarg);
                break;

            case 'i':
                min = atoll(optarg);
                break;

            case 'a':
                max = atoll(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    cl_init(NULL);
    c = cl_counter_create(CL_8BIT_COUNTER, -10, 110, cv, true);
    tmp = cl_bool_to_string(cl_counter_lt(c, v));
    printf("%s: %lld < %lld: %s\n", __FUNCTION__, cl_counter_get(c),
            v, tmp);

    printf("%s: error = %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
    free(tmp);
    cl_counter_destroy(c);
    cl_uninit();

    return 0;
}

