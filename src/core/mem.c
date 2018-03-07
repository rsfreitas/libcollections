
/*
 * Description: Memory related functions.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 24 15:44:01 2015
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
#include <string.h>

#include "collections.h"

/*
 * Duplicates a specific buffer.
 */
__PUB_API__ void *cl_memdup(const void *src, unsigned int len)
{
    void *p;

    __clib_function_init__(false, NULL, -1, NULL);
    p = calloc(1, len);

    if (p != NULL)
        memcpy(p, src, len);

    return p;
}

