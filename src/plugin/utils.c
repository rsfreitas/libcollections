
/*
 * Description:
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

#include "collections.h"
#include "plugin.h"

void destroy_arg_type(const struct cl_ref_s *ref)
{
    struct cl_arg_type *arg = cl_container_of(ref, struct cl_arg_type, ref);

    if (NULL == arg)
        return;

    free(arg);
}

struct cl_arg_type *new_arg_type(enum cl_type type)
{
    struct cl_arg_type *arg = NULL;

    arg = calloc(1, sizeof(struct cl_arg_type));

    if (NULL == arg)
        return NULL;

    arg->type = type;
    arg->ref.count = 1;
    arg->ref.free = destroy_arg_type;

    return arg;
}

static void unref_arg_type(void *ptr)
{
    struct cl_arg_type *arg = (struct cl_arg_type *)ptr;

    if (NULL == arg)
        return;

    cl_ref_dec(&arg->ref);
}

void destroy_cplugin_function_s(void *a)
{
    struct cplugin_function_s *f = (struct cplugin_function_s *)a;

    if (f->arguments != NULL)
        cl_hashtable_uninit(f->arguments);

    cl_list_destroy(f->arg_types);
    free(f->name);
    free(f);
}

static void release_argument(void *ptr)
{
    cl_object_t *arg = (cl_object_t *)ptr;

    if (NULL == ptr)
        return;

    cl_object_unref(arg);
}

struct cplugin_function_s *new_cplugin_function_s(const char *name,
    enum cl_type return_value)
{
    struct cplugin_function_s *f = NULL;

    f = calloc(1, sizeof(struct cplugin_function_s));

    if (NULL == f) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    f->arguments = cl_hashtable_init(MAX_ARGUMENTS, false, NULL, release_argument);
    f->arg_types = cl_list_create(unref_arg_type, NULL, NULL, NULL);
    f->name = strdup(name);
    f->return_value = return_value;

    return f;
}

void destroy_cplugin_function_s_list(struct cplugin_function_s *flist)
{
    cl_dll_free(flist, destroy_cplugin_function_s);
}

cplugin_s *new_cplugin_s(void)
{
    cplugin_s *p = NULL;

    p = calloc(1, sizeof(cplugin_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    typeof_set(CL_OBJ_PLUGIN, p);

    return p;
}

int destroy_cplugin_s(cplugin_s *cpl)
{
    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    /* Need to free the info struct of the plugin. */
    info_unref(cpl->info);

    free(cpl);

    return 0;
}

