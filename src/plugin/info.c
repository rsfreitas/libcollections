
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

#include <string.h>
#include <ctype.h>

#include "collections.h"
#include "plugin.h"

#define DEFAULT_TOKEN           ','

/*
 * Internal structure to pass as argument while manipulating internal lists.
 */
struct list_data {
    cstring_t   *s;
    char        token;
    cjson_t     *j;
};

static int strcat_function_name(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    struct list_data *ld = (struct list_data *)b;

    cstring_cat(ld->s, "%s%c", foo->name, ld->token);

    return 0;
}

static bool is_invalid_token(char token)
{
    if (isalnum(token) || isblank(token))
        return false;

    return true;
}

static char *functions_list_str(struct cplugin_s *cpl, char token)
{
    struct list_data ld;

    if (is_invalid_token(token) == true)
        token = DEFAULT_TOKEN;

    ld.s = NULL;
    ld.token = token;
    cdll_map(cpl->functions, strcat_function_name, &ld);

    return ld.s;
}

static int strcat_function_arg(void *a, void *b)
{
    struct cplugin_fdata_s *arg = (struct cplugin_fdata_s *)a;
    struct list_data *ld = (struct list_data *)b;

    cstring_cat(ld->s, "%s,", arg->name);

    return 0;
}

static char *function_arg_list_str(struct cplugin_s *cpl,
    const char *function_name, char token)
{
    struct cplugin_function_s *foo = NULL;
    struct list_data ld;
    char *p;

    if (is_invalid_token(token) == true)
        token = DEFAULT_TOKEN;

    ld.s = NULL;
    ld.token = token;
    foo = cdll_map(cpl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
        return NULL;
    }

    /* No arguments. No error. */
    if (NULL == foo->args)
        return NULL;

    cdll_map(foo->args, strcat_function_arg, &ld);
    p = strdup(cstring_valueof(ld.s));
    cstring_free(ld.s);

    return p;
}

char *get_info(enum cplugin_info info, struct cplugin_s *cpl,
    const char *function_name, char token)
{
    struct cplugin_info_s *pl_info = NULL;

    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    pl_info = cpl->info;

    if (NULL == pl_info) {
        cset_errno(CL_NO_PLUGIN_INFO);
        return NULL;
    }

    switch (info) {
        case CPLUGIN_INFO_NAME:
            return pl_info->name;

        case CPLUGIN_INFO_VERSION:
            return pl_info->version;

        case CPLUGIN_INFO_DESCRIPTION:
            return pl_info->description;

        case CPLUGIN_INFO_CREATOR:
            return pl_info->creator;

        case CPLUGIN_INFO_FUNCTION_LIST:
            return functions_list_str(cpl, token);

        case CPLUGIN_INFO_FUNCTION_ARG_LIST:
            return function_arg_list_str(cpl, function_name, token);

        default:
            cset_errno(CL_INVALID_VALUE);
            return NULL;
    }

    return NULL;
}

struct cplugin_info_s *info_create_from_data(const char *name,
    const char *version, const char *creator, const char *description,
    const char *api)
{
    struct cplugin_info_s *info = NULL;

    info = new_cplugin_info_s(name, version, description, creator);

    if (NULL == info)
        return NULL;

    info->api = api_load(api);

    if (NULL == info->api) {
        destroy_cplugin_info_s(info);
        return NULL;
    }

    return info;
}

struct cplugin_info_s *info_create_from_entry(struct cplugin_entry_s *entry)
{
    struct cplugin_info_s *info = NULL;

    info = new_cplugin_info_s(entry->name, entry->version, entry->description,
                              entry->creator);

    if (NULL == info)
        return NULL;

    info->startup = entry->startup;
    info->shutdown = entry->shutdown;
    info->api = api_load(entry->api);

    if (NULL == info->api) {
        destroy_cplugin_info_s(info);
        return NULL;
    }

    return info;
}

/* TODO: Move to api_parse.c */
cjson_t *get_function_object(cjson_t *api, const char *function_name)
{
    cjson_t *f, *o, *p;
    int i, t = 0;
    cstring_t *name;

    f = cjson_get_object_item(api, "API");
    t = cjson_get_array_size(f);

    for (i = 0; i < t; i++) {
        o = cjson_get_array_item(f, i);
        p = cjson_get_object_item(o, "name");
        name = cjson_get_object_value(p);

        if (!strcmp(cstring_valueof(name), function_name))
            return o;
    }

    return NULL;
}

