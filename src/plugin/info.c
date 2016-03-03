
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

struct info_s {
    char                    *name;
    char                    *version;
    char                    *author;
    char                    *description;
    cjson_t                 *api;
    struct ref_s            ref;
};

static void __destroy_info_s(const struct ref_s *ref)
{
    struct info_s *info = container_of(ref, struct info_s, ref);

    if (NULL == info)
        return;

    if (info->api != NULL)
        api_unload(info->api);

    free(info->author);
    free(info->description);
    free(info->version);
    free(info->name);
    free(info);
}

static struct info_s *new_info_s(const char *name,
    const char *version, const char *description, const char *author)
{
    struct info_s *i = NULL;

    i = calloc(1, sizeof(struct info_s));

    if (NULL == i) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    i->name = strdup(name);
    i->version = strdup(version);
    i->description = strdup(description);
    i->author = strdup(author);
    i->api = NULL;

    i->ref.count = 1;
    i->ref.free = __destroy_info_s;

    return i;
}

cplugin_info_t *info_ref(cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    ref_inc(&i->ref);

    return info;
}

void info_unref(cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return;

    ref_dec(&i->ref);
}

cplugin_info_t *info_create_from_data(const char *name,
    const char *version, const char *author, const char *description,
    const char *api)
{
    struct info_s *info = NULL;

    info = new_info_s(name, version, description, author);

    if (NULL == info)
        return NULL;

    info->api = api_load(api);

    if (NULL == info->api) {
        info_unref((cplugin_info_t *)info);
        return NULL;
    }

    return (cplugin_info_t *)info;
}

cplugin_info_t *info_create_from_entry(struct cplugin_entry_s *entry)
{
    struct info_s *info = NULL;

    info = new_info_s(entry->name, entry->version, entry->description,
                              entry->author);

    if (NULL == info)
        return NULL;

    info->api = api_load(entry->api);

    if (NULL == info->api) {
        info_unref((cplugin_info_t *)info);
        return NULL;
    }

    return (cplugin_info_t *)info;
}

cjson_t *info_get_api(const cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    return i->api;
}

char *info_get_name(const cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    return i->name;
}

char *info_get_version(const cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    return i->version;
}

char *info_get_description(const cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    return i->description;
}

char *info_get_author(const cplugin_info_t *info)
{
    struct info_s *i = (struct info_s *)info;

    if (NULL == i)
        return NULL;

    return i->author;
}

