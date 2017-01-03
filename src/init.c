
/*
 * Description: API to initialize/uninitialize anything that the library needs.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Sep  7 10:07:54 2016
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
#include <time.h>

#include <magic.h>

#include "collections.h"

struct cl_data {
    magic_t         cookie;
    bool            initialized;
    struct ref_s    ref;
};

static struct cl_data __cl_data = {
    .initialized = false,
    .ref.count = 0,
};

static void __uninit(const struct ref_s *ref __attribute__((unused)))
{
    dl_library_uninit();
    magic_close(__cl_data.cookie);
}

void LIBEXPORT collections_init(void)
{
    int old = 0, new = 1;

    if (ref_bool_compare(&__cl_data.ref, old, new) == true) {
        /* Initialize libc random numbers seed */
        srandom(time(NULL) + cseed());

        /* Initialize libmagic environment */
        __cl_data.cookie = magic_open(MAGIC_MIME_TYPE);

        if (NULL == __cl_data.cookie)
            return;

        if (magic_load(__cl_data.cookie, NULL) != 0) {
            magic_close(__cl_data.cookie);
            return;
        }

        /* Initialize plugins */
        dl_library_init();

        __cl_data.ref.free = __uninit;
        __cl_data.initialized = true;
    } else
        ref_inc(&__cl_data.ref);
}

void LIBEXPORT collections_uninit(void)
{
    ref_dec(&__cl_data.ref);
}

/*
 * We need to call this function inside every exported function from the
 * library.
 */
bool library_initialized(void)
{
    if (__cl_data.initialized == false) {
        /* Set the error code here */
        cset_errno(CL_LIB_WAS_NOT_STARTED);
        return false;
    }

    return true;
}

magic_t *library_get_cookie(void)
{
    return &__cl_data.cookie;
}

