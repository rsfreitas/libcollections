
/*
 * Description: Circular queue API.
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

#define cl_cqueue_members                      \
    cl_struct_member(unsigned int, max_size)        \
    cl_struct_member(unsigned int, size)            \
    cl_struct_member(cl_queue_t *, queue)           \
    cl_struct_member(pthread_mutex_t, lock)         \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_cqueue_s, cl_cqueue_members);

#define cl_cqueue_s            cl_struct(cl_cqueue_s)

static void destroy_circular_queue_s(const struct cl_ref_s *ref)
{
    cl_cqueue_s *q = cl_container_of(ref, cl_cqueue_s, ref);

    if (NULL == q)
        return;

    if (q->queue != NULL)
        cl_queue_destroy(q->queue);

    free(q);
    q = NULL;
}

static cl_cqueue_s *new_circular_queue_s(unsigned int max_size)
{
    cl_cqueue_s *q = NULL;

    q = calloc(1, sizeof(cl_cqueue_s));

    if (NULL == q) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    q->max_size = max_size;
    pthread_mutex_init(&q->lock, NULL);
    set_typeof(CL_OBJ_CIRCULAR_QUEUE, q);

    /* Reference count */
    q->ref.count = 1;
    q->ref.free = destroy_circular_queue_s;

    return q;
}

/*
 *
 * API
 *
 */

__PUB_API__ cl_cqueue_t *cl_cqueue_ref(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = (cl_cqueue_s *)cqueue;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    cl_ref_inc(&q->ref);

    return cqueue;
}

__PUB_API__ int cl_cqueue_unref(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = (cl_cqueue_s *)cqueue;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    cl_ref_dec(&q->ref);

    return 0;
}

__PUB_API__ cl_cqueue_t *cl_cqueue_create(unsigned int size,
    void (*unref_node)(void *),
    int (*compare_to)(cl_queue_node_t *, cl_queue_node_t *),
    int (*filter)(cl_queue_node_t *, void *),
    int (*equals)(cl_queue_node_t *, cl_queue_node_t *))
{
    cl_cqueue_s *q = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (size == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    q = new_circular_queue_s(size);

    if (NULL == q)
        return NULL;

    q->queue = cl_queue_create(unref_node, compare_to, filter, equals);

    if (NULL == q->queue) {
        cl_cqueue_unref(q);
        /* Sets error code here */
        return NULL;
    }

    return q;
}

__PUB_API__ int cl_cqueue_destroy(cl_cqueue_t *cqueue)
{
    return cl_cqueue_unref(cqueue);
}

__PUB_API__ int cl_cqueue_size(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int size;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    size = q->size;
    cl_cqueue_unref(q);

    return size;
}

__PUB_API__ int cl_cqueue_enqueue(cl_cqueue_t *cqueue,
    const void *data, unsigned int data_size)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;
    cl_queue_node_t *node;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    pthread_mutex_lock(&q->lock);

    if (q->size >= q->max_size) {
        node = cl_queue_dequeue(q->queue);
        cl_queue_node_unref(node);
        q->size--;
    }

    ret = cl_queue_enqueue(q->queue, data, data_size);
    q->size++;
    pthread_mutex_unlock(&q->lock);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_dequeue(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    pthread_mutex_lock(&q->lock);

    if (q->size == 0)
        goto end_block;

    node = cl_queue_dequeue(q->queue);
    q->size--;

end_block:
    pthread_mutex_unlock(&q->lock);
    cl_cqueue_unref(q);

    return node;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_map(cl_cqueue_t *cqueue,
    int (*foo)(cl_queue_node_t *, void *), void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_queue_map(q->queue, foo, data);

end_block:
    cl_cqueue_unref(q);
    return node;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_map_indexed(cl_cqueue_t *cqueue,
    int (*foo)(unsigned int, cl_queue_node_t *, void *), void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_queue_map_indexed(q->queue, foo, data);

end_block:
    cl_cqueue_unref(q);
    return node;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_map_reverse(cl_cqueue_t *cqueue,
    int (*foo)(cl_queue_node_t *, void *), void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_queue_map_reverse(q->queue, foo, data);

end_block:
    cl_cqueue_unref(q);
    return node;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_map_reverse_indexed(cl_cqueue_t *cqueue,
    int (*foo)(unsigned int, cl_queue_node_t *, void *), void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cl_queue_map_reverse_indexed(q->queue, foo, data);

end_block:
    cl_cqueue_unref(q);
    return node;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_at(cl_cqueue_t *cqueue,
    unsigned int index)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    node = cl_queue_at(q->queue, index);
    cl_cqueue_unref(q);

    return node;
}

__PUB_API__ int cl_cqueue_delete(cl_cqueue_t *cqueue, void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_delete(q->queue, data);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ int cl_cqueue_delete_indexed(cl_cqueue_t *cqueue,
    unsigned int index)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_delete_indexed(q->queue, index);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ cl_cqueue_t *cl_cqueue_move(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_cqueue_s *n = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    n = new_circular_queue_s(q->size);

    if (NULL == n) {
        cl_cqueue_unref(q);
        return NULL;
    }

    n->queue = cl_queue_move(q->queue);
    cl_cqueue_unref(q);

    if (NULL == n->queue) {
        cl_cqueue_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ cl_cqueue_t *cl_cqueue_filter(cl_cqueue_t *cqueue,
    void *data)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_cqueue_s *n = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    n = new_circular_queue_s(q->size);

    if (NULL == n) {
        cl_cqueue_unref(q);
        return NULL;
    }

    n->queue = cl_queue_filter(q->queue, data);
    cl_cqueue_unref(q);

    if (NULL == n->queue) {
        cl_cqueue_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ int cl_cqueue_sort(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_sort(q->queue);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ int cl_cqueue_indexof(cl_cqueue_t *cqueue, void *element,
    unsigned int size)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_indexof(q->queue, element, size);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ int cl_cqueue_last_indexof(cl_cqueue_t *cqueue,
    void *element, unsigned int size)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_last_indexof(q->queue, element, size);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ bool cl_cqueue_contains(cl_cqueue_t *cqueue,
    void *element, unsigned int size)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    bool ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, false);
    ret = cl_queue_contains(q->queue, element, size);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ cl_queue_node_t *cl_cqueue_front(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    cl_queue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, NULL);
    node = cl_queue_front(q->queue);
    cl_cqueue_unref(q);

    return node;
}

__PUB_API__ bool cl_cqueue_is_empty(cl_cqueue_t *cqueue)
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int size;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, false);
    size = q->size;
    cl_cqueue_unref(q);

    return (size > 0) ? true : false;
}

__PUB_API__ int cl_cqueue_set_compare_to(cl_cqueue_t *cqueue,
    int (*compare_to)(cl_queue_node_t *, cl_queue_node_t *))
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_set_compare_to(q->queue, compare_to);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ int cl_cqueue_set_filter(cl_cqueue_t *cqueue,
    int (*filter)(cl_queue_node_t *, void *))
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_set_filter(q->queue, filter);
    cl_cqueue_unref(q);

    return ret;
}

__PUB_API__ int cl_cqueue_set_equals(cl_cqueue_t *cqueue,
    int (*equals)(cl_queue_node_t *, cl_queue_node_t *))
{
    cl_cqueue_s *q = cl_cqueue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CL_OBJ_CIRCULAR_QUEUE, -1);
    ret = cl_queue_set_equals(q->queue, equals);
    cl_cqueue_unref(q);

    return ret;
}

