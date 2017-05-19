
/*
 * Description: API to handle string lists using cl_string_list_t object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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

#include "collections.h"

struct cl_string_list_node_s {
    cl_list_entry_t       *prev;
    cl_list_entry_t       *next;
    cl_string_t           *s;
};

#define cl_string_list_members                                \
    cl_struct_member(struct cl_string_list_node_s *, list)    \
    cl_struct_member(unsigned int, size)

cl_struct_declare(cl_string_list_s, cl_string_list_members);

#define cl_string_list_s      cl_struct(cl_string_list_s)

static struct cl_string_list_node_s *new_list_node_s(cl_string_t *v)
{
    struct cl_string_list_node_s *l = NULL;

    l = calloc(1, sizeof(struct cl_string_list_node_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->s = cl_string_ref(v);

    return l;
}

static void destroy_list_node_s(void *ptr)
{
    struct cl_string_list_node_s *l = (struct cl_string_list_node_s *)ptr;

    if (NULL == ptr)
        return;

    cl_string_unref(l->s);
    free(l);
}

__PUB_API__ cl_string_list_t *cl_string_list_create(void)
{
    cl_string_list_s *l = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    l = calloc(1, sizeof(cl_string_list_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->list = NULL;
    l->size = 0;

    set_typeof(CL_OBJ_STRINGLIST, l);

    return l;
}

__PUB_API__ int cl_string_list_destroy(cl_string_list_t *l)
{
    cl_string_list_s *p = (cl_string_list_s *)l;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, -1);

    if (p->size > 0)
        cl_dll_free(p->list, destroy_list_node_s);

    free(l);

    return 0;
}

__PUB_API__ int cl_string_list_size(const cl_string_list_t *l)
{
    cl_string_list_s *p = (cl_string_list_s *)l;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, -1);

    return p->size;
}

__PUB_API__ int cl_string_list_add(cl_string_list_t *l, cl_string_t *s)
{
    cl_string_list_s *p = (cl_string_list_s *)l;
    struct cl_string_list_node_s *n = NULL;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(l, CL_OBJ_STRINGLIST) == false) ||
        (validate_object(s, CL_OBJ_STRING) == false))
    {
        return -1;
    }

    n = new_list_node_s(s);

    if (NULL == n)
        return -1;

    p->list = cl_dll_unshift(p->list, n);
    p->size++;

    return 0;
}

__PUB_API__ cl_string_t *cl_string_list_get(const cl_string_list_t *l,
    unsigned int index)
{
    cl_string_list_s *p = (cl_string_list_s *)l;
    struct cl_string_list_node_s *n = NULL;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, NULL);

    if (index >= (unsigned int)cl_string_list_size(p))
        return NULL;

    n = cl_dll_at(p->list, index);

    if (NULL == n) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    return cl_string_ref(n->s);
}

__PUB_API__ cl_string_t *cl_string_list_flat(const cl_string_list_t *l,
    const char delimiter)
{
    cl_string_list_s *p = (cl_string_list_s *)l;
    struct cl_string_list_node_s *n = NULL;
    cl_string_t *s = NULL;
    unsigned int i;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, NULL);
    s = cl_string_create_empty(0);

    if (NULL == s)
        return NULL;

    for (i = 0; i < p->size; i++) {
        n = cl_dll_at(p->list, i);

        if (n != NULL)
            cl_string_cat(s, "%s%c", cl_string_valueof(n->s), delimiter);
    }

    return s;
}

__PUB_API__ cl_string_list_t *cl_string_list_dup(const cl_string_list_t *list)
{
    int i, t;
    cl_string_list_t *new = NULL;
    cl_string_t *tmp, *s;

    __clib_function_init__(true, list, CL_OBJ_STRINGLIST, NULL);
    t = cl_string_list_size(list);
    new = cl_string_list_create();

    if (NULL == new)
        return NULL;

    for (i = 0; i < t; i++) {
        tmp = cl_string_list_get(list, i);
        s = cl_string_dup(tmp);
        cl_string_unref(tmp);

        cl_string_list_add(new, s);
        cl_string_unref(s);
    }

    return new;
}

