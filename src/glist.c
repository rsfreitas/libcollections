
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

/*
 * TODO:
 *
 * Add API to change internal list functions like compare_to, filter and
 * equals on the fly.
 *
 * peek
 */

#include <stdlib.h>

#include <pthread.h>

#include "collections.h"

struct gnode_s {
    clist_entry_t       *prev;
    clist_entry_t       *next;
    struct cobject_hdr  hdr;
    void                *content;
    struct ref_s        ref;

    /*
     * We save the pointer to the node free function here so we don't need
     * the list when release one.
     */
    void                (*free_data)(void *);
};

#define CLIST_NODE_OFFSET           \
    (sizeof(clist_entry_t *) + sizeof(clist_entry_t *))

#define clist_members                                                       \
    cl_struct_member(struct gnode_s *, list)                           \
    cl_struct_member(unsigned int, size)                                    \
    cl_struct_member(struct ref_s, ref)                                     \
    cl_struct_member(void, (*free_data)(void *))                            \
    cl_struct_member(int, (*compare_to)(void *, void *))    \
    cl_struct_member(int, (*filter)(void *, void *))                \
    cl_struct_member(int, (*equals)(void *, void *))        \
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
    struct gnode_s *node = NULL;

    node = cdll_at(list->list, 0);

    if (NULL == node)
        return false;

    return validate_object(node->content, COBJECT);
}

/*
 * If we have a list of cobjects, this function uses its compare_to function
 * to compare two cobjects.
 */
static int compare_cobjects(void *a, void *b)
{
    struct gnode_s *node_a = (struct gnode_s *)a;
    struct gnode_s *node_b = (struct gnode_s *)b;
    cobject_t *ob1 = (cobject_t *)node_a->content;
    cobject_t *ob2 = (cobject_t *)node_b->content;

    return cobject_compare_to(ob1, ob2);
}

/*
 * If we have a list of cobjects, this functions uses its equals function to
 * compare two cobjects.
 */
static int cobjects_are_equal(void *a, void *b)
{
    struct gnode_s *node_a = (struct gnode_s *)a;
    struct gnode_s *node_b = (struct gnode_s *)b;
    cobject_t *ob1 = (cobject_t *)node_a->content;
    cobject_t *ob2 = (cobject_t *)node_b->content;

    return cobject_equals(ob1, ob2);
}

/*
 * Releases a struct gnode_s from memory. Its content will also be released
 * if @free_content is true. In this case the function checks which _free_
 * function will be used according the type of a node content.
 */
static void destroy_node(struct gnode_s *node, bool free_content)
{
    if (NULL == node)
        return;

    if (free_content == true) {
        if (node->free_data != NULL)
            (node->free_data)(node->content);
        else {
            /*
             * If we're holding cobject_t pointers we know how to destroy them.
             */
            if (validate_object(node->content, COBJECT) == true)
                cobject_destroy(node->content);
            else
                free(node->content);
        }
    }

    free(node);
}

/*
 * The function to release a node called when a reference count from them drops
 * to 0.
 */
static void __destroy_node(const struct ref_s *ref)
{
    struct gnode_s *node = container_of(ref, struct gnode_s, ref);

    destroy_node(node, true);
}

/*
 * Creates a new struct gnode_s with @content inside.
 */
static struct gnode_s *new_node(const void *content, clist_s *list,
    enum cl_object object)
{
    struct gnode_s *n = NULL;

    n = calloc(1, sizeof(struct gnode_s));

    if (NULL == n) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    n->content = (void *)content;
    n->free_data = list->free_data;
    n->ref.free = __destroy_node;
    n->ref.count = 1;

    set_typeof_with_offset(object, n, CLIST_NODE_OFFSET);

    return n;
}

/*
 * Destroy a clist_s from memory. Releasing all internal nodes and its
 * respectives content.
 */
static void destroy_list(const struct ref_s *ref)
{
    clist_s *list = container_of(ref, clist_s, ref);
    enum cl_object node_object = CLIST_NODE;
    struct gnode_s *p = NULL;

    if (NULL == list)
        return;

    if (validate_object(list, CSTACK))
        node_object = CSTACK_NODE;
    else if (validate_object(list, CQUEUE))
        node_object = CQUEUE_NODE;

    while ((p = cdll_pop(&list->list)) != NULL)
        cglist_node_unref(p, node_object);

    pthread_mutex_destroy(&list->lock);
    free(list);
}

/*
 * Creates a new clist_s object, starting its reference count.
 */
static clist_s *new_clist(enum cl_object object)
{
    clist_s *l = NULL;

    l = calloc(1, sizeof(clist_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->size = 0;
    pthread_mutex_init(&l->lock, NULL);
    set_typeof(object, l);

    l->ref.free = destroy_list;
    l->ref.count = 1;

    return l;
}

void LIBEXPORT *cglist_node_ref(void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, NULL);
    ref_inc(&n->ref);

    return node;
}

int LIBEXPORT cglist_node_unref(void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, -1);
    ref_dec(&n->ref);

    return 0;
}

void LIBEXPORT *cglist_ref(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;

    __clib_function_init__(true, list, object, NULL);
    ref_inc(&l->ref);

    return list;
}

int LIBEXPORT cglist_unref(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;

    __clib_function_init__(true, list, object, -1);
    ref_dec(&l->ref);

    return 0;
}

void LIBEXPORT *cglist_create(enum cl_object object, void (*free_data)(void *),
    int (*compare_to)(void *, void *), int (*filter)(void *, void *),
    int (*equals)(void *, void *))
{
    clist_s *l = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    l = new_clist(object);

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

int LIBEXPORT cglist_destroy(void *list, enum cl_object object)
{
    return cglist_unref(list, object);
}

int LIBEXPORT cglist_size(const void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;

    __clib_function_init__(true, list, object, -1);

    return l->size;
}

int LIBEXPORT cglist_push(void *list, enum cl_object object,
    const void *node_content, enum cl_object node_object)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);
    node = new_node(node_content, l, node_object);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cdll_push(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void LIBEXPORT *cglist_pop(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;
    void *p = NULL;

    __clib_function_init__(true, list, object, NULL);

    pthread_mutex_lock(&l->lock);
    node = cdll_pop(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    p = node->content;
    destroy_node(node, false);

    /* Warning, the user is responsible for releasing @p */
    return (is_list_of_cobjects(l) == true) ? cobject_ref(p) : p;
}

void LIBEXPORT *cglist_shift(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;
    void *content = NULL;

    __clib_function_init__(true, list, object, NULL);

    pthread_mutex_lock(&l->lock);
    node = cdll_shift(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    /* We save the node content to return it soon. */
    content = node->content;
    destroy_node(node, false);

    /* Warning, the user is responsible for releasing @p */
    return (is_list_of_cobjects(l) == true) ? cobject_ref(content) : content;
}

int LIBEXPORT cglist_unshift(void *list, enum cl_object object,
    const void *node_content, enum cl_object node_object)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);
    node = new_node(node_content, l, node_object);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cdll_unshift(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void LIBEXPORT *cglist_map(const void *list, enum cl_object object,
    int (*foo)(void *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

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

void LIBEXPORT *cglist_map_indexed(const void *list, enum cl_object object,
    int (*foo)(unsigned int, void *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

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

void LIBEXPORT *cglist_map_reverse(const void *list, enum cl_object object,
    int (*foo)(void *, void *), void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

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

void LIBEXPORT *cglist_map_reverse_indexed(const void *list,
    enum cl_object object, int (*foo)(unsigned int, void *, void *),
    void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

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

void LIBEXPORT *cglist_at(const void *list, enum cl_object object,
    unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);
    node = cdll_at(l->list, index);

    if (NULL == node)
        return NULL;

    return (is_list_of_cobjects(l) == true) ? cobject_ref(node->content)
                                            : node->content;
}

/* TODO: Maybe return the number of deleted elements */
int LIBEXPORT cglist_delete(void *list, enum cl_object object, void *data)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);

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

int LIBEXPORT cglist_delete_indexed(void *list, enum cl_object object,
    unsigned int index)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);

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

void LIBEXPORT *cglist_move(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list, *n = NULL;

    __clib_function_init__(true, list, object, NULL);
    n = new_clist(object);

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

void LIBEXPORT *cglist_filter(void *list, enum cl_object object, void *data)
{
    clist_s *l = (clist_s *)list, *n = NULL;

    __clib_function_init__(true, list, object, NULL);

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    n = new_clist(object);

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
void LIBEXPORT *cglist_node_content(const void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, NULL);

    return n->content;
}

int LIBEXPORT cglist_sort(void *list, enum cl_object object)
{
    clist_s *l = (clist_s *)list;
    bool list_cobjects;

    __clib_function_init__(true, list, object, -1);
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

static int get_indexof(const void *list, enum cl_object object, void *content,
    enum cl_object node_content, bool last_index)
{
    clist_s *l = (clist_s *)list;
    struct gnode_s *node = NULL;
    bool list_cobjects;
    int idx;

    __clib_function_init__(true, list, object, -1);
    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(content, l, node_content);

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

    destroy_node(node, false);

    return idx;
}

int LIBEXPORT cglist_indexof(const void *list, enum cl_object object,
    void *content, enum cl_object node_object)
{
    return get_indexof(list, object, content, node_object, false);
}

int LIBEXPORT cglist_last_indexof(const void *list, enum cl_object object,
    void *content, enum cl_object node_object)
{
    return get_indexof(list, object, content, node_object, true);
}

bool LIBEXPORT cglist_contains(const void *list, enum cl_object object,
    void *content, enum cl_object node_object)
{
    clist_s *l = (clist_s *)list;
    bool list_cobjects;
    struct gnode_s *node = NULL;
    bool st;

    __clib_function_init__(true, list, object, -1);
    list_cobjects = is_list_of_cobjects(l);

    if ((list_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(content, l, node_object);

    if (NULL == node)
        return -1;

    st = cdll_contains(l->list, node,
                       (list_cobjects == true) ? cobjects_are_equal
                                               : l->equals);

    destroy_node(node, false);

    return st;
}

