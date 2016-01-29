
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

#include <string.h>

#include "collections.h"

cstring_t LIBEXPORT *cbool_to_cstring(bool flag)
{
    cerrno_clear();

    if ((flag != true) && (flag != false)) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    return cstring_new("%s", (flag == true) ? "true" : "false");
}

char LIBEXPORT *cbool_to_c_string(bool flag)
{
    cstring_t *s = NULL;
    char *t = NULL;

    s = cbool_to_cstring(flag);

    if (NULL == s)
        return NULL;

    t = strdup(cstring_valueof(s));
    cstring_free(s);

    return t;
}

char LIBEXPORT *collections_version(void)
{
    char *s = NULL;

    if (asprintf(&s, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION, BUILD) < 0)
        return NULL;

    return s;
}

