
/*
 * Description: API to handle linked lists.
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

#ifndef _COLLECTIONS_CL_LIST_H
#define _COLLECTIONS_CL_LIST_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_list.h> directly; include <collections.h> instead."
# endif
#endif

#define clist_node_ref(node)        \
    (clist_node_t *)cglist_node_ref((clist_node_t *)node, CLIST_NODE)

#define clist_node_unref(node)      \
    cglist_node_unref((clist_node_t *)node, CLIST_NODE)

#define clist_ref(list)             \
    (clist_t *)cglist_ref((clist_t *)list, CLIST)

#define clist_unref(list)           \
    cglist_unref((clist_t *)list, CLIST)

#define clist_create(free_data, compare_to, filter, equals) \
    (clist_t *)cglist_create(CLIST, free_data, compare_to, filter, equals)

#define clist_destroy(list)         \
    clist_unref(list)

#define clist_size(list)            \
    cglist_size((clist_t *)list, CLIST)

#define clist_push(list, node_content)                      \
    cglist_push((clist_t *)list, CLIST, node_content, CLIST_NODE)

#define clist_pop(list)             \
    cglist_pop((clist_t *)list, CLIST)

#define clist_shift(list)           \
    cglist_shift((clist_t *)list, CLIST)

#define clist_unshift(list, node_content)                   \
    cglist_unshift((clist_t *)list, CLIST, node_content, CLIST_NODE)

#define clist_map(list, foo, data)  \
    cglist_map((clist_t *)list, CLIST, foo, data)

#define clist_map_indexed(list, foo, data)                  \
    cglist_map_indexed((clist_t *)list, CLIST, foo, data)

#define clist_map_reverse(list, foo, data)                  \
    cglist_map_reverse((clist_t *)list, CLIST, foo, data)

#define clist_map_reverse_indexed(list, foo, data)          \
    cglist_map_reverse_indexed((clist_t *)list, CLIST, foo, data)

#define clist_at(list, index)       \
    cglist_at((clist_t *)list, CLIST, index)

#define clist_delete(list, data)    \
    cglist_delete((clist_t *)list, CLIST, data)

#define clist_delete_indexed(list, index)                   \
    cglist_delete_indexed((clist_t *)list, CLIST, index)

#define clist_move(list)            \
    cglist_move((clist_t *)list, CLIST)

#define clist_filter(list, data)    \
    cglist_filter((clist_t *)list, CLIST, data)

#define clist_node_content(node)    \
    cglist_node_content((clist_node_t *)node, CLIST_NODE)

#define clist_sort(list)            \
    cglist_sort((clist_t *)list, CLIST)

#define clist_indexof(list, content)                        \
    cglist_indexof((clist_t *)list, CLIST, content, CLIST_NODE)

#define clist_last_indexof(list, content)                   \
    cglist_last_indexof((clist_t *)list, CLIST, content, CLIST_NODE)

#define clist_contains(list, content)                       \
    cglist_contains((clist_t *)list, CLIST, content, CLIST_NODE)

#endif

