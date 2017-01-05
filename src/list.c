
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

void LIBEXPORT *clist_node_content(clist_node_t *node)
{
    return cglist_node_content(node, CLIST_NODE);
}

clist_node_t LIBEXPORT *clist_node_ref(clist_node_t *node)
{
    return (clist_node_t *)cglist_node_ref(node, CLIST_NODE);
}

int LIBEXPORT clist_node_unref(clist_node_t *node)
{
    return cglist_node_unref(node, CLIST_NODE);
}

clist_t LIBEXPORT *clist_ref(clist_t *list)
{
    return (clist_t *)cglist_ref((clist_t *)list, CLIST);
}

int LIBEXPORT clist_unref(clist_t *list)
{
    return cglist_unref((clist_t *)list, CLIST);
}

clist_t LIBEXPORT *clist_create(void (*free_data)(void *),
    int (*compare_to)(clist_node_t *, clist_node_t *),
    int (*filter)(clist_node_t *, void *),
    int (*equals)(clist_node_t *, clist_node_t *))
{
    return (clist_t *)cglist_create(CLIST, free_data, compare_to, filter,
                                    equals);
}

int LIBEXPORT clist_destroy(clist_t *list)
{
    return cglist_destroy((clist_t *)list, CLIST);
}

int LIBEXPORT clist_size(const clist_t *list)
{
    return cglist_size((clist_t *)list, CLIST);
}

int LIBEXPORT clist_push(clist_t *list, const void *node_content,
    unsigned int size)
{
    return cglist_push((clist_t *)list, CLIST, node_content, size, CLIST_NODE);
}

clist_node_t LIBEXPORT *clist_pop(clist_t *list)
{
    return (clist_node_t *)cglist_pop((clist_t *)list, CLIST);
}

clist_node_t LIBEXPORT *clist_shift(clist_t *list)
{
    return (clist_node_t *)cglist_shift((clist_t *)list, CLIST);
}

int LIBEXPORT clist_unshift(clist_t *list, const void *node_content,
    unsigned int size)
{
    return cglist_unshift((clist_t *)list, CLIST, node_content, size, CLIST_NODE);
}

clist_node_t LIBEXPORT *clist_map(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    return (clist_node_t *)cglist_map((clist_t *)list, CLIST, CLIST_NODE, foo,
                                      data);
}

clist_node_t LIBEXPORT *clist_map_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    return (clist_node_t *)cglist_map_indexed((clist_t *)list, CLIST,
                                              CLIST_NODE, foo, data);
}

clist_node_t LIBEXPORT *clist_map_reverse(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    return (clist_node_t *)cglist_map_reverse((clist_t *)list, CLIST,
                                              CLIST_NODE, foo, data);
}

clist_node_t LIBEXPORT *clist_map_reverse_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    return (clist_node_t *)cglist_map_reverse_indexed((clist_t *)list, CLIST,
                                                      CLIST_NODE, foo, data);
}

clist_node_t LIBEXPORT *clist_at(const clist_t *list, unsigned int index)
{
    return (clist_node_t *)cglist_at((clist_t *)list, CLIST, CLIST_NODE, index);
}

int LIBEXPORT clist_delete(clist_t *list, void *data)
{
    return cglist_delete((clist_t *)list, CLIST, data);
}

int LIBEXPORT clist_delete_indexed(clist_t *list, unsigned int index)
{
    return cglist_delete_indexed((clist_t *)list, CLIST, index);
}

clist_t LIBEXPORT *clist_move(clist_t *list)
{
    return (clist_t *)cglist_move((clist_t *)list, CLIST);
}

clist_t LIBEXPORT *clist_filter(clist_t *list, void *data)
{
    return (clist_t *)cglist_filter((clist_t *)list, CLIST, data);
}

int LIBEXPORT clist_sort(clist_t *list)
{
    return cglist_sort((clist_t *)list, CLIST);
}

int LIBEXPORT clist_indexof(const clist_t *list, void *element,
    unsigned int size)
{
    return cglist_indexof((clist_t *)list, CLIST, element, size, CLIST_NODE);
}

int LIBEXPORT clist_last_indexof(const clist_t *list, void *element,
    unsigned int size)
{
    return cglist_last_indexof((clist_t *)list, CLIST, element, size, CLIST_NODE);
}

bool LIBEXPORT clist_contains(const clist_t *list, void *element,
    unsigned int size)
{
    return cglist_contains((clist_t *)list, CLIST, element, size, CLIST_NODE);
}

clist_node_t LIBEXPORT *clist_peek(const clist_t *list)
{
    return (clist_node_t *)cglist_peek((clist_t *)list, CLIST, CLIST_NODE);
}

bool LIBEXPORT clist_is_empty(const clist_t *list)
{
    return cglist_is_empty((clist_t *)list, CLIST);
}

int LIBEXPORT clist_set_compare_to(const clist_t *list,
    int (*compare_to)(clist_node_t *, clist_node_t *))
{
    return cglist_set_compare_to((clist_t *)list, CLIST, compare_to);
}

int LIBEXPORT clist_set_filter(const clist_t *list,
    int (*filter)(clist_node_t *, void *))
{
    return cglist_set_filter((clist_t *)list, CLIST, filter);
}

int LIBEXPORT clist_set_equals(const clist_t *list,
    int (*equals)(clist_node_t *, clist_node_t *))
{
    return cglist_set_equals((clist_t *)list, CLIST, equals);
}

