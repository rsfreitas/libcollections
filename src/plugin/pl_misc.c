
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

struct cplugin_fdata_s *new_cplugin_fdata_s(const char *name, enum cl_type type)
{
    struct cplugin_fdata_s *f = NULL;

    f = calloc(1, sizeof(struct cplugin_fdata_s));

    if (NULL == f) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    if (name != NULL)
        f->name = strdup(name);

    f->type = type;
    set_typeof_with_offset(CPLUGIN_ARG, f, CPLUGIN_ARG_OBJECT_OFFSET);

    return f;
}

void destroy_cplugin_fdata_s(void *a)
{
    struct cplugin_fdata_s *fdata = (struct cplugin_fdata_s *)a;

    if (NULL == fdata)
        return;

    if (fdata->name != NULL)
        free(fdata->name);

    free(fdata);
}

struct cplugin_function_s *new_cplugin_function_s(const char *name,
    enum cl_type return_value, enum cplugin_arg_mode arg_mode,
    struct cplugin_fdata_s *args)
{
    struct cplugin_function_s *f = NULL;

    f = calloc(1, sizeof(struct cplugin_function_s));

    if (NULL == f) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    f->name = strdup(name);
    f->return_value = return_value;
    f->args = args;
    f->arg_mode = arg_mode;

    return f;
}

static void destroy_cplugin_function_s(void *a)
{
    struct cplugin_function_s *f = (struct cplugin_function_s *)a;

    if (f->args != NULL)
        cdll_free(f->args, destroy_cplugin_fdata_s);

    free(f->name);
    free(f);
}

void destroy_cplugin_function_s_list(struct cplugin_function_s *flist)
{
    cdll_free(flist, destroy_cplugin_function_s);
}

cplugin_s *new_cplugin_s(void)
{
    cplugin_s *p = NULL;

    p = calloc(1, sizeof(cplugin_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->functions = NULL;
    set_typeof(CPLUGIN, p);

    return p;
}

int destroy_cplugin_s(cplugin_s *cpl)
{
    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (cpl->functions != NULL)
        cdll_free(cpl->functions, destroy_cplugin_function_s);

    /* Need to free the info struct of the plugin. */
    info_unref(cpl->info);

    free(cpl);

    return 0;
}

/*
 * Compares if a specific struct 'struct cplugin_function_s' matches a
 * function name indicated in @b.
 */
int search_cplugin_function_s(void *a, void *b)
{
    struct cplugin_function_s *f = (struct cplugin_function_s *)a;
    char *function_name = (char *)b;

    if (strcmp(f->name, function_name) == 0)
        return 1;

    return 0;
}

/*
 * Compares if a specific struct 'struct cplugin_fdata_s' matches an argument
 * name indicated in @b.
 */
int search_cplugin_fdata_s(void *a, void *b)
{
    struct cplugin_fdata_s *arg = (struct cplugin_fdata_s *)a;
    char *arg_name = (char *)b;

    if (strcmp(arg->name, arg_name) == 0)
        return 1;

    return 0;
}

