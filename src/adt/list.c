
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Dec 21 10:49:15 2016
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

/*
 *
 * API
 *
 */

void *cl_list_node_content(cl_list_node_t *node)
{
    return cglist_node_content(node, CL_OBJ_LIST_NODE);
}

cl_list_node_t *cl_list_node_ref(cl_list_node_t *node)
{
    return (cl_list_node_t *)cglist_node_ref(node, CL_OBJ_LIST_NODE);
}

int cl_list_node_unref(cl_list_node_t *node)
{
    return cglist_node_unref(node, CL_OBJ_LIST_NODE);
}

cl_list_t *cl_list_ref(cl_list_t *list)
{
    return (cl_list_t *)cglist_ref((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_unref(cl_list_t *list)
{
    return cglist_unref((cl_list_t *)list, CL_OBJ_LIST);
}

cl_list_t *cl_list_create(void (*free_data)(void *),
    int (*compare_to)(cl_list_node_t *, cl_list_node_t *),
    int (*filter)(cl_list_node_t *, void *),
    int (*equals)(cl_list_node_t *, cl_list_node_t *))
{
    return (cl_list_t *)cglist_create(CL_OBJ_LIST, free_data, compare_to,
                                      filter, equals);
}

int cl_list_destroy(cl_list_t *list)
{
    return cglist_destroy((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_size(const cl_list_t *list)
{
    return cglist_size((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_push(cl_list_t *list, const void *node_content,
    unsigned int size)
{
    return cglist_push((cl_list_t *)list, CL_OBJ_LIST, node_content, size,
                       CL_OBJ_LIST_NODE);
}

cl_list_node_t *cl_list_pop(cl_list_t *list)
{
    return (cl_list_node_t *)cglist_pop((cl_list_t *)list, CL_OBJ_LIST);
}

cl_list_node_t *cl_list_shift(cl_list_t *list)
{
    return (cl_list_node_t *)cglist_shift((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_unshift(cl_list_t *list, const void *node_content,
    unsigned int size)
{
    return cglist_unshift((cl_list_t *)list, CL_OBJ_LIST, node_content, size,
                          CL_OBJ_LIST_NODE);
}

cl_list_node_t *cl_list_map(const cl_list_t *list,
    int (*foo)(cl_list_node_t *, void *), void *data)
{
    return (cl_list_node_t *)cglist_map((cl_list_t *)list, CL_OBJ_LIST,
                                        CL_OBJ_LIST_NODE, foo, data);
}

cl_list_node_t *cl_list_map_indexed(const cl_list_t *list,
    int (*foo)(unsigned int, cl_list_node_t *, void *), void *data)
{
    return (cl_list_node_t *)cglist_map_indexed((cl_list_t *)list, CL_OBJ_LIST,
                                                CL_OBJ_LIST_NODE, foo, data);
}

cl_list_node_t *cl_list_map_reverse(const cl_list_t *list,
    int (*foo)(cl_list_node_t *, void *), void *data)
{
    return (cl_list_node_t *)cglist_map_reverse((cl_list_t *)list, CL_OBJ_LIST,
                                                CL_OBJ_LIST_NODE, foo, data);
}

cl_list_node_t *cl_list_map_reverse_indexed(const cl_list_t *list,
    int (*foo)(unsigned int, cl_list_node_t *, void *), void *data)
{
    return (cl_list_node_t *)cglist_map_reverse_indexed((cl_list_t *)list,
                                                        CL_OBJ_LIST,
                                                        CL_OBJ_LIST_NODE, foo,
                                                        data);
}

cl_list_node_t *cl_list_at(const cl_list_t *list, unsigned int index)
{
    return (cl_list_node_t *)cglist_at((cl_list_t *)list, CL_OBJ_LIST,
                                       CL_OBJ_LIST_NODE, index);
}

int cl_list_delete(cl_list_t *list, void *data)
{
    return cglist_delete((cl_list_t *)list, CL_OBJ_LIST, data);
}

int cl_list_delete_indexed(cl_list_t *list, unsigned int index)
{
    return cglist_delete_indexed((cl_list_t *)list, CL_OBJ_LIST, index);
}

cl_list_t *cl_list_move(cl_list_t *list)
{
    return (cl_list_t *)cglist_move((cl_list_t *)list, CL_OBJ_LIST);
}

cl_list_t *cl_list_filter(cl_list_t *list, void *data)
{
    return (cl_list_t *)cglist_filter((cl_list_t *)list, CL_OBJ_LIST, data);
}

int cl_list_sort(cl_list_t *list)
{
    return cglist_sort((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_indexof(const cl_list_t *list, void *element,
    unsigned int size)
{
    return cglist_indexof((cl_list_t *)list, CL_OBJ_LIST, element, size,
                          CL_OBJ_LIST_NODE);
}

int cl_list_last_indexof(const cl_list_t *list, void *element,
    unsigned int size)
{
    return cglist_last_indexof((cl_list_t *)list, CL_OBJ_LIST, element, size,
                               CL_OBJ_LIST_NODE);
}

bool cl_list_contains(const cl_list_t *list, void *element,
    unsigned int size)
{
    return cglist_contains((cl_list_t *)list, CL_OBJ_LIST, element, size,
                           CL_OBJ_LIST_NODE);
}

cl_list_node_t *cl_list_peek(const cl_list_t *list)
{
    return (cl_list_node_t *)cglist_peek((cl_list_t *)list, CL_OBJ_LIST,
                                         CL_OBJ_LIST_NODE);
}

bool cl_list_is_empty(const cl_list_t *list)
{
    return cglist_is_empty((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_set_compare_to(const cl_list_t *list,
    int (*compare_to)(cl_list_node_t *, cl_list_node_t *))
{
    return cglist_set_compare_to((cl_list_t *)list, CL_OBJ_LIST, compare_to);
}

int cl_list_set_filter(const cl_list_t *list,
    int (*filter)(cl_list_node_t *, void *))
{
    return cglist_set_filter((cl_list_t *)list, CL_OBJ_LIST, filter);
}

int cl_list_set_equals(const cl_list_t *list,
    int (*equals)(cl_list_node_t *, cl_list_node_t *))
{
    return cglist_set_equals((cl_list_t *)list, CL_OBJ_LIST, equals);
}

cl_list_node_t *cl_list_middle(const cl_list_t *list)
{
    return (cl_list_node_t *)cglist_middle((cl_list_t *)list, CL_OBJ_LIST);
}

int cl_list_rotate(cl_list_t *list, unsigned int n)
{
    return cglist_rotate(list, CL_OBJ_LIST, n);
}

