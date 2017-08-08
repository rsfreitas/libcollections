
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

#define cl_cstack_members                           \
    cl_struct_member(unsigned int, max_size)        \
    cl_struct_member(unsigned int, size)            \
    cl_struct_member(cl_stack_t *, stack)           \
    cl_struct_member(pthread_mutex_t, lock)         \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_cstack_s, cl_cstack_members);

#define cl_cstack_s            cl_struct(cl_cstack_s)

static void destroy_circular_stack_s(const struct cl_ref_s *ref)
{
    cl_cstack_s *q = cl_container_of(ref, cl_cstack_s, ref);

    if (NULL == q)
        return;

    if (q->stack != NULL)
        cl_stack_destroy(q->stack);

    free(q);
    q = NULL;
}

static cl_cstack_s *new_circular_stack_s(unsigned int max_size)
{
    cl_cstack_s *q = NULL;

    q = calloc(1, sizeof(cl_cstack_s));

    if (NULL == q) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    q->max_size = max_size;
    pthread_mutex_init(&q->lock, NULL);
    typeof_set(CL_OBJ_CIRCULAR_STACK, q);

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

__PUB_API__ cl_cstack_t *cl_cstack_ref(cl_cstack_t *cstack)
{
    cl_cstack_s *q = (cl_cstack_s *)cstack;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    cl_ref_inc(&q->ref);

    return cstack;
}

__PUB_API__ int cl_cstack_unref(cl_cstack_t *cstack)
{
    cl_cstack_s *q = (cl_cstack_s *)cstack;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    cl_ref_dec(&q->ref);

    return 0;
}

__PUB_API__ cl_cstack_t *cl_cstack_create(unsigned int size,
    void (*unref_node)(void *),
    int (*compare_to)(cl_stack_node_t *, cl_stack_node_t *),
    int (*filter)(cl_stack_node_t *, void *),
    int (*equals)(cl_stack_node_t *, cl_stack_node_t *))
{
    cl_cstack_s *q = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (size == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    q = new_circular_stack_s(size);

    if (NULL == q)
        return NULL;

    q->stack = cl_stack_create(unref_node, compare_to, filter, equals);

    if (NULL == q->stack) {
        cl_cstack_unref(q);
        /* Sets error code here */
        return NULL;
    }

    return q;
}

__PUB_API__ int cl_cstack_destroy(cl_cstack_t *cstack)
{
    return cl_cstack_unref(cstack);
}

__PUB_API__ int cl_cstack_size(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int size;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    size = q->size;
    cl_cstack_unref(q);

    return size;
}

__PUB_API__ int cl_cstack_push(cl_cstack_t *cstack,
    const void *data, unsigned int data_size)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;
    cl_stack_node_t *node;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    pthread_mutex_lock(&q->lock);

    if (q->size >= q->max_size) {
        node = cglist_shift(q->stack, CL_OBJ_STACK);
        cl_stack_node_unref(node);
        q->size--;
    }

    ret = cl_stack_push(q->stack, data, data_size);
    q->size++;
    pthread_mutex_unlock(&q->lock);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ cl_stack_node_t *cl_cstack_pop(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    pthread_mutex_lock(&q->lock);

    if (q->size == 0)
        goto end_block;

    node = cl_stack_pop(q->stack);
    q->size--;

end_block:
    pthread_mutex_unlock(&q->lock);
    cl_cstack_unref(q);

    return node;
}

__PUB_API__ cl_stack_node_t *cl_cstack_map(cl_cstack_t *cstack,
    int (*foo)(cl_stack_node_t *, void *), void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_stack_map(q->stack, foo, data);

end_block:
    cl_cstack_unref(q);
    return node;
}

__PUB_API__ cl_stack_node_t *cl_cstack_map_indexed(cl_cstack_t *cstack,
    int (*foo)(unsigned int, cl_stack_node_t *, void *), void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_stack_map_indexed(q->stack, foo, data);

end_block:
    cl_cstack_unref(q);
    return node;
}

__PUB_API__ cl_stack_node_t *cl_cstack_map_reverse(cl_cstack_t *cstack,
    int (*foo)(cl_stack_node_t *, void *), void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_stack_map_reverse(q->stack, foo, data);

end_block:
    cl_cstack_unref(q);
    return node;
}

__PUB_API__ cl_stack_node_t *cl_cstack_map_reverse_indexed(cl_cstack_t *cstack,
    int (*foo)(unsigned int, cl_stack_node_t *, void *), void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_stack_map_reverse_indexed(q->stack, foo, data);

end_block:
    cl_cstack_unref(q);
    return node;
}

__PUB_API__ cl_stack_node_t *cl_cstack_at(cl_cstack_t *cstack,
    unsigned int index)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    node = cl_stack_at(q->stack, index);
    cl_cstack_unref(q);

    return node;
}

__PUB_API__ int cl_cstack_delete(cl_cstack_t *cstack, void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_delete(q->stack, data);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ int cl_cstack_delete_indexed(cl_cstack_t *cstack,
    unsigned int index)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_delete_indexed(q->stack, index);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ cl_cstack_t *cl_cstack_move(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_cstack_s *n = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    n = new_circular_stack_s(q->size);

    if (NULL == n) {
        cl_cstack_unref(q);
        return NULL;
    }

    n->stack = cl_stack_move(q->stack);
    cl_cstack_unref(q);

    if (NULL == n->stack) {
        cl_cstack_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ cl_cstack_t *cl_cstack_filter(cl_cstack_t *cstack,
    void *data)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_cstack_s *n = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    n = new_circular_stack_s(q->size);

    if (NULL == n) {
        cl_cstack_unref(q);
        return NULL;
    }

    n->stack = cl_stack_filter(q->stack, data);
    cl_cstack_unref(q);

    if (NULL == n->stack) {
        cl_cstack_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ int cl_cstack_sort(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_sort(q->stack);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ int cl_cstack_indexof(cl_cstack_t *cstack, void *element,
    unsigned int size)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_indexof(q->stack, element, size);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ int cl_cstack_last_indexof(cl_cstack_t *cstack,
    void *element, unsigned int size)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_last_indexof(q->stack, element, size);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ bool cl_cstack_contains(cl_cstack_t *cstack,
    void *element, unsigned int size)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    bool ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, false);
    ret = cl_stack_contains(q->stack, element, size);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ cl_stack_node_t *cl_cstack_peek(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    cl_stack_node_t *node = NULL;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, NULL);
    node = cl_stack_peek(q->stack);
    cl_cstack_unref(q);

    return node;
}

__PUB_API__ bool cl_cstack_is_empty(cl_cstack_t *cstack)
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int size;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, false);
    size = q->size;
    cl_cstack_unref(q);

    return (size > 0) ? true : false;
}

__PUB_API__ int cl_cstack_set_compare_to(cl_cstack_t *cstack,
    int (*compare_to)(cl_stack_node_t *, cl_stack_node_t *))
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_set_compare_to(q->stack, compare_to);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ int cl_cstack_set_filter(cl_cstack_t *cstack,
    int (*filter)(cl_stack_node_t *, void *))
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_set_filter(q->stack, filter);
    cl_cstack_unref(q);

    return ret;
}

__PUB_API__ int cl_cstack_set_equals(cl_cstack_t *cstack,
    int (*equals)(cl_stack_node_t *, cl_stack_node_t *))
{
    cl_cstack_s *q = cl_cstack_ref(cstack);
    int ret = -1;

    __clib_function_init__(true, cstack, CL_OBJ_CIRCULAR_STACK, -1);
    ret = cl_stack_set_equals(q->stack, equals);
    cl_cstack_unref(q);

    return ret;
}

