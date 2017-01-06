
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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include <magic.h>

#include "collections.h"

struct cl_data {
    magic_t             cookie;
    bool                initialized;
    struct ref_s        ref;
    struct random_data  rd_data;
    char                state[128];
    cjson_t             *cfg;
    char                *package;
    char                *locale_dir;
};

static struct cl_data __cl_data = {
    .initialized = false,
    .ref.count = 0,
    .cfg = NULL,
    .package = NULL,
    .locale_dir = NULL,
};

static bool load_arg_as_json_string(const char *data)
{
    cstring_t *s = NULL;

    s = cstring_create("%s", data);
    __cl_data.cfg = cjson_parse(s);
    cstring_unref(s);

    if (NULL == __cl_data.cfg)
        return false;

    return true;
}

static bool load_arg_as_file(const char *pathname)
{
    struct stat st;

    if (stat(pathname, &st) < 0)
        return false;

    __cl_data.cfg = cjson_read_file(pathname);

    if (NULL == __cl_data.cfg)
        return false;

    return true;
}

static char *get_program_name(void)
{
    char *tmp, *bname, *real_name;

    tmp = strdup(program_invocation_name);
    bname = basename(tmp);

    while (!(isalpha(*bname)))
        bname++;

    real_name = strdup(bname);
    free(tmp);

    return real_name;
}

#define get_configuration(object) \
({\
     cstring_t *s;\
     cobject_t *o = NULL;\
     o = cjson_get_object_item(__cl_data.cfg, object);\
     s = cjson_get_object_value(o);\
     cstring_valueof(s);\
})

static void load_default_values(void)
{
    __cl_data.package =
        (NULL == __cl_data.cfg)
            ? get_program_name()
            : strdup(get_configuration("package"));

    __cl_data.locale_dir =
        strdup((NULL == __cl_data.cfg)
            ? ""
            : get_configuration("locale_dir"));
}

static void load_arg(const char *arg)
{
    /*
     * Ok, we set ourselves here as initialized so we can use some internal
     * library APIs.
     */
    __cl_data.initialized = true;

    if (arg != NULL)
        if (load_arg_as_file(arg) == false)
            load_arg_as_json_string(arg);

    load_default_values();

    __cl_data.initialized = false;
}

static void __uninit(const struct ref_s *ref __attribute__((unused)))
{
    if (__cl_data.package != NULL)
        free(__cl_data.package);

    if (__cl_data.locale_dir != NULL)
        free(__cl_data.locale_dir);

    dl_library_uninit();
    magic_close(__cl_data.cookie);
}

static int __init(const char *arg)
{
    load_arg(arg);

    /* Initialize libc random numbers seed */
    initstate_r(time(NULL) + cl_cseed(), __cl_data.state,
                sizeof(__cl_data.state), &__cl_data.rd_data);

    /* Initialize translation support */
    intl_start(__cl_data.package, __cl_data.locale_dir);

    /* Initialize libmagic environment */
    __cl_data.cookie = magic_open(MAGIC_MIME_TYPE);

    if (NULL == __cl_data.cookie)
        return -1;

    if (magic_load(__cl_data.cookie, NULL) != 0) {
        magic_close(__cl_data.cookie);
        return -1;
    }

    /* Initialize plugins */
    dl_library_init();

    __cl_data.ref.free = __uninit;
    __cl_data.initialized = true;

    return 0;
}

int LIBEXPORT collections_init(const char *arg)
{
    int old = 0, new = 1, ret = 0;

    if (ref_bool_compare(&__cl_data.ref, old, new) == true)
        ret = __init(arg);
    else
        ref_inc(&__cl_data.ref);

    return ret;
}

void LIBEXPORT collections_uninit(void)
{
    ref_dec(&__cl_data.ref);
}

/*
 *
 * Internal library API to access configuration or status.
 *
 */

/*
 * We need to call this function inside every exported function from the
 * library to make sure that the library was initialized before.
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

struct random_data *library_random_data(void)
{
    return &__cl_data.rd_data;
}

const char *library_package_name(void)
{
    return __cl_data.package;
}

const char *library_locale_dir(void)
{
    return __cl_data.locale_dir;
}

cjson_t *library_configuration(void)
{
    return __cl_data.cfg;
}

