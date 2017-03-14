
/*
 * Description: Circular stack API.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Feb 20 14:55:48 2017
 * Project: libcollections
 *
 * Copyright (C) 2017 Rodrigo Freitas
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

#define circular_stack_members                      \
    cl_struct_member(unsigned int, max_size)        \
    cl_struct_member(unsigned int, size)            \
    cl_struct_member(cstack_t *, stack)             \
    cl_struct_member(pthread_mutex_t, lock)         \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(circular_stack_s, circular_stack_members);

#define circular_stack_s            cl_struct(circular_stack_s)

static void destroy_circular_stack_s(const struct cref_s *ref)
{
    circular_stack_s *q = cl_container_of(ref, circular_stack_s, ref);

    if (NULL == q)
        return;

    if (q->stack != NULL)
        cstack_destroy(q->stack);

    free(q);
    q = NULL;
}

static circular_stack_s *new_circular_stack_s(unsigned int max_size)
{
    circular_stack_s *q = NULL;

    q = calloc(1, sizeof(circular_stack_s));

    if (NULL == q) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    q->max_size = max_size;
    pthread_mutex_init(&q->lock, NULL);
    set_typeof(CIRCULAR_STACK, q);

    /* Reference count */
    q->ref.count = 1;
    q->ref.free = destroy_circular_stack_s;

    return q;
}

/*
 *
 * API
 *
 */

__PUB_API__ circular_stack_t *circular_stack_ref(circular_stack_t *cstack)
{
    circular_stack_s *q = (circular_stack_s *)cstack;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    cref_inc(&q->ref);

    return cstack;
}

__PUB_API__ int circular_stack_unref(circular_stack_t *cstack)
{
    circular_stack_s *q = (circular_stack_s *)cstack;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    cref_dec(&q->ref);

    return 0;
}

__PUB_API__ circular_stack_t *circular_stack_create(unsigned int size,
    void (*unref_node)(void *),
    int (*compare_to)(cstack_node_t *, cstack_node_t *),
    int (*filter)(cstack_node_t *, void *),
    int (*equals)(cstack_node_t *, cstack_node_t *))
{
    circular_stack_s *q = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (size == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    q = new_circular_stack_s(size);

    if (NULL == q)
        return NULL;

    q->stack = cstack_create(unref_node, compare_to, filter, equals);

    if (NULL == q->stack) {
        circular_stack_unref(q);
        /* Sets error code here */
        return NULL;
    }

    return q;
}

__PUB_API__ int circular_stack_destroy(circular_stack_t *cstack)
{
    return circular_stack_unref(cstack);
}

__PUB_API__ int circular_stack_size(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int size;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    size = q->size;
    circular_stack_unref(q);

    return size;
}

__PUB_API__ int circular_stack_push(circular_stack_t *cstack,
    const void *data, unsigned int data_size)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;
    cstack_node_t *node;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    pthread_mutex_lock(&q->lock);

    if (q->size >= q->max_size) {
        node = cglist_shift(q->stack, CSTACK);
        cstack_node_unref(node);
        q->size--;
    }

    ret = cstack_push(q->stack, data, data_size);
    q->size++;
    pthread_mutex_unlock(&q->lock);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ cstack_node_t *circular_stack_pop(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    pthread_mutex_lock(&q->lock);

    if (q->size == 0)
        goto end_block;

    node = cstack_pop(q->stack);
    q->size--;

end_block:
    pthread_mutex_unlock(&q->lock);
    circular_stack_unref(q);

    return node;
}

__PUB_API__ cstack_node_t *circular_stack_map(circular_stack_t *cstack,
    int (*foo)(cstack_node_t *, void *), void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cstack_map(q->stack, foo, data);

end_block:
    circular_stack_unref(q);
    return node;
}

__PUB_API__ cstack_node_t *circular_stack_map_indexed(circular_stack_t *cstack,
    int (*foo)(unsigned int, cstack_node_t *, void *), void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cstack_map_indexed(q->stack, foo, data);

end_block:
    circular_stack_unref(q);
    return node;
}

__PUB_API__ cstack_node_t *circular_stack_map_reverse(circular_stack_t *cstack,
    int (*foo)(cstack_node_t *, void *), void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cstack_map_reverse(q->stack, foo, data);

end_block:
    circular_stack_unref(q);
    return node;
}

__PUB_API__ cstack_node_t *circular_stack_map_reverse_indexed(circular_stack_t *cstack,
    int (*foo)(unsigned int, cstack_node_t *, void *), void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cstack_map_reverse_indexed(q->stack, foo, data);

end_block:
    circular_stack_unref(q);
    return node;
}

__PUB_API__ cstack_node_t *circular_stack_at(circular_stack_t *cstack,
    unsigned int index)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    node = cstack_at(q->stack, index);
    circular_stack_unref(q);

    return node;
}

__PUB_API__ int circular_stack_delete(circular_stack_t *cstack, void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_delete(q->stack, data);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ int circular_stack_delete_indexed(circular_stack_t *cstack,
    unsigned int index)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_delete_indexed(q->stack, index);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ circular_stack_t *circular_stack_move(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    circular_stack_s *n = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    n = new_circular_stack_s(q->size);

    if (NULL == n) {
        circular_stack_unref(q);
        return NULL;
    }

    n->stack = cstack_move(q->stack);
    circular_stack_unref(q);

    if (NULL == n->stack) {
        circular_stack_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ circular_stack_t *circular_stack_filter(circular_stack_t *cstack,
    void *data)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    circular_stack_s *n = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    n = new_circular_stack_s(q->size);

    if (NULL == n) {
        circular_stack_unref(q);
        return NULL;
    }

    n->stack = cstack_filter(q->stack, data);
    circular_stack_unref(q);

    if (NULL == n->stack) {
        circular_stack_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ int circular_stack_sort(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_sort(q->stack);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ int circular_stack_indexof(circular_stack_t *cstack, void *element,
    unsigned int size)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_indexof(q->stack, element, size);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ int circular_stack_last_indexof(circular_stack_t *cstack,
    void *element, unsigned int size)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_last_indexof(q->stack, element, size);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ bool circular_stack_contains(circular_stack_t *cstack,
    void *element, unsigned int size)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    bool ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, false);
    ret = cstack_contains(q->stack, element, size);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ cstack_node_t *circular_stack_peek(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    cstack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, NULL);
    node = cstack_peek(q->stack);
    circular_stack_unref(q);

    return node;
}

__PUB_API__ bool circular_stack_is_empty(circular_stack_t *cstack)
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int size;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, false);
    size = q->size;
    circular_stack_unref(q);

    return (size > 0) ? true : false;
}

__PUB_API__ int circular_stack_set_compare_to(circular_stack_t *cstack,
    int (*compare_to)(cstack_node_t *, cstack_node_t *))
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_set_compare_to(q->stack, compare_to);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ int circular_stack_set_filter(circular_stack_t *cstack,
    int (*filter)(cstack_node_t *, void *))
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_set_filter(q->stack, filter);
    circular_stack_unref(q);

    return ret;
}

__PUB_API__ int circular_stack_set_equals(circular_stack_t *cstack,
    int (*equals)(cstack_node_t *, cstack_node_t *))
{
    circular_stack_s *q = circular_stack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CIRCULAR_STACK, -1);
    ret = cstack_set_equals(q->stack, equals);
    circular_stack_unref(q);

    return ret;
}

