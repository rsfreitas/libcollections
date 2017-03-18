
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

__PUB_API__ void *cl_queue_node_content(cl_queue_node_t *node)
{
    return cglist_node_content(node, CL_OBJ_QUEUE_NODE);
}

__PUB_API__ cl_queue_node_t *cl_queue_node_ref(cl_queue_node_t *node)
{
    return (cl_queue_node_t *)cglist_node_ref(node, CL_OBJ_QUEUE_NODE);
}

__PUB_API__ int cl_queue_node_unref(cl_queue_node_t *node)
{
    return cglist_node_unref(node, CL_OBJ_QUEUE_NODE);
}

__PUB_API__ cl_queue_t *cl_queue_ref(cl_queue_t *queue)
{
    return (cl_queue_t *)cglist_ref((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ int cl_queue_unref(cl_queue_t *queue)
{
    return cglist_unref((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ cl_queue_t *cl_queue_create(void (*free_data)(void *),
    int (*compare_to)(cl_queue_node_t *, cl_queue_node_t *),
    int (*filter)(cl_queue_node_t *, void *),
    int (*equals)(cl_queue_node_t *, cl_queue_node_t *))
{
    return (cl_queue_t *)cglist_create(CL_OBJ_QUEUE, free_data, compare_to,
                                       filter, equals);
}

__PUB_API__ int cl_queue_destroy(cl_queue_t *queue)
{
    return cglist_destroy((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ int cl_queue_size(const cl_queue_t *queue)
{
    return cglist_size((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ cl_queue_node_t *cl_queue_dequeue(cl_queue_t *queue)
{
    return (cl_list_node_t *)cglist_pop((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ int cl_queue_enqueue(cl_queue_t *queue, const void *node_content,
    unsigned int size)
{
    return cglist_unshift((cl_queue_t *)queue, CL_OBJ_QUEUE, node_content, size,
                          CL_OBJ_QUEUE_NODE);
}

__PUB_API__ cl_queue_node_t *cl_queue_map(const cl_queue_t *queue,
    int (*foo)(cl_queue_node_t *, void *), void *data)
{
    return (cl_queue_node_t *)cglist_map((cl_queue_t *)queue, CL_OBJ_QUEUE,
                                         CL_OBJ_QUEUE_NODE, foo, data);
}

__PUB_API__ cl_queue_node_t *cl_queue_map_indexed(const cl_queue_t *queue,
    int (*foo)(unsigned int, cl_queue_node_t *, void *), void *data)
{
    return (cl_queue_node_t *)cglist_map_indexed((cl_queue_t *)queue,
                                                 CL_OBJ_QUEUE,
                                                 CL_OBJ_QUEUE_NODE, foo, data);
}

__PUB_API__ cl_queue_node_t *cl_queue_map_reverse(const cl_queue_t *queue,
    int (*foo)(cl_queue_node_t *, void *), void *data)
{
    return (cl_queue_node_t *)cglist_map_reverse((cl_queue_t *)queue,
                                                 CL_OBJ_QUEUE,
                                                 CL_OBJ_QUEUE_NODE, foo, data);
}

__PUB_API__ cl_queue_node_t *cl_queue_map_reverse_indexed(const cl_queue_t *queue,
    int (*foo)(unsigned int, cl_queue_node_t *, void *), void *data)
{
    return (cl_queue_node_t *)cglist_map_reverse_indexed((cl_queue_t *)queue,
                                                         CL_OBJ_QUEUE,
                                                         CL_OBJ_QUEUE_NODE, foo,
                                                         data);
}

__PUB_API__ cl_queue_node_t *cl_queue_at(const cl_queue_t *queue,
    unsigned int index)
{
    return (cl_queue_node_t *)cglist_at((cl_queue_t *)queue, CL_OBJ_QUEUE,
                                        CL_OBJ_QUEUE_NODE, index);
}

__PUB_API__ int cl_queue_delete(cl_queue_t *queue, void *data)
{
    return cglist_delete((cl_queue_t *)queue, CL_OBJ_QUEUE, data);
}

__PUB_API__ int cl_queue_delete_indexed(cl_queue_t *queue, unsigned int index)
{
    return cglist_delete_indexed((cl_queue_t *)queue, CL_OBJ_QUEUE, index);
}

__PUB_API__ cl_queue_t *cl_queue_move(cl_queue_t *queue)
{
    return (cl_queue_t *)cglist_move((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ cl_queue_t *cl_queue_filter(cl_queue_t *queue, void *data)
{
    return (cl_queue_t *)cglist_filter((cl_queue_t *)queue, CL_OBJ_QUEUE, data);
}

__PUB_API__ int cl_queue_sort(cl_queue_t *queue)
{
    return cglist_sort((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ int cl_queue_indexof(const cl_queue_t *queue, void *element,
    unsigned int size)
{
    return cglist_indexof((cl_queue_t *)queue, CL_OBJ_QUEUE, element, size,
                          CL_OBJ_QUEUE_NODE);
}

__PUB_API__ int cl_queue_last_indexof(const cl_queue_t *queue, void *element,
    unsigned int size)
{
    return cglist_last_indexof((cl_queue_t *)queue, CL_OBJ_QUEUE, element, size,
                               CL_OBJ_QUEUE_NODE);
}

__PUB_API__ bool cl_queue_contains(const cl_queue_t *queue, void *element,
    unsigned int size)
{
    return cglist_contains((cl_queue_t *)queue, CL_OBJ_QUEUE, element, size,
                           CL_OBJ_QUEUE_NODE);
}

__PUB_API__ cl_queue_node_t *cl_queue_front(const cl_queue_t *queue)
{
    return (cl_queue_node_t *)cglist_peek((cl_queue_t *)queue, CL_OBJ_QUEUE,
                                          CL_OBJ_QUEUE_NODE);
}

__PUB_API__ bool cl_queue_is_empty(const cl_queue_t *queue)
{
    return cglist_is_empty((cl_queue_t *)queue, CL_OBJ_QUEUE);
}

__PUB_API__ int cl_queue_set_compare_to(const cl_queue_t *queue,
    int (*compare_to)(cl_queue_node_t *, cl_queue_node_t *))
{
    return cglist_set_compare_to((cl_queue_t *)queue, CL_OBJ_QUEUE, compare_to);
}

__PUB_API__ int cl_queue_set_filter(const cl_queue_t *queue,
    int (*filter)(cl_queue_node_t *, void *))
{
    return cglist_set_filter((cl_queue_t *)queue, CL_OBJ_QUEUE, filter);
}

__PUB_API__ int cl_queue_set_equals(const cl_queue_t *queue,
    int (*equals)(cl_queue_node_t *, cl_queue_node_t *))
{
    return cglist_set_equals((cl_queue_t *)queue, CL_OBJ_QUEUE, equals);
}

