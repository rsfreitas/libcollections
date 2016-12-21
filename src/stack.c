
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

void LIBEXPORT *cstack_node_content(cstack_node_t *node)
{
    return cglist_node_content(node, CSTACK_NODE);
}

cstack_node_t LIBEXPORT *cstack_node_ref(cstack_node_t *node)
{
    return (cstack_node_t *)cglist_node_ref(node, CSTACK_NODE);
}

int LIBEXPORT cstack_node_unref(cstack_node_t *node)
{
    return cglist_node_unref(node, CSTACK_NODE);
}

cstack_t LIBEXPORT *cstack_ref(cstack_t *stack)
{
    return (cstack_t *)cglist_ref((cstack_t *)stack, CSTACK);
}

int LIBEXPORT cstack_unref(cstack_t *stack)
{
    return cglist_unref((cstack_t *)stack, CSTACK);
}

cstack_t LIBEXPORT *cstack_create(void (*free_data)(void *),
    int (*compare_to)(cstack_node_t *, cstack_node_t *),
    int (*filter)(cstack_node_t *, void *),
    int (*equals)(cstack_node_t *, cstack_node_t *))
{
    return (cstack_t *)cglist_create(CSTACK, free_data, compare_to, filter,
                                    equals);
}

int LIBEXPORT cstack_destroy(cstack_t *stack)
{
    return cglist_destroy((cstack_t *)stack, CSTACK);
}

int LIBEXPORT cstack_size(const cstack_t *stack)
{
    return cglist_size((cstack_t *)stack, CSTACK);
}

int LIBEXPORT cstack_push(cstack_t *stack, const void *node_content,
    unsigned int size)
{
    return cglist_push((cstack_t *)stack, CSTACK, node_content, size,
                       CSTACK_NODE);
}

cstack_node_t LIBEXPORT *cstack_pop(cstack_t *stack)
{
    return (cstack_node_t *)cglist_shift((cstack_t *)stack, CSTACK);
}

cstack_node_t LIBEXPORT *cstack_map(const cstack_t *stack,
    int (*foo)(cstack_node_t *, void *), void *data)
{
    return (cstack_node_t *)cglist_map((cstack_t *)stack, CSTACK, foo, data);
}

cstack_node_t LIBEXPORT *cstack_map_indexed(const cstack_t *stack,
    int (*foo)(unsigned int, cstack_node_t *, void *), void *data)
{
    return (cstack_node_t *)cglist_map_indexed((cstack_t *)stack, CSTACK, foo,
                                               data);
}

cstack_node_t LIBEXPORT *cstack_map_reverse(const cstack_t *stack,
    int (*foo)(cstack_node_t *, void *), void *data)
{
    return (cstack_node_t *)cglist_map_reverse((cstack_t *)stack, CSTACK, foo,
                                               data);
}

cstack_node_t LIBEXPORT *cstack_map_reverse_indexed(const cstack_t *stack,
    int (*foo)(unsigned int, cstack_node_t *, void *), void *data)
{
    return (cstack_node_t *)cglist_map_reverse_indexed((cstack_t *)stack, CSTACK,
                                                      foo, data);
}

cstack_node_t LIBEXPORT *cstack_at(const cstack_t *stack, unsigned int index)
{
    return (cstack_node_t *)cglist_at((cstack_t *)stack, CSTACK, index);
}

int LIBEXPORT cstack_delete(cstack_t *stack, void *data)
{
    return cglist_delete((cstack_t *)stack, CSTACK, data);
}

int LIBEXPORT cstack_delete_indexed(cstack_t *stack, unsigned int index)
{
    return cglist_delete_indexed((cstack_t *)stack, CSTACK, index);
}

cstack_t LIBEXPORT *cstack_move(cstack_t *stack)
{
    return (cstack_t *)cglist_move((cstack_t *)stack, CSTACK);
}

cstack_t LIBEXPORT *cstack_filter(cstack_t *stack, void *data)
{
    return (cstack_t *)cglist_filter((cstack_t *)stack, CSTACK, data);
}

int LIBEXPORT cstack_sort(cstack_t *stack)
{
    return cglist_sort((cstack_t *)stack, CSTACK);
}

int LIBEXPORT cstack_indexof(const cstack_t *stack, void *element,
    unsigned int size)
{
    return cglist_indexof((cstack_t *)stack, CSTACK, element, size, CSTACK_NODE);
}

int LIBEXPORT cstack_last_indexof(const cstack_t *stack, void *element,
    unsigned int size)
{
    return cglist_last_indexof((cstack_t *)stack, CSTACK, element, size,
                               CSTACK_NODE);
}

bool LIBEXPORT cstack_contains(const cstack_t *stack, void *element,
    unsigned int size)
{
    return cglist_contains((cstack_t *)stack, CSTACK, element, size, CSTACK_NODE);
}

cstack_node_t LIBEXPORT *cstack_peek(const cstack_t *stack)
{
    return (cstack_node_t *)cglist_peek((cstack_t *)stack, CSTACK, CSTACK_NODE);
}

bool LIBEXPORT cstack_is_empty(const cstack_t *stack)
{
    return cglist_is_empty((cstack_t *)stack, CSTACK);
}

int LIBEXPORT cstack_set_compare_to(const cstack_t *stack,
    int (*compare_to)(cstack_node_t *, cstack_node_t *))
{
    return cglist_set_compare_to((cstack_t *)stack, CSTACK, compare_to);
}

int LIBEXPORT cstack_set_filter(const cstack_t *stack,
    int (*filter)(cstack_node_t *, void *))
{
    return cglist_set_filter((cstack_t *)stack, CSTACK, filter);
}

int LIBEXPORT cstack_set_equals(const cstack_t *stack,
    int (*equals)(cstack_node_t *, cstack_node_t *))
{
    return cglist_set_equals((cstack_t *)stack, CSTACK, equals);
}

