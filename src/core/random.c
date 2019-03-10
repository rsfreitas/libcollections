
/*
 * Description: Functions to handle pseudo-random numbers.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 12:33:09 2015
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

#include <stdlib.h>

#include "collections.h"

/*
 *
 * API
 *
 */

unsigned int cl_cseed(void)
{
    FILE *f;
    char tmp[64] = {0}, *p;

#ifdef GNU_LINUX
    f = popen("cat /proc/sys/kernel/random/uuid | cut -d '-' -f 2", "r");
#else
    f = popen("cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 16 | head -n 1",
              "r");
#endif

    if (NULL == f)
        return 0;

    p = fgets(tmp, sizeof(tmp) - 1, f);
    pclose(f);

    if (NULL == p)
        return 0;

    return strtol(tmp, NULL, 16);
}

unsigned int cl_seed(void)
{
    unsigned int x = 0;

    __clib_function_init__(false, NULL, -1, 0);
    x = cl_cseed();

    if (x == 0) {
        cset_errno(CL_NULL_DATA);
        return 0;
    }

    return x;
}

unsigned int cl_rand(unsigned int random_max)
{
    unsigned int num_bins, num_rand, bin_size, defect, x;

    __clib_function_init__(false, NULL, -1, -1);

    num_bins = (unsigned int)random_max + 1;
    num_rand = (unsigned int)RAND_MAX + 1;
    bin_size = num_rand / num_bins;
    defect = num_rand % num_bins;

    do {
#ifdef GNU_LINUX
        random_r(library_random_data(), (int32_t *)&x);
#else
        x = random();
#endif
    } while (num_rand - defect <= x);

    return x / bin_size;
}

