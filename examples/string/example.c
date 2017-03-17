
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
    cstring_t *s;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                return 1;

            case '?':
                return -1;
        }
    } while (option != -1);

    collections_init(NULL);

    /* cstring_rplsubstr */
    {
        s = cstring_create("1 2 3 4 1 2 3 1 2 3 1");
        printf("%s: '%s'\n", __FUNCTION__, cstring_valueof(s));
        ret = cstring_rplsubstr(s, "1 2", "5 6");
        printf("%s: '%s' - ret = %d\n", __FUNCTION__, cstring_valueof(s), ret);
        cstring_unref(s);
    }

    /* cstring_rplchr */
    {
        s = cstring_create("1 2 3 4 1 2 3 1 2 3 1");
        printf("%s: '%s'\n", __FUNCTION__, cstring_valueof(s));
        ret = cstring_rplchr(s, '1', '8');
        printf("%s: '%s' - ret = %d\n", __FUNCTION__, cstring_valueof(s), ret);
        cstring_unref(s);
    }

    collections_uninit();

    return 0;
}

