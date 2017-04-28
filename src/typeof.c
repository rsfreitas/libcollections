
/*
 * Description:
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

void set_typeof_with_offset(enum cl_object type, void *p, unsigned int offset)
{
    struct cl_object_hdr *hdr = p + offset;

    if (NULL == hdr)
        return;

    hdr->lib_id = LIBID;
    hdr->object = type;
}

void set_typeof(enum cl_object type, void *p)
{
    set_typeof_with_offset(type, p, 0);
}

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

bool validate_object_with_offset(const void *p, enum cl_object type,
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

bool validate_object(const void *p, enum cl_object type)
{
    return validate_object_with_offset(p, type, 0);
}

