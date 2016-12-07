
/*
 * Description: API to handle stacks.
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

#ifndef _COLLECTIONS_CL_STACK_H
#define _COLLECTIONS_CL_STACK_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_stack.h> directly; include <collections.h> instead."
# endif
#endif

#define cstack_node_ref(node)       \
    (cstack_node_t *)cglist_node_ref((cstack_node_t *)node, CSTACK_NODE)

#define cstack_node_unref(node)     \
    cglist_node_unref((cstack_node_t *)node, CSTACK_NODE)

#define cstack_ref(stack)           \
    (cstack_t *)cglist_ref((cstack_t *)stack, CSTACK)

#define cstack_unref(stack)         \
    cglist_unref((cstack_t *)stack, CSTACK)

#define cstack_create(free_data, compare_to, filter, equals)    \
    (cstack_t *)cglist_create(CSTACK, free_data, compare_to, filter, equals)

#define cstack_destroy(stack)       \
    cstack_unref(stack)

#define cstack_size(stack)          \
    cglist_size((cstack_t *)stack, CSTACK)

#define cstack_push(stack, node_content)                        \
    cglist_push((cstack_t *)stack, CSTACK, node_content, CSTACK_NODE)

#define cstack_pop(stack)           \
    cglist_pop((cstack_t *)stack, CSTACK)

#define cstack_map(stack, foo, data)                            \
    cglist_map((cstack_t *)stack, CSTACK, foo, data)

#define cstack_map_indexed(stack, foo, data)                    \
    cglist_map_indexed((cstack_t *)stack, CSTACK, foo, data)

#define cstack_map_reverse(stack, foo, data)                    \
    cglist_map_reverse((cstack_t *)stack, CSTACK, foo, data)

#define cstack_map_reverse_indexed(stack, foo, data)            \
    cglist_map_reverse_indexed((cstack_t *)stack, CSTACK, foo, data)

#define cstack_at(stack, index)     \
    cglist_at((cstack_t *)stack, CSTACK, index)

#define cstack_delete(stack, data)  \
    cglist_delete((cstack_t *)stack, CSTACK, data)

#define cstack_delete_indexed(stack, index)                     \
    cglist_delete_indexed((cstack_t *)stack, CSTACK, index)

#define cstack_move(stack)          \
    cglist_move((cstack_t *)stack, CSTACK)

#define cstack_filter(stack, data)  \
    cglist_filter((cstack_t *)stack, CSTACK, data)

#define cstack_node_content(node)   \
    cglist_node_content((cstack_node_t *)node, CSTACK_NODE)

#define cstack_sort(stack)          \
    cglist_sort((cstack_t *)stack, CSTACK)

#define cstack_indexof(stack, content)                          \
    cglist_indexof((cstack_t *)stack, CSTACK, content, CSTACK_NODE)

#define cstack_last_indexof(stack, content)                     \
    cglist_last_indexof((cstack_t *)stack, CSTACK, content, CSTACK_NODE)

#define cstack_contains(stack, content)                         \
    cglist_contains((cstack_t *)stack, CSTACK, content, CSTACK_NODE)

#endif

