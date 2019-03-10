
/*
 * Description: Internal functions to manipulate/check internal library objects.
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  2 09:37:34 2016
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

#include "collections.h"

#define LIBID       0xC011EC7105LL

/*
 *
 * Internal functions
 *
 */

static bool validate_libid(void *p, unsigned int offset)
{
    struct cl_object_hdr *hdr = p + offset;

    if (NULL == hdr)
        return false;

    if (hdr->lib_id != LIBID)
        return false;

    return true;
}

static bool validate_typeof(void *p, enum cl_object type, unsigned int offset)
{
    struct cl_object_hdr *hdr = p + offset;

    if (NULL == hdr)
        return false;

    if (hdr->object != type)
        return false;

    return true;
}

/*
 *
 * Internal API
 *
 */

/*
 * Sets an object as an internal known object, expecting that it may have enough
 * space to hold this information.
 *
 * This function writes this information into the object using an offset to
 * align it.
 */
CL_INTERNAL_API
void typeof_set_with_offset(enum cl_object type, void *p, unsigned int offset)
{
    struct cl_object_hdr *hdr = p + offset;

    if (NULL == hdr)
        return;

    hdr->lib_id = LIBID;
    hdr->object = type;
}

/*
 * Sets an object as an internal known object, expecting that it may have enough
 * space to hold this information.
 *
 * This function writes this information into the beginning of the object.
 */
CL_INTERNAL_API
void typeof_set(enum cl_object type, void *p)
{
    typeof_set_with_offset(type, p, 0);
}

/*
 * Checks if an object is of a specific type, using an offset to try to find the
 * object information.
 */
CL_INTERNAL_API
bool typeof_validate_object_with_offset(const void *p, enum cl_object type,
    unsigned int offset)
{
    if (NULL == p) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    if (validate_libid((void *)p, offset) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return false;
    }

    if (validate_typeof((void *)p, type, offset) == false) {
        cset_errno(CL_INVALID_VALUE);
        return false;
    }

    return true;
}

/*
 * Checks if an object is of a specific type, assuming that the information
 * about it is in the beginning.
 */
CL_INTERNAL_API
bool typeof_validate_object(const void *p, enum cl_object type)
{
    return typeof_validate_object_with_offset(p, type, 0);
}

/*
 * A function that tries to find the object type. Always returns an information,
 * even if it's not an internal known object.
 */
CL_INTERNAL_API
enum cl_object typeof_guess_object(const void *ptr)
{
    if (NULL == ptr) {
        cset_errno(CL_NULL_ARG);
        return CL_OBJ_UNKNOWN;
    }

    /* TODO */
    return CL_OBJ_UNKNOWN;
}

