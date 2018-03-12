
/*
 * Description: Plugin information functions.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:34:01 BRST 2015
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
#include <ctype.h>

#include "collections.h"
#include "plugin.h"

#define cinfo_members                       \
    cl_struct_member(char *, name)          \
    cl_struct_member(char *, version)       \
    cl_struct_member(char *, author)        \
    cl_struct_member(char *, description)   \
    cl_struct_member(void *, data)          \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cinfo_s, cinfo_members);

#define cinfo_s         cl_struct(cinfo_s)

static void __destroy_info_s(const struct cl_ref_s *ref)
{
    cinfo_s *info = cl_container_of(ref, cinfo_s, ref);

    if (NULL == info)
        return;

    free(info->author);
    free(info->description);
    free(info->version);
    free(info->name);
    free(info);
}

static cinfo_s *new_info_s(const char *name, const char *version,
    const char *description, const char *author)
{
    cinfo_s *i = NULL;

    i = calloc(1, sizeof(cinfo_s));

    if (NULL == i) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    i->name = strdup(name);
    i->version = strdup(version);
    i->description = strdup(description);
    i->author = strdup(author);
    i->data = NULL;

    i->ref.count = 1;
    i->ref.free = __destroy_info_s;

    typeof_set(CL_OBJ_PLUGIN_INFO, i);

    return i;
}

cl_plugin_info_t *info_ref(cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return NULL;

    cl_ref_inc(&i->ref);

    return info;
}

void info_unref(cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return;

    cl_ref_dec(&i->ref);
}

cl_plugin_info_t *info_create_from_data(const char *name,
    const char *version, const char *author, const char *description)
{
    cinfo_s *info = NULL;

    info = new_info_s(name, version, description, author);

    if (NULL == info)
        return NULL;

    return (cl_plugin_info_t *)info;
}

char *info_get_name(const cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return NULL;

    return i->name;
}

char *info_get_version(const cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return NULL;

    return i->version;
}

char *info_get_description(const cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return NULL;

    return i->description;
}

char *info_get_author(const cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if (NULL == i)
        return NULL;

    return i->author;
}

void info_set_custom_data(cl_plugin_info_t *info, void *ptr)
{
    cinfo_s *i = (cinfo_s *)info;

    if ((NULL == i) || (NULL == ptr))
        return;

    i->data = ptr;
}

void *info_get_custom_data(cl_plugin_info_t *info)
{
    cinfo_s *i = (cinfo_s *)info;

    if ((NULL == i) || (NULL == i->data))
        return NULL;

    return i->data;
}

