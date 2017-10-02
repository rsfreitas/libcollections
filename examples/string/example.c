
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Mar 16 00:05:24 BRT 2017
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
    const char *opt = "h\0";
    int option, ret;
    cl_string_t *s;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                return 1;

            case '?':
                return -1;
        }
    } while (option != -1);

    cl_init(NULL);

    /* cl_string_rplsubstr */
    {
        s = cl_string_create("1 2 3 4 1 2 3 1 2 3 1");
        printf("%s: '%s'\n", __FUNCTION__, cl_string_valueof(s));
        ret = cl_string_rplsubstr(s, "1 2", "5 6");
        printf("%s: '%s' - ret = %d\n", __FUNCTION__, cl_string_valueof(s), ret);
        cl_string_unref(s);
    }

    /* cl_string_rplchr */
    {
        s = cl_string_create("1 2 3 4 1 2 3 1 2 3 1");
        printf("%s: '%s'\n", __FUNCTION__, cl_string_valueof(s));
        ret = cl_string_rplchr(s, '1', '8');
        printf("%s: '%s' - ret = %d\n", __FUNCTION__, cl_string_valueof(s), ret);
        cl_string_unref(s);
    }

    /* cl_string_split */
    {
        cl_stringlist_t *l = NULL;
        cl_string_t *tmp = NULL;
        int i;

        s = cl_string_create("one two three four five six seven eight nine ten");
        l = cl_string_split(s, " ");

        for (i = 0; i < cl_stringlist_size(l); i++) {
            tmp = cl_stringlist_get(l, i);
            printf("%d -> %s\n", i, cl_string_valueof(tmp));
            cl_string_unref(tmp);
        }

        tmp = cl_stringlist_flat(l, '|');
        printf("flat: '%s'\n", cl_string_valueof(tmp));
        cl_string_unref(tmp);
        cl_stringlist_destroy(l);
    }

    /* cl_string_is_float_number */
    {
        s = cl_string_create("5.25");
        printf("%s: %d\n", __FUNCTION__, cl_string_is_float_number(s));
        cl_string_unref(s);
        s = cl_string_create("525");
        printf("%s: %d\n", __FUNCTION__, cl_string_is_float_number(s));
        cl_string_unref(s);
        s = cl_string_create("525a");
        printf("%s: %d\n", __FUNCTION__, cl_string_is_float_number(s));
        cl_string_unref(s);
        s = cl_string_create("");
        printf("%s: %d\n", __FUNCTION__, cl_string_is_float_number(s));
        cl_string_unref(s);
    }

    /* cl_string_dup */
    {
        cl_string_t *d = NULL;

        s = cl_string_create("test of string");
        d = cl_string_dup(s);
        cl_string_unref(s);
        cl_string_unref(d);
    }

    /* cl_string_reverse */
    {
        s = cl_string_create("This is a very long string just used to test, "
                             "the reverse algorithm");

        printf("%s: %s\n", __FUNCTION__, cl_string_valueof(s));
        cl_string_reverse(s);
        printf("%s: %s\n", __FUNCTION__, cl_string_valueof(s));
        cl_string_unref(s);
    }

    cl_uninit();

    return 0;
}

