
/*
 * Description: API to handle queues.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Dec  7 10:15:29 BRST 2016
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

#ifndef _COLLECTIONS_CL_QUEUE_H
#define _COLLECTIONS_CL_QUEUE_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_queue.h> directly; include <collections.h> instead."
# endif
#endif

#define cqueue_node_ref(node)       \
    (cqueue_queue_t *)cglist_node_ref((cqueue_queue_t *)node, CQUEUE_NODE)

#define cqueue_node_unref(node)     \
    cglist_node_unref((cqueue_queue_t *)node, CQUEUE_NODE)

#define cqueue_ref(queue)           \
    (cqueue_t *)cglist_ref((cqueue_t *)queue, CQUEUE)

#define cqueue_unref(queue)         \
    cglist_unref((cqueue_t *)queue, CQUEUE)

#define cqueue_create(free_data, compare_to, filter, equals)    \
    (cqueue_t *)cglist_create(CQUEUE, free_data, compare_to, filter, equals)

#define cqueue_destroy(queue)       \
    cqueue_unref(queue)

#define cqueue_size(queue)          \
    cglist_size((cqueue_t *)queue, CQUEUE)

#define cqueue_enqueue(queue)       \
    cglist_shift((cqueue_t *)queue, CQUEUE)

#define cqueue_dequeue(queue, node_content)                     \
    cglist_unshift((cqueue_t *)queue, CQUEUE, node_content, CQUEUE_NODE)

#define cqueue_map(queue, foo, data)                            \
    cglist_map((cqueue_t *)queue, CQUEUE, foo, data)

#define cqueue_map_indexed(queue, foo, data)                    \
    cglist_map_indexed((cqueue_t *)queue, CQUEUE, foo, data)

#define cqueue_map_reverse(queue, foo, data)                    \
    cglist_map_reverse((cqueue_t *)queue, CQUEUE, foo, data)

#define cqueue_map_reverse_indexed(queue, foo, data)            \
    cglist_map_reverse_indexed((cqueue_t *)queue, CQUEUE, foo, data)

#define cqueue_at(queue, index)     \
    cglist_at((cqueue_t *)queue, CQUEUE, index)

#define cqueue_delete(queue, data)  \
    cglist_delete((cqueue_t *)queue, CQUEUE, data)

#define cqueue_delete_indexed(queue, index)                     \
    cglist_delete_indexed((cqueue_t *)queue, CQUEUE, index)

#define cqueue_move(queue)          \
    cglist_move((cqueue_t *)queue, CQUEUE)

#define cqueue_filter(queue, data)  \
    cglist_filter((cqueue_t *)queue, CQUEUE, data)

#define cqueue_node_content(node)   \
    cglist_node_content((cqueue_queue_t *)node, CQUEUE_NODE)

#define cqueue_sort(queue)          \
    cglist_sort((cqueue_t *)queue, CQUEUE)

#define cqueue_indexof(queue, content)                          \
    cglist_indexof((cqueue_t *)queue, CQUEUE, content, CQUEUE_NODE)

#define cqueue_last_indexof(queue, content)                     \
    cglist_last_indexof((cqueue_t *)queue, CQUEUE, content, CQUEUE_NODE)

#define cqueue_contains(queue, content)                         \
    cglist_contains((cqueue_t *)queue, CQUEUE, content, CQUEUE_NODE)

#endif

