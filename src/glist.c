
/*
 * Description:
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

/*
 * TODO:
 *
 * Add API to change internal list functions like compare_to, filter and
 * equals on the fly.
 */

#include <stdlib.h>

#include <pthread.h>

#include "collections.h"

struct glist_node_s {
    clist_entry_t   *prev;
    clist_entry_t   *next;
    void            *p;
};

#define clist_members                                                       \
    cl_struct_member(struct glist_node_s *, list)                           \
    cl_struct_member(unsigned int, size)                                    \
    cl_struct_member(struct ref_s, ref)                                     \
    cl_struct_member(void, (*free_data)(void *))                            \
    cl_struct_member(int, (*compare_to)(clist_node_t *, clist_node_t *))    \
    cl_struct_member(int, (*filter)(clist_node_t *, void *))                \
    cl_struct_member(int, (*equals)(clist_node_t *, clist_node_t *))        \
    cl_struct_member(pthread_mutex_t, lock)

cl_struct_declare(clist_s, clist_members);

#define clist_s     cl_struct(clist_s)

/*
 * Duplicate internal clist_s members to another one.
 */
static void dup_internal_data(clist_s *orig, clist_s *dest)
{
    if ((NULL == orig) || (NULL == dest))
        return;

    dest->size = orig->size;
    dest->free_data = orig->free_data;
    dest->filter = orig->filter;
    dest->compare_to = orig->compare_to;
    dest->equals = orig->equals;
}

/*
 * Checks if we're holding cobjects inside the list. Returns true if does.
 */
static bool is_list_of_cobjects(clist_s *list)
{
    struct glist_node_s *node = NULL;

    node = cdll_at(list->list, 0);

    if (NULL == node)
        return false;

    return validate_object(node->p, COBJECT);
}

/*
 * If we have a list of cobjects, this function uses its compare_to function
 * to compare two cobjects.
 */
static int compare_cobjects(void *a, void *b)
{
    struct glist_node_s *node_a = (struct glist_node_s *)a;
    struct glist_node_s *node_b = (struct glist_node_s *)b;
    cobject_t *ob1 = (cobject_t *)node_a->p;
    cobject_t *ob2 = (cobject_t *)node_b->p;

    return cobject_compare_to(ob1, ob2);
}

/*
 * If we have a list of cobjects, this functions uses its equals function to
 * compare two cobjects.
 */
static int cobjects_are_equal(void *a, void *b)
{
    struct glist_node_s *node_a = (struct glist_node_s *)a;
    struct glist_node_s *node_b = (struct glist_node_s *)b;
    cobject_t *ob1 = (cobject_t *)node_a->p;
    cobject_t *ob2 = (cobject_t *)node_b->p;

    return cobject_equals(ob1, ob2);
}

/*
 * Releases a struct glist_node_s from memory without releasing its content,
 * so we can still use it somewhere.
 */
static void release_node(void *p)
{
    struct glist_node_s *node = (struct glist_node_s *)p;

    if (NULL == p)
        return;

    free(node);
}

/*
 * Creates a new struct glist_node_s with @content inside.
 */
static struct glist_node_s *new_node(void *content)
{
    struct glist_node_s *n = NULL;

    n = calloc(1, sizeof(struct glist_node_s));

    if (NULL == n) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    n->p = content;

    return n;
}

/*
 * Destroy a struct glist_node_s from memory. The function checks which _free_
 * function will be used according the type of a node content.
 */
static int destroy_node(void *a, void *b)
{
    struct glist_node_s *node = (struct glist_node_s *)a;
    clist_s *list = (clist_s *)b;

    if (list->free_data != NULL)
        (list->free_data)(node->p);
    else {
        /*
         * If we're holding cobject_t pointers we know how to destroy them.
         */
        if (validate_object(node->p, COBJECT) == true)
            cobject_destroy(node->p);
        else
            free(node->p);
    }

    return 0;
}

/*
 * Destroy a clist_s from memory. Releasing all internal nodes and its
 * respectives content.
 */
static void destroy_list(const struct ref_s *ref)
{
    clist_s *list = container_of(ref, clist_s, ref);
    struct glist_node_s *p = NULL;

    if (NULL == list)
        return;

    while ((p = cdll_pop(&list->list)) != NULL) {
        destroy_node(p, list);
        release_node(p);
    }

    pthread_mutex_destroy(&list->lock);
    free(list);
}

/*
 * Creates a new clist_s object, starting its reference count.
 */
static clist_s *new_clist(void)
{
    clist_s *l = NULL;

    l = calloc(1, sizeof(clist_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->size = 0;
    pthread_mutex_init(&l->lock, NULL);
    set_typeof(CLIST, l);

    l->ref.free = destroy_list;
    l->ref.count = 1;

    return l;
}

clist_t LIBEXPORT *clist_ref(clist_t *list)
{
    clist_s *l = (clist_s *)list;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    ref_inc(&l->ref);

    return list;
}

int LIBEXPORT clist_unref(clist_t *list)
{
    clist_s *l = (clist_s *)list;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    ref_dec(&l->ref);

    return 0;
}

clist_t LIBEXPORT *clist_create(void (*free_data)(void *),
    int (*compare_to)(clist_node_t *, clist_node_t *),
    int (*filter)(clist_node_t *, void *),
    int (*equals)(clist_node_t *, clist_node_t *))
{
    clist_s *l = NULL;

    cerrno_clear();
    l = new_clist();

    if (NULL == l)
        return NULL;

    if (free_data != NULL)
        l->free_data = free_data;

    if (compare_to != NULL)
        l->compare_to = compare_to;

    if (filter != NULL)
        l->filter = filter;

    if (equals != NULL)
        l->equals = equals;

    return l;
}

int LIBEXPORT clist_destroy(clist_t *list)
{
    return clist_unref(list);
}

int LIBEXPORT clist_size(const clist_t *list)
{
    clist_s *l = (clist_s *)list;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    return l->size;
}

int LIBEXPORT clist_push(clist_t *list, void *node_content)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    node = new_node(node_content);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cdll_push(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void LIBEXPORT *clist_pop(clist_t *list)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;
    void *p = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    pthread_mutex_lock(&l->lock);
    node = cdll_pop(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    p = node->p;
    release_node(node);

    /* Warning, the user is responsible for releasing @p */
    return (is_list_of_cobjects(l) == true) ? cobject_ref(p) : p;
}

void LIBEXPORT *clist_shift(clist_t *list)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;
    void *p = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    pthread_mutex_lock(&l->lock);
    node = cdll_shift(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    p = node->p;
    release_node(node);

    /* Warning, the user is responsible for releasing @p */
    return (is_list_of_cobjects(l) == true) ? cobject_ref(p) : p;
}

int LIBEXPORT clist_unshift(clist_t *list, void *node_content)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    node = new_node(node_content);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cdll_unshift(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void LIBEXPORT *clist_map(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->p) : node->p;
}

void LIBEXPORT *clist_map_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_indexed(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->p) : node->p;
}

void LIBEXPORT *clist_map_reverse(const clist_t *list,
    int (*foo)(clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_reverse(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->p) : node->p;
}

void LIBEXPORT *clist_map_reverse_indexed(const clist_t *list,
    int (*foo)(unsigned int, clist_node_t *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    node = cdll_map_indexed_reverse(l->list, foo, data);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->p) : node->p;
}

void LIBEXPORT *clist_at(const clist_t *list, unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    node = cdll_at(l->list, index);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->p) : node->p;
}

/* TODO: Maybe return the number of extracted elements */
int LIBEXPORT clist_delete(clist_t *list, void *data)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    pthread_mutex_lock(&l->lock);
    node = cdll_delete(l->list, l->filter, data, NULL);

    if (node)
        destroy_node(list, node);

    pthread_mutex_unlock(&l->lock);

    return 0;
}

int LIBEXPORT clist_delete_indexed(clist_t *list, unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    pthread_mutex_lock(&l->lock);
    node = cdll_delete_indexed(l->list, index, NULL);

    if (node)
        destroy_node(list, node);

    pthread_mutex_unlock(&l->lock);

    return 0;
}

clist_t LIBEXPORT *clist_move(clist_t *list)
{
    clist_s *l = (clist_s *)list, *n = NULL;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    n = new_clist();

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

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return NULL;

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    n = new_clist();

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
    struct glist_node_s *n = (struct glist_node_s *)node;

    if (NULL == node)
        return NULL;

    return n->p;
}

int LIBEXPORT clist_sort(clist_t *list)
{
    clist_s *l = (clist_s *)list;
    bool list_cobjects;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

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

static int get_indexof(const clist_t *list, void *object, bool last_index)
{
    clist_s *l = (clist_s *)list;
    struct glist_node_s *node = NULL;
    bool list_cobjects;
    int idx;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(object);

    if (NULL == node)
        return -1;

    if (last_index == false) {
        idx = cdll_indexof(l->list, node,
                           (list_cobjects == true) ? cobjects_are_equal
                                                   : l->equals);
    } else
        idx = cdll_last_indexof(l->list, node,
                                (list_cobjects == true) ? cobjects_are_equal
                                                        : l->equals);

    release_node(node);

    return idx;
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
    struct glist_node_s *node = NULL;
    bool st;

    cerrno_clear();

    if (validate_object(list, CLIST) == false)
        return -1;

    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(object);

    if (NULL == node)
        return -1;

    st = cdll_contains(l->list, node,
                       (list_cobjects == true) ? cobjects_are_equal
                                               : l->equals);

    release_node(node);

    return st;
}

