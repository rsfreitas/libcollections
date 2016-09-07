
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

#define INITIAL_BUFFER_SIZE         120

unsigned char LIBEXPORT *cfload(const char *filename, unsigned int *bsize)
{
    FILE *f;
    struct stat info;
    unsigned char *b=NULL;
    size_t sread = 0;

    if (library_initialized() == false)
        return NULL;

    if (stat(filename, &info) < 0)
        return NULL;

    b = calloc(1, info.st_size + 1);

    if (NULL == b)
        return NULL;

    f = fopen(filename, "r");

    if (NULL == f) {
        free(b);
        return NULL;
    }

    sread = fread(b, 1, info.st_size, f);
    fclose(f);

    if ((off_t)sread != info.st_size)
        return NULL;

    *bsize = info.st_size;

    return b;
}

int LIBEXPORT cfunload(unsigned char *buffer)
{
    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (NULL == buffer) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    free(buffer);
    buffer = NULL;

    return 0;
}

int LIBEXPORT cfsave(const char *filename, const unsigned char *buffer,
    unsigned int bsize)
{
    FILE *f;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

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

static void *getblock(size_t n_bytes)
{
    void *result;

    result = malloc(n_bytes);

    if (!result)
        return NULL;

    return result;
}

static void freeblock(void *ptr)
{
    free(ptr);
}

char LIBEXPORT *cfreadline(FILE *infile)
{
    char *line, *nline;
    int n = 0, ch, size = INITIAL_BUFFER_SIZE;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (NULL == infile)
        return NULL;

    line = getblock(size + 1);

    while (((ch = getc(infile)) != '\n') && (ch != EOF)) {
        if (n == size) {
            size *= 2;
            nline = (char *)getblock(size + 1);
            strncpy(nline, line, n);
            freeblock(line);
            line = nline;
        }

        line[n++] = ch;
    }

    /* EOF */
    if ((n == 0) && (ch == EOF)) {
        freeblock(line);
        return NULL;
    }

    line[n] = '\0';
    nline = (char *)getblock(n + 1);
    strcpy(nline, line);
    freeblock(line);

    return nline;
}

