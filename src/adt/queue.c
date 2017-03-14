
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

__PUB_API__ void *cqueue_node_content(cqueue_node_t *node)
{
    return cglist_node_content(node, CQUEUE_NODE);
}

__PUB_API__ cqueue_node_t *cqueue_node_ref(cqueue_node_t *node)
{
    return (cqueue_node_t *)cglist_node_ref(node, CQUEUE_NODE);
}

__PUB_API__ int cqueue_node_unref(cqueue_node_t *node)
{
    return cglist_node_unref(node, CQUEUE_NODE);
}

__PUB_API__ cqueue_t *cqueue_ref(cqueue_t *queue)
{
    return (cqueue_t *)cglist_ref((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ int cqueue_unref(cqueue_t *queue)
{
    return cglist_unref((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ cqueue_t *cqueue_create(void (*free_data)(void *),
    int (*compare_to)(cqueue_node_t *, cqueue_node_t *),
    int (*filter)(cqueue_node_t *, void *),
    int (*equals)(cqueue_node_t *, cqueue_node_t *))
{
    return (cqueue_t *)cglist_create(CQUEUE, free_data, compare_to, filter,
                                    equals);
}

__PUB_API__ int cqueue_destroy(cqueue_t *queue)
{
    return cglist_destroy((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ int cqueue_size(const cqueue_t *queue)
{
    return cglist_size((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ cqueue_node_t *cqueue_dequeue(cqueue_t *queue)
{
    return (clist_node_t *)cglist_pop((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ int cqueue_enqueue(cqueue_t *queue, const void *node_content,
    unsigned int size)
{
    return cglist_unshift((cqueue_t *)queue, CQUEUE, node_content, size,
                          CQUEUE_NODE);
}

__PUB_API__ cqueue_node_t *cqueue_map(const cqueue_t *queue,
    int (*foo)(cqueue_node_t *, void *), void *data)
{
    return (cqueue_node_t *)cglist_map((cqueue_t *)queue, CQUEUE, CQUEUE_NODE,
                                       foo, data);
}

__PUB_API__ cqueue_node_t *cqueue_map_indexed(const cqueue_t *queue,
    int (*foo)(unsigned int, cqueue_node_t *, void *), void *data)
{
    return (cqueue_node_t *)cglist_map_indexed((cqueue_t *)queue, CQUEUE,
                                               CQUEUE_NODE, foo, data);
}

__PUB_API__ cqueue_node_t *cqueue_map_reverse(const cqueue_t *queue,
    int (*foo)(cqueue_node_t *, void *), void *data)
{
    return (cqueue_node_t *)cglist_map_reverse((cqueue_t *)queue, CQUEUE,
                                               CQUEUE_NODE, foo, data);
}

__PUB_API__ cqueue_node_t *cqueue_map_reverse_indexed(const cqueue_t *queue,
    int (*foo)(unsigned int, cqueue_node_t *, void *), void *data)
{
    return (cqueue_node_t *)cglist_map_reverse_indexed((cqueue_t *)queue, CQUEUE,
                                                       CQUEUE_NODE, foo, data);
}

__PUB_API__ cqueue_node_t *cqueue_at(const cqueue_t *queue, unsigned int index)
{
    return (cqueue_node_t *)cglist_at((cqueue_t *)queue, CQUEUE, CQUEUE_NODE,
                                      index);
}

__PUB_API__ int cqueue_delete(cqueue_t *queue, void *data)
{
    return cglist_delete((cqueue_t *)queue, CQUEUE, data);
}

__PUB_API__ int cqueue_delete_indexed(cqueue_t *queue, unsigned int index)
{
    return cglist_delete_indexed((cqueue_t *)queue, CQUEUE, index);
}

__PUB_API__ cqueue_t *cqueue_move(cqueue_t *queue)
{
    return (cqueue_t *)cglist_move((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ cqueue_t *cqueue_filter(cqueue_t *queue, void *data)
{
    return (cqueue_t *)cglist_filter((cqueue_t *)queue, CQUEUE, data);
}

__PUB_API__ int cqueue_sort(cqueue_t *queue)
{
    return cglist_sort((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ int cqueue_indexof(const cqueue_t *queue, void *element,
    unsigned int size)
{
    return cglist_indexof((cqueue_t *)queue, CQUEUE, element, size, CQUEUE_NODE);
}

__PUB_API__ int cqueue_last_indexof(const cqueue_t *queue, void *element,
    unsigned int size)
{
    return cglist_last_indexof((cqueue_t *)queue, CQUEUE, element, size,
                               CQUEUE_NODE);
}

__PUB_API__ bool cqueue_contains(const cqueue_t *queue, void *element,
    unsigned int size)
{
    return cglist_contains((cqueue_t *)queue, CQUEUE, element, size, CQUEUE_NODE);
}

__PUB_API__ cqueue_node_t *cqueue_front(const cqueue_t *queue)
{
    return (cqueue_node_t *)cglist_peek((cqueue_t *)queue, CQUEUE, CQUEUE_NODE);
}

__PUB_API__ bool cqueue_is_empty(const cqueue_t *queue)
{
    return cglist_is_empty((cqueue_t *)queue, CQUEUE);
}

__PUB_API__ int cqueue_set_compare_to(const cqueue_t *queue,
    int (*compare_to)(cqueue_node_t *, cqueue_node_t *))
{
    return cglist_set_compare_to((cqueue_t *)queue, CQUEUE, compare_to);
}

__PUB_API__ int cqueue_set_filter(const cqueue_t *queue,
    int (*filter)(cqueue_node_t *, void *))
{
    return cglist_set_filter((cqueue_t *)queue, CQUEUE, filter);
}

__PUB_API__ int cqueue_set_equals(const cqueue_t *queue,
    int (*equals)(cqueue_node_t *, cqueue_node_t *))
{
    return cglist_set_equals((cqueue_t *)queue, CQUEUE, equals);
}


