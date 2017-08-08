
/*
 * Description: API to handle string lists using cl_stringlist_t object.
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

#define cl_stringlist_members              \
    cl_struct_member(cl_list_t *, data)

cl_struct_declare(cl_stringlist_s, cl_stringlist_members);

#define cl_stringlist_s      cl_struct(cl_stringlist_s)

static void release_node(void *ptr)
{
    cl_string_t *s = (cl_string_t *)ptr;

    cl_string_unref(s);
}

static int compare_node(cl_list_node_t *a, cl_list_node_t *b)
{
    cl_string_t *s_a = cl_list_node_content(a);
    cl_string_t *s_b = cl_list_node_content(b);

    return cl_string_cmp(s_a, s_b);
}

static int filter_node(cl_list_node_t *a, void *ptr)
{
    cl_string_t *content = cl_list_node_content(a);
    cl_string_t *value = (cl_string_t *)ptr;

    if (cl_string_cmp(content, value) == 0)
        return 1;

    return 0;
}

static int equals_node(cl_list_node_t *a, cl_list_node_t *b)
{
    cl_string_t *s_a = cl_list_node_content(a);
    cl_string_t *s_b = cl_list_node_content(b);

    if (cl_string_cmp(s_a, s_b) == 0)
        return 1;

    return 0;
}

__PUB_API__ cl_stringlist_t *cl_stringlist_create(void)
{
    cl_stringlist_s *l = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    l = calloc(1, sizeof(cl_stringlist_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->data = cl_list_create(release_node, compare_node, filter_node,
                             equals_node);

    if (NULL == l->data)
        return NULL;

    set_typeof(CL_OBJ_STRINGLIST, l);

    return l;
}

__PUB_API__ int cl_stringlist_destroy(cl_stringlist_t *l)
{
    cl_stringlist_s *p = (cl_stringlist_s *)l;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, -1);

    cl_list_destroy(p->data);
    free(l);

    return 0;
}

__PUB_API__ int cl_stringlist_size(const cl_stringlist_t *l)
{
    cl_stringlist_s *p = (cl_stringlist_s *)l;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, -1);

    return cl_list_size(p->data);
}

__PUB_API__ int cl_stringlist_add(cl_stringlist_t *l, cl_string_t *s)
{
    cl_stringlist_s *p = (cl_stringlist_s *)l;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(l, CL_OBJ_STRINGLIST) == false) ||
        (validate_object(s, CL_OBJ_STRING) == false))
    {
        return -1;
    }

    cl_list_unshift(p->data, cl_string_ref(s), -1);

    return 0;
}

__PUB_API__ cl_string_t *cl_stringlist_get(const cl_stringlist_t *l,
    unsigned int index)
{
    cl_stringlist_s *p = (cl_stringlist_s *)l;
    cl_list_node_t *node = NULL;
    cl_string_t *s = NULL;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, NULL);

    if (index >= (unsigned int)cl_stringlist_size(p))
        return NULL;

    node = cl_list_at(p->data, index);

    if (NULL == node) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    s = cl_string_ref(cl_list_node_content(node));
    cl_list_node_unref(node);

    return s;
}

__PUB_API__ cl_string_t *cl_stringlist_flat(const cl_stringlist_t *l,
    const char delimiter)
{
    cl_stringlist_s *p = (cl_stringlist_s *)l;
    cl_string_t *s = NULL, *node = NULL;
    unsigned int i, size;

    __clib_function_init__(true, l, CL_OBJ_STRINGLIST, NULL);
    s = cl_string_create_empty(0);

    if (NULL == s)
        return NULL;

    size = cl_list_size(p->data);

    for (i = 0; i < size; i++) {
        node = cl_stringlist_get(l, i);

        if (node != NULL) {
            cl_string_cat(s, "%s%c", cl_string_valueof(node), delimiter);
            cl_string_unref(node);
        }
    }

    return s;
}

__PUB_API__ cl_stringlist_t *cl_stringlist_dup(const cl_stringlist_t *list)
{
    int i, t;
    cl_stringlist_t *new = NULL;
    cl_string_t *tmp, *s;

    __clib_function_init__(true, list, CL_OBJ_STRINGLIST, NULL);
    t = cl_stringlist_size(list);
    new = cl_stringlist_create();

    if (NULL == new)
        return NULL;

    for (i = 0; i < t; i++) {
        tmp = cl_stringlist_get(list, i);
        s = cl_string_dup(tmp);
        cl_string_unref(tmp);

        cl_stringlist_add(new, s);
        cl_string_unref(s);
    }

    return new;
}

