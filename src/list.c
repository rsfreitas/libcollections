
/*
 * Description: API do handle lists with an opaque object.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Sep  2 14:42:57 2016
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

#include <pthread.h>

#include "collections.h"

clist_node_t LIBEXPORT *clist_node_ref(clist_node_t *node)
{
    return glist_node_ref(node, CLIST_NODE);
}

int LIBEXPORT clist_node_unref(clist_node_t *node)
{
    return glist_node_unref(node, CLIST_NODE);
}

clist_t LIBEXPORT *clist_ref(clist_t *list)
{
    return glist_ref(list, CLIST);
}

int LIBEXPORT clist_unref(clist_t *list)
{
    return glist_unref(list, CLIST);
}

clist_t LIBEXPORT *clist_create(void (*free_data)(void *),
    int (*compare_to)(clist_node_t *, clist_node_t *),
    int (*filter)(clist_node_t *, void *),
    int (*equals)(clist_node_t *, clist_node_t *))
{
    return glist_create(CLIST, free_data, compare_to, filter, equals);
}

int LIBEXPORT clist_destroy(clist_t *list)
{
    return clist_unref(list);
}

int LIBEXPORT clist_size(const clist_t *list)
{
    return glist_size(list, CLIST);
}

int LIBEXPORT clist_push(clist_t *list, void *node_content)
{
    return glist_push(list, CLIST, node_content, CLIST_NODE);
}

void LIBEXPORT *clist_pop(clist_t *list)
{
    return glist_pop(list, CLIST);
}

void LIBEXPORT *clist_shift(clist_t *list)
{
    return glist_shift(list, CLIST);
}

int LIBEXPORT clist_unshift(clist_t *list, void *node_content)
{
    return glist_unshift(list, CLIST, node_content, CLIST_NODE);
}

void LIBEXPORT *clist_map(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    return glist_map(list, CLIST,
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

void LIBEXPORT *clist_map_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_indexed(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

void LIBEXPORT *clist_map_reverse(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_reverse(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

void LIBEXPORT *clist_map_reverse_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_indexed_reverse(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

void LIBEXPORT *clist_at(const clist_t *list, unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, NULL);
    node = cdll_at(l->list, index);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

/* TODO: Maybe return the number of deleted elements */
int LIBEXPORT clist_delete(clist_t *list, void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, -1);

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    pthread_mutex_lock(&l->lock);
    node = cdll_delete(l->list, l->filter, data, NULL);

    if (node) {
        /*
         * Since the node is removed from the list we need to really remove it
         * from the memory.
         */
        destroy_node(node, true);
    }

    pthread_mutex_unlock(&l->lock);

    return 0;
}

int LIBEXPORT clist_delete_indexed(clist_t *list, unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, CLIST, -1);

    pthread_mutex_lock(&l->lock);
    node = cdll_delete_indexed(l->list, index, NULL);

    if (node) {
        /*
         * Since the node is removed from the list we need to really remove it
         * from the memory.
         */
        destroy_node(node, true);
    }

    pthread_mutex_unlock(&l->lock);

    return 0;
}

clist_t LIBEXPORT *clist_move(clist_t *list)
{
    clist_s *l = (clist_s *)list, *n = NULL;

    __clib_function_init__(true, list, CLIST, NULL);
    n = new_clist(CLIST);

    if (NULL == n)
        return NULL;

    pthread_mutex_lock(&l->lock);
    dup_internal_data(l, n);
    n->list = cdll_move(l->list);

    /*
     * Since we just move all nodes to another list, we mark its size as
     * zero.
     */
    l->size = 0;
    pthread_mutex_unlock(&l->lock);

    return n;
}

clist_t LIBEXPORT *clist_filter(clist_t *list, void *data)
{
    clist_s *l = (clist_s *)list, *n = NULL;

    __clib_function_init__(true, list, CLIST, NULL);

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    n = new_clist(CLIST);

    if (NULL == n)
        return NULL;

    pthread_mutex_lock(&l->lock);
    dup_internal_data(l, n);
    n->list = cdll_filter(&l->list, l->filter, data);
    n->size = cdll_size(n->list);
    l->size -= n->size;
    pthread_mutex_unlock(&l->lock);

    return n;
}

/*
 * This function must be used by the user to get a reference to its own
 * object from events function, like compare_to, filter, equals and
 * functions passed to _map_ functions.
 */
void LIBEXPORT *clist_node_content(const clist_node_t *node)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == node) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return n->content;
}

int LIBEXPORT clist_sort(clist_t *list)
{
    clist_s *l = (clist_s *)list;
    bool list_cobjects;

    __clib_function_init__(true, list, CLIST, -1);
    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->compare_to)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    pthread_mutex_lock(&l->lock);
    l->list = cdll_mergesort(l->list,
                             (list_cobjects == true) ? compare_cobjects
                                                     : l->compare_to);

    pthread_mutex_unlock(&l->lock);

    return 0;
}

int LIBEXPORT clist_indexof(const clist_t *list, void *object)
{
    return get_indexof(list, object, false);
}

int LIBEXPORT clist_last_indexof(const clist_t *list, void *object)
{
    return get_indexof(list, object, true);
}

bool LIBEXPORT clist_contains(const clist_t *list, void *object)
{
    clist_s *l = (clist_s *)list;
    bool list_cobjects;
    struct gnode_s *node = NULL;
    bool st;

    __clib_function_init__(true, list, CLIST, -1);
    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(object, l, CLIST_NODE);

    if (NULL == node)
        return -1;

    st = cdll_contains(l->list, node,
                       (list_cobjects == true) ? cobjects_are_equal
                                               : l->equals);

    destroy_node(node, false);

    return st;
}

