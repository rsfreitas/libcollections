
/*
 * Description: Example showing the datetime API from libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Jan 19 10:33:58 2016
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
#include <unistd.h>

#include "collections.h"

int main(int argc, char **argv)
{
    const char *opt = "m:\0";
    int option, m = 0;
    cdatetime_t *dt, *p;
    cstring_t *s;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'm':
                m = atoi(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    dt = cdt_mktime(2016, 3, 30, 10, 30, 40);
    s = cdt_to_string(dt, "%Y/%m/%d %H:%M:%S");
    printf("%s: %s\n", __FUNCTION__, cstring_valueof(s));
    cstring_destroy(s);

    p = cdt_minus_months(dt, m);
    s = cdt_to_string(p, "%Y/%m/%d %H:%M:%S");
    printf("%s: %s\n", __FUNCTION__, cstring_valueof(s));
    cstring_destroy(s);

    cdt_destroy(dt);

    return 0;
}

