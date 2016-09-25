
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

cstring_t LIBEXPORT *cbool_to_cstring(bool flag)
{
    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if ((flag != true) && (flag != false)) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    return cstring_create("%s", (flag == true) ? "true" : "false");
}

char LIBEXPORT *cbool_to_string(bool flag)
{
    cstring_t *s = NULL;
    char *t = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    s = cbool_to_cstring(flag);

    if (NULL == s)
        return NULL;

    t = strdup(cstring_valueof(s));
    cstring_destroy(s);

    return t;
}

char LIBEXPORT *collections_version(void)
{
    char *s = NULL;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (asprintf(&s, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD) < 0)
        return NULL;

    return s;
}

cstring_t LIBEXPORT *cl_type_to_cstring(enum cl_type value)
{
    const char *value_desc[] = {
        "void", "char", "unsigned char", "int", "unsigned int", "short int",
        "unsigned short int", "float", "double", "long", "unsigned long",
        "long long", "unsigned long long", "pointer", "string", "bool",
        "cstring" };

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    if (validate_cl_type(value) == false) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    return cstring_create("%s", value_desc[value]);
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

    return strdup(ext);
}

