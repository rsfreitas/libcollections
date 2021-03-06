
/*
 * Description: Utility functions to use with FILEs.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "collections.h"

__PUB_API__ unsigned char *cl_fload(const char *filename, unsigned int *bsize)
{
    FILE *f;
    struct stat info;
    unsigned char *b=NULL;
    size_t sread = 0;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (stat(filename, &info) < 0) {
        cset_errno(CL_FILE_NOT_FOUND);
        return NULL;
    }

    b = calloc(1, info.st_size + 1);

    if (NULL == b) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    f = fopen(filename, "r");

    if (NULL == f) {
        cset_errno(CL_FILE_OPEN_ERROR);
        free(b);
        return NULL;
    }

    sread = fread(b, 1, info.st_size, f);
    fclose(f);

    if ((off_t)sread != info.st_size) {
        cset_errno(CL_INVALID_FILE_SIZE);
        free(b);
        return NULL;
    }

    *bsize = info.st_size;

    return b;
}

__PUB_API__ int cl_funload(unsigned char *buffer)
{
    __clib_function_init__(false, NULL, -1, -1);

    if (NULL == buffer) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    free(buffer);
    buffer = NULL;

    return 0;
}

__PUB_API__ int cl_fsave(const char *filename, const unsigned char *buffer,
    unsigned int bsize)
{
    FILE *f;

    __clib_function_init__(false, NULL, -1, -1);

    if ((NULL == filename) || (NULL == buffer)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    f = fopen(filename, "w+");

    if (NULL == f) {
        /* TODO: set error code */
        return -1;
    }

    fwrite(buffer, bsize, 1, f);
    fclose(f);

    return 0;
}

__PUB_API__ char *cl_freadline(FILE *infile)
{
    char *line = NULL;
    size_t size = 0;
    int bytes_read = 0;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == infile) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    bytes_read = getline(&line, &size, infile);

    if (bytes_read < 0) {
        if (line != NULL)
            free(line);

        return NULL;
    }else if (bytes_read > 0)
        line[bytes_read - 1] = '\0';

    return line;
}

__PUB_API__ char *cl_file_mime_type(const char *pathname)
{
    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return library_file_mime_type(pathname);
}

__PUB_API__ char *cl_buffer_mime_type(const unsigned char *buffer,
    unsigned int size)
{
    __clib_function_init__(false, NULL, -1, NULL);

    if ((NULL == buffer) || (size == 0)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return library_buffer_mime_type(buffer, size);
}

