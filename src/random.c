
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

unsigned int LIBEXPORT cseed(void)
{
    FILE *f;
    char tmp[64]={0};

    cerrno_clear();
    f = popen("cat /proc/sys/kernel/random/uuid | cut -d '-' -f 2", "r");

    if (NULL == f) {
        cset_errno(CL_NULL_DATA);
        return 0;
    }

    fgets(tmp, sizeof(tmp) - 1, f);
    pclose(f);

    return strtol(tmp, NULL, 16);
}

