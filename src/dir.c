
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Feb  6 10:28:02 2017
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "collections.h"

__PUB_API__ int cl_mkdir(const char *dname)
{
    char new_path[512] = {0}, partial_path[64] = {0};
    const char *relative_path;

    __clib_function_init__(false, NULL, -1, -1);

    if (NULL == dname) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    /* o primeiro caracter deve ser um '/' */
    if (dname[0] != '/') {
        cset_errno(CL_INVALID_VALUE);
        return -1;
    }

    /* a path tem que ter pelo menos 2 caracteres */
    if (strlen(dname) < 2) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    relative_path = dname;

    do {
        sscanf(relative_path, "/%[^/]", partial_path);
        relative_path += (strlen(partial_path) + 1);

        strcat(new_path, "/");
        strcat(new_path, partial_path);

        if (access(new_path, F_OK)) {
            if (mkdir(new_path, S_IWUSR | S_IRUSR | S_IXUSR |
                      S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
            {
                cset_errno(CL_MKDIR_FAILED);
                return -1;
            }
        }
    } while ((relative_path[0] != '\0') && (strlen(relative_path) > 1));

    return 0;
}

