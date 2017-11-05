
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan 15 09:11:59 2016
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

__PUB_API__ cl_string_t *cl_bool_to_cstring(bool flag)
{
    __clib_function_init__(false, NULL, -1, NULL);

    if ((flag != true) && (flag != false)) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    return cl_string_create("%s", (flag == true) ? "true" : "false");
}

__PUB_API__ char *cl_bool_to_string(bool flag)
{
    cl_string_t *s = NULL;
    char *t = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    s = cl_bool_to_cstring(flag);

    if (NULL == s)
        return NULL;

    t = strdup(cl_string_valueof(s));
    cl_string_destroy(s);

    return t;
}

__PUB_API__ char *cl_version(void)
{
    char *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (asprintf(&s, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD) < 0)
        return NULL;

    return s;
}

__PUB_API__ cl_string_t *cl_type_to_cstring(enum cl_type value)
{
    const char *value_desc[] = {
        "void", "char", "unsigned char", "int", "unsigned int", "short int",
        "unsigned short int", "float", "double", "long", "unsigned long",
        "long long", "unsigned long long", "pointer", "string", "bool",
        "cstring" };

    __clib_function_init__(false, NULL, -1, NULL);

    if (cl_object_is_valid(value) == false) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    return cl_string_create("%s", value_desc[value]);
}

char *value_to_hex(void *p, unsigned int size)
{
    char *d = NULL, tmp[3] = {0}, *pp = p;
    unsigned int i;

    d = calloc((size * 2) + 1, sizeof(char));

    if (NULL == d)
        return NULL;

    for (i = 0; i < size; i++) {
        snprintf(tmp, sizeof(tmp) - 1, "%02x", pp[i]);
        strcat(d, tmp);
    }

    return d;
}

/*
 * Removes the file name extension, as well as all path, leaving only
 * its name.
 */
char *strip_filename(const char *pathname)
{
    char *ext = NULL, *n = NULL, *tmp = NULL, *bname = NULL;

    tmp = strdup(pathname);
    bname = basename(tmp);
    ext = strrchr(bname, '.');

    if (NULL == ext)
        n = strdup(bname);
    else {
        n = calloc(1, strlen(bname) - strlen(ext) + 1);

        if (NULL == n) {
            cset_errno(CL_NO_MEM);
            return NULL;
        }

        strncpy(n, bname, strlen(bname) - strlen(ext));
    }

    free(tmp);

    return n;
}

/*
 * Returns a file name extension.
 */
char *file_extension(const char *pathname)
{
    char *ext = NULL;

    ext = strrchr(pathname, '.');

    if (NULL == ext)
        return NULL;

    return strdup(ext + 1);
}

/**
 * @name cl_exit
 * @brief Terminate calling thread.
 *
 * This function must be called at the end of main function if the user wants no
 * memory leak errors reported by the valgrind tool.
 */
__PUB_API__ void cl_exit(void)
{
    if (library_initialized() == false)
        return;

    if (dl_is_plugin_enabled(CL_PLUGIN_JAVA)) {
        /* Does nothing here. It hangs if pthread_exit is called */
        return;
    }

#ifndef CL_USE_IMAGEAPI
    pthread_exit(NULL);
#endif
}

