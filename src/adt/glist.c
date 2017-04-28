
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

struct gnode_s {
    cl_list_entry_t         *prev;
    cl_list_entry_t         *next;
    struct cl_object_hdr    hdr;
    void                    *content;
    unsigned int            content_size;
    struct cl_ref_s         ref;

    /*
     * We save the pointer to the node free function here so we don't need
     * the list when release one.
     */
    void                (*free_data)(void *);
};

#define CLIST_NODE_OFFSET           \
    (sizeof(cl_list_entry_t *) + sizeof(cl_list_entry_t *))

#define clist_members                                                       \
    cl_struct_member(struct gnode_s *, list)                                \
    cl_struct_member(unsigned int, size)                                    \
    cl_struct_member(struct cl_ref_s, ref)                                  \
    cl_struct_member(void, (*free_data)(void *))                            \
    cl_struct_member(int, (*compare_to)(void *, void *))                    \
    cl_struct_member(int, (*filter)(void *, void *))                        \
    cl_struct_member(int, (*equals)(void *, void *))                        \
    cl_struct_member(pthread_mutex_t, lock)

cl_struct_declare(glist_s, clist_members);

#define glist_s     cl_struct(glist_s)

/*
 * Duplicate internal glist_s members to another one.
 */
static void dup_internal_data(glist_s *orig, glist_s *dest)
{
    if ((NULL == orig) || (NULL == dest))
        return;

    dest->size = orig->size;
    dest->free_data = orig->free_data;
    dest->filter = orig->filter;
    dest->compare_to = orig->compare_to;
    dest->equals = orig->equals;
}

static bool is_cobject(struct gnode_s *node)
{
    if (NULL == node)
        return false;

    /*
     * If we're holding information smaller than a cl_object_hdr structure we
     * don't even need to validate it.
     */
    if (node->content_size < CL_OBJECT_HEADER_ID_SIZE)
        return false;

    return validate_object(node->content, CL_OBJ_OBJECT);
}

/*
 * Checks if we're holding cobjects inside the list. Returns true if does.
 */
static bool is_list_of_cobjects(glist_s *list)
{
    struct gnode_s *node = NULL;

    node = cl_dll_at(list->list, 0);

    return is_cobject(node);
}

/*
 * If we have a list of cobjects, this function uses its compare_to function
 * to compare two cobjects.
 */
static int compare_cobjects(void *a, void *b)
{
    struct gnode_s *node_a = (struct gnode_s *)a;
    struct gnode_s *node_b = (struct gnode_s *)b;
    cl_object_t *ob1 = (cl_object_t *)node_a->content;
    cl_object_t *ob2 = (cl_object_t *)node_b->content;

    return cl_object_compare_to(ob1, ob2);
}

/*
 * If we have a list of cobjects, this functions uses its equals function to
 * compare two cobjects.
 */
static int cobjects_are_equal(void *a, void *b)
{
    struct gnode_s *node_a = (struct gnode_s *)a;
    struct gnode_s *node_b = (struct gnode_s *)b;
    cl_object_t *ob1 = (cl_object_t *)node_a->content;
    cl_object_t *ob2 = (cl_object_t *)node_b->content;

    return cl_object_equals(ob1, ob2);
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
             * If we're holding information smaller than a cl_object_hdr structure
             * we don't even need to validate it and call free on it.
             */
            if (node->content_size < CL_OBJECT_HEADER_ID_SIZE)
                free(node->content);
            else {
                /*
                 * If we're holding cl_object_t pointers we know how to destroy
                 * them.
                 */
                if (validate_object(node->content, CL_OBJ_OBJECT) == true)
                    cl_object_destroy(node->content);
            }
        }
    }

    free(node);
    node = NULL;
}

/*
 * The function to release a node called when a reference count from them drops
 * to 0.
 */
static void __destroy_node(const struct cl_ref_s *ref)
{
    struct gnode_s *node = cl_container_of(ref, struct gnode_s, ref);

    destroy_node(node, true);
}

/*
 * Creates a new struct gnode_s with @content inside.
 */
static struct gnode_s *new_node(const void *content, unsigned int content_size,
    glist_s *list, enum cl_object object)
{
    struct gnode_s *n = NULL;

    n = calloc(1, sizeof(struct gnode_s));

    if (NULL == n) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    n->content = (void *)content;
    n->content_size = content_size;
    n->free_data = list->free_data;
    n->ref.free = __destroy_node;
    n->ref.count = 1;

    set_typeof_with_offset(object, n, CLIST_NODE_OFFSET);

    return n;
}

/*
 * Destroy a glist_s from memory. Releasing all internal nodes and its
 * respectives content.
 */
static void destroy_list(const struct cl_ref_s *ref)
{
    glist_s *list = cl_container_of(ref, glist_s, ref);
    enum cl_object node_object = CL_OBJ_LIST_NODE;
    struct gnode_s *p = NULL;

    if (NULL == list)
        return;

    if (validate_object(list, CL_OBJ_STACK))
        node_object = CL_OBJ_STACK_NODE;
    else if (validate_object(list, CL_OBJ_QUEUE))
        node_object = CL_OBJ_QUEUE_NODE;

    while ((p = cl_dll_pop(&list->list)) != NULL)
        cglist_node_unref(p, node_object);

    pthread_mutex_destroy(&list->lock);
    free(list);
    list = NULL;
}

/*
 * Creates a new glist_s object, starting its reference count.
 */
static glist_s *new_clist(enum cl_object object)
{
    glist_s *l = NULL;

    l = calloc(1, sizeof(glist_s));

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

void *cglist_node_ref(void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, NULL);
    cl_ref_inc(&n->ref);

    return node;
}

int cglist_node_unref(void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, -1);
    cl_ref_dec(&n->ref);

    return 0;
}

void *cglist_ref(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, NULL);
    cl_ref_inc(&l->ref);

    return list;
}

int cglist_unref(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, -1);
    cl_ref_dec(&l->ref);

    return 0;
}

void *cglist_create(enum cl_object object, void (*free_data)(void *),
    int (*compare_to)(void *, void *), int (*filter)(void *, void *),
    int (*equals)(void *, void *))
{
    glist_s *l = NULL;

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

int cglist_destroy(void *list, enum cl_object object)
{
    return cglist_unref(list, object);
}

int cglist_size(const void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, -1);

    return l->size;
}

int cglist_push(void *list, enum cl_object object,
    const void *node_content, unsigned int size, enum cl_object node_object)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);
    node = new_node(node_content, size, l, node_object);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cl_dll_push(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void *cglist_pop(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    pthread_mutex_lock(&l->lock);
    node = cl_dll_pop(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return node;
}

void *cglist_shift(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    pthread_mutex_lock(&l->lock);
    node = cl_dll_shift(&l->list);

    if (node)
        l->size--;

    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return node;
}

int cglist_unshift(void *list, enum cl_object object,
    const void *node_content, unsigned int size, enum cl_object node_object)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);
    node = new_node(node_content, size, l, node_object);

    if (NULL == node)
        return -1;

    pthread_mutex_lock(&l->lock);
    l->list = cl_dll_unshift(l->list, node);
    l->size++;
    pthread_mutex_unlock(&l->lock);

    return 0;
}

void *cglist_map(const void *list, enum cl_object object,
    enum cl_object node_object, int (*foo)(void *, void *), void *data)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    pthread_mutex_lock(&l->lock);
    node = cl_dll_map(l->list, foo, data);
    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

void *cglist_map_indexed(const void *list, enum cl_object object,
    enum cl_object node_object, int (*foo)(unsigned int, void *, void *),
    void *data)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    pthread_mutex_lock(&l->lock);
    node = cl_dll_map_indexed(l->list, foo, data);
    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

void *cglist_map_reverse(const void *list, enum cl_object object,
    enum cl_object node_object, int (*foo)(void *, void *), void *data)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    pthread_mutex_lock(&l->lock);
    node = cl_dll_map_reverse(l->list, foo, data);
    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

void *cglist_map_reverse_indexed(const void *list,
    enum cl_object object, enum cl_object node_object,
    int (*foo)(unsigned int, void *, void *), void *data)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    pthread_mutex_lock(&l->lock);
    node = cl_dll_map_indexed_reverse(l->list, foo, data);
    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

void *cglist_at(const void *list, enum cl_object object,
    enum cl_object node_object, unsigned int index)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, NULL);
    pthread_mutex_lock(&l->lock);
    node = cl_dll_at(l->list, index);
    pthread_mutex_unlock(&l->lock);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

/* TODO: Maybe return the number of deleted elements */
int cglist_delete(void *list, enum cl_object object, void *data)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);

    if (NULL == l->filter) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    pthread_mutex_lock(&l->lock);
    node = cl_dll_delete(l->list, l->filter, data, NULL);

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

int cglist_delete_indexed(void *list, enum cl_object object,
    unsigned int index)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;

    __clib_function_init__(true, list, object, -1);

    pthread_mutex_lock(&l->lock);
    node = cl_dll_delete_indexed(l->list, index, NULL);

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

void *cglist_move(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list, *n = NULL;

    __clib_function_init__(true, list, object, NULL);
    n = new_clist(object);

    if (NULL == n)
        return NULL;

    pthread_mutex_lock(&l->lock);
    dup_internal_data(l, n);
    n->list = cl_dll_move(l->list);

    /*
     * Since we just move all nodes to another list, we mark its size as
     * zero.
     */
    l->size = 0;
    pthread_mutex_unlock(&l->lock);

    return n;
}

void *cglist_filter(void *list, enum cl_object object, void *data)
{
    glist_s *l = (glist_s *)list, *n = NULL;

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
    n->list = cl_dll_filter(&l->list, l->filter, data);
    n->size = cl_dll_size(n->list);
    l->size -= n->size;
    pthread_mutex_unlock(&l->lock);

    return n;
}

/*
 * This function must be used by the user to get a reference to its own
 * object from within events function, like compare_to, filter, equals and
 * functions passed to _map_ functions.
 */
void *cglist_node_content(const void *node, enum cl_object object)
{
    struct gnode_s *n = (struct gnode_s *)node;

    __clib_function_init_ex__(true, node, object, CLIST_NODE_OFFSET, NULL);

    return (is_cobject(n) == true) ? cl_object_ref(n->content)
                                   : n->content;
}

int cglist_sort(void *list, enum cl_object object)
{
    glist_s *l = (glist_s *)list;
    bool list_of_cobjects;

    __clib_function_init__(true, list, object, -1);
    list_of_cobjects = is_list_of_cobjects(l);

    if ((list_of_cobjects == false) && (NULL == l->compare_to)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    pthread_mutex_lock(&l->lock);
    l->list = cl_dll_mergesort(l->list,
                               (list_of_cobjects == true) ? compare_cobjects
                                                          : l->compare_to);

    pthread_mutex_unlock(&l->lock);

    return 0;
}

static int get_indexof(const void *list, enum cl_object object, void *content,
    unsigned int size, enum cl_object node_content, bool bottom_up)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node = NULL;
    bool list_of_cobjects;
    int idx;

    __clib_function_init__(true, list, object, -1);
    list_of_cobjects = is_list_of_cobjects(l);

    if ((list_of_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(content, size, l, node_content);

    if (NULL == node)
        return -1;

    if (bottom_up == false) {
        idx = cl_dll_indexof(l->list, node,
                             (list_of_cobjects == true) ? cobjects_are_equal
                                                        : l->equals);
    } else
        idx = cl_dll_last_indexof(l->list, node,
                                  (list_of_cobjects == true) ? cobjects_are_equal
                                                             : l->equals);

    destroy_node(node, false);

    return idx;
}

int cglist_indexof(const void *list, enum cl_object object,
    void *content, unsigned int size, enum cl_object node_object)
{
    return get_indexof(list, object, content, size, node_object, false);
}

int cglist_last_indexof(const void *list, enum cl_object object,
    void *content, unsigned int size, enum cl_object node_object)
{
    return get_indexof(list, object, content, size, node_object, true);
}

bool cglist_contains(const void *list, enum cl_object object,
    void *content, unsigned int size, enum cl_object node_object)
{
    glist_s *l = (glist_s *)list;
    bool list_of_cobjects;
    struct gnode_s *node = NULL;
    bool st;

    __clib_function_init__(true, list, object, -1);
    list_of_cobjects = is_list_of_cobjects(l);

    if ((list_of_cobjects == false) && (NULL == l->equals)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    node = new_node(content, size, l, node_object);

    if (NULL == node)
        return -1;

    st = cl_dll_contains(l->list, node,
                         (list_of_cobjects == true) ? cobjects_are_equal
                                                    : l->equals);

    destroy_node(node, false);

    return st;
}

void *cglist_peek(const void *list, enum cl_object object,
    enum cl_object node_object)
{
    glist_s *l = (glist_s *)list;
    struct gnode_s *node;

    __clib_function_init__(true, list, object, NULL);
    node = cl_dll_peek(l->list);

    if (NULL == node)
        return NULL;

    return cglist_node_ref(node, node_object);
}

bool cglist_is_empty(const void *list, enum cl_object object)
{
    __clib_function_init__(true, list, object, false);

    return (NULL == list) ? true : false;
}

int cglist_set_compare_to(const void *list, enum cl_object object,
    int (*compare_to)(void *, void *))
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, -1);
    l->compare_to = compare_to;

    return 0;
}

int cglist_set_filter(const void *list, enum cl_object object,
    int (*filter)(void *, void *))
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, -1);
    l->filter = filter;

    return 0;
}

int cglist_set_equals(const void *list, enum cl_object object,
    int (*equals)(void *, void *))
{
    glist_s *l = (glist_s *)list;

    __clib_function_init__(true, list, object, -1);
    l->equals = equals;

    return 0;
}

