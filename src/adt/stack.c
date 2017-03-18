
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

__PUB_API__ void *cl_stack_node_content(cl_stack_node_t *node)
{
    return cglist_node_content(node, CL_OBJ_STACK_NODE);
}

__PUB_API__ cl_stack_node_t *cl_stack_node_ref(cl_stack_node_t *node)
{
    return (cl_stack_node_t *)cglist_node_ref(node, CL_OBJ_STACK_NODE);
}

__PUB_API__ int cl_stack_node_unref(cl_stack_node_t *node)
{
    return cglist_node_unref(node, CL_OBJ_STACK_NODE);
}

__PUB_API__ cl_stack_t *cl_stack_ref(cl_stack_t *stack)
{
    return (cl_stack_t *)cglist_ref((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ int cl_stack_unref(cl_stack_t *stack)
{
    return cglist_unref((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ cl_stack_t *cl_stack_create(void (*free_data)(void *),
    int (*compare_to)(cl_stack_node_t *, cl_stack_node_t *),
    int (*filter)(cl_stack_node_t *, void *),
    int (*equals)(cl_stack_node_t *, cl_stack_node_t *))
{
    return (cl_stack_t *)cglist_create(CL_OBJ_STACK, free_data, compare_to,
                                       filter, equals);
}

__PUB_API__ int cl_stack_destroy(cl_stack_t *stack)
{
    return cglist_destroy((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ int cl_stack_size(const cl_stack_t *stack)
{
    return cglist_size((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ int cl_stack_push(cl_stack_t *stack, const void *node_content,
    unsigned int size)
{
    return cglist_push((cl_stack_t *)stack, CL_OBJ_STACK, node_content, size,
                       CL_OBJ_STACK_NODE);
}

__PUB_API__ cl_stack_node_t *cl_stack_pop(cl_stack_t *stack)
{
    return (cl_stack_node_t *)cglist_shift((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ cl_stack_node_t *cl_stack_map(const cl_stack_t *stack,
    int (*foo)(cl_stack_node_t *, void *), void *data)
{
    return (cl_stack_node_t *)cglist_map((cl_stack_t *)stack, CL_OBJ_STACK,
                                         CL_OBJ_STACK_NODE, foo, data);
}

__PUB_API__ cl_stack_node_t *cl_stack_map_indexed(const cl_stack_t *stack,
    int (*foo)(unsigned int, cl_stack_node_t *, void *), void *data)
{
    return (cl_stack_node_t *)cglist_map_indexed((cl_stack_t *)stack,
                                                 CL_OBJ_STACK,
                                                 CL_OBJ_STACK_NODE, foo, data);
}

__PUB_API__ cl_stack_node_t *cl_stack_map_reverse(const cl_stack_t *stack,
    int (*foo)(cl_stack_node_t *, void *), void *data)
{
    return (cl_stack_node_t *)cglist_map_reverse((cl_stack_t *)stack,
                                                 CL_OBJ_STACK,
                                                 CL_OBJ_STACK_NODE, foo, data);
}

__PUB_API__ cl_stack_node_t *cl_stack_map_reverse_indexed(const cl_stack_t *stack,
    int (*foo)(unsigned int, cl_stack_node_t *, void *), void *data)
{
    return (cl_stack_node_t *)cglist_map_reverse_indexed((cl_stack_t *)stack,
                                                         CL_OBJ_STACK,
                                                         CL_OBJ_STACK_NODE, foo,
                                                         data);
}

__PUB_API__ cl_stack_node_t *cl_stack_at(const cl_stack_t *stack,
    unsigned int index)
{
    return (cl_stack_node_t *)cglist_at((cl_stack_t *)stack, CL_OBJ_STACK,
                                        CL_OBJ_STACK_NODE, index);
}

__PUB_API__ int cl_stack_delete(cl_stack_t *stack, void *data)
{
    return cglist_delete((cl_stack_t *)stack, CL_OBJ_STACK, data);
}

__PUB_API__ int cl_stack_delete_indexed(cl_stack_t *stack, unsigned int index)
{
    return cglist_delete_indexed((cl_stack_t *)stack, CL_OBJ_STACK, index);
}

__PUB_API__ cl_stack_t *cl_stack_move(cl_stack_t *stack)
{
    return (cl_stack_t *)cglist_move((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ cl_stack_t *cl_stack_filter(cl_stack_t *stack, void *data)
{
    return (cl_stack_t *)cglist_filter((cl_stack_t *)stack, CL_OBJ_STACK, data);
}

__PUB_API__ int cl_stack_sort(cl_stack_t *stack)
{
    return cglist_sort((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ int cl_stack_indexof(const cl_stack_t *stack, void *element,
    unsigned int size)
{
    return cglist_indexof((cl_stack_t *)stack, CL_OBJ_STACK, element, size,
                          CL_OBJ_STACK_NODE);
}

__PUB_API__ int cl_stack_last_indexof(const cl_stack_t *stack, void *element,
    unsigned int size)
{
    return cglist_last_indexof((cl_stack_t *)stack, CL_OBJ_STACK, element, size,
                               CL_OBJ_STACK_NODE);
}

__PUB_API__ bool cl_stack_contains(const cl_stack_t *stack, void *element,
    unsigned int size)
{
    return cglist_contains((cl_stack_t *)stack, CL_OBJ_STACK, element, size,
                           CL_OBJ_STACK_NODE);
}

__PUB_API__ cl_stack_node_t *cl_stack_peek(const cl_stack_t *stack)
{
    return (cl_stack_node_t *)cglist_peek((cl_stack_t *)stack, CL_OBJ_STACK,
                                          CL_OBJ_STACK_NODE);
}

__PUB_API__ bool cl_stack_is_empty(const cl_stack_t *stack)
{
    return cglist_is_empty((cl_stack_t *)stack, CL_OBJ_STACK);
}

__PUB_API__ int cl_stack_set_compare_to(const cl_stack_t *stack,
    int (*compare_to)(cl_stack_node_t *, cl_stack_node_t *))
{
    return cglist_set_compare_to((cl_stack_t *)stack, CL_OBJ_STACK, compare_to);
}

__PUB_API__ int cl_stack_set_filter(const cl_stack_t *stack,
    int (*filter)(cl_stack_node_t *, void *))
{
    return cglist_set_filter((cl_stack_t *)stack, CL_OBJ_STACK, filter);
}

__PUB_API__ int cl_stack_set_equals(const cl_stack_t *stack,
    int (*equals)(cl_stack_node_t *, cl_stack_node_t *))
{
    return cglist_set_equals((cl_stack_t *)stack, CL_OBJ_STACK, equals);
}

