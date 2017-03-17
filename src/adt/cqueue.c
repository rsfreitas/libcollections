
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

#define circular_queue_members                      \
    cl_struct_member(unsigned int, max_size)        \
    cl_struct_member(unsigned int, size)            \
    cl_struct_member(cqueue_t *, queue)             \
    cl_struct_member(pthread_mutex_t, lock)         \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(circular_queue_s, circular_queue_members);

#define circular_queue_s            cl_struct(circular_queue_s)

static void destroy_circular_queue_s(const struct cref_s *ref)
{
    circular_queue_s *q = cl_container_of(ref, circular_queue_s, ref);

    if (NULL == q)
        return;

    if (q->queue != NULL)
        cqueue_destroy(q->queue);

    free(q);
    q = NULL;
}

static circular_queue_s *new_circular_queue_s(unsigned int max_size)
{
    circular_queue_s *q = NULL;

    q = calloc(1, sizeof(circular_queue_s));

    if (NULL == q) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    q->max_size = max_size;
    pthread_mutex_init(&q->lock, NULL);
    set_typeof(CIRCULAR_QUEUE, q);

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

__PUB_API__ circular_queue_t *circular_queue_ref(circular_queue_t *cqueue)
{
    circular_queue_s *q = (circular_queue_s *)cqueue;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    cref_inc(&q->ref);

    return cqueue;
}

__PUB_API__ int circular_queue_unref(circular_queue_t *cqueue)
{
    circular_queue_s *q = (circular_queue_s *)cqueue;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    cref_dec(&q->ref);

    return 0;
}

__PUB_API__ circular_queue_t *circular_queue_create(unsigned int size,
    void (*unref_node)(void *),
    int (*compare_to)(cqueue_node_t *, cqueue_node_t *),
    int (*filter)(cqueue_node_t *, void *),
    int (*equals)(cqueue_node_t *, cqueue_node_t *))
{
    circular_queue_s *q = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (size == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    q = new_circular_queue_s(size);

    if (NULL == q)
        return NULL;

    q->queue = cqueue_create(unref_node, compare_to, filter, equals);

    if (NULL == q->queue) {
        circular_queue_unref(q);
        /* Sets error code here */
        return NULL;
    }

    return q;
}

__PUB_API__ int circular_queue_destroy(circular_queue_t *cqueue)
{
    return circular_queue_unref(cqueue);
}

__PUB_API__ int circular_queue_size(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int size;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    size = q->size;
    circular_queue_unref(q);

    return size;
}

__PUB_API__ int circular_queue_enqueue(circular_queue_t *cqueue,
    const void *data, unsigned int data_size)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;
    cqueue_node_t *node;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    pthread_mutex_lock(&q->lock);

    if (q->size >= q->max_size) {
        node = cqueue_dequeue(q->queue);
        cqueue_node_unref(node);
        q->size--;
    }

    ret = cqueue_enqueue(q->queue, data, data_size);
    q->size++;
    pthread_mutex_unlock(&q->lock);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ cqueue_node_t *circular_queue_dequeue(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    pthread_mutex_lock(&q->lock);

    if (q->size == 0)
        goto end_block;

    node = cqueue_dequeue(q->queue);
    q->size--;

end_block:
    pthread_mutex_unlock(&q->lock);
    circular_queue_unref(q);

    return node;
}

__PUB_API__ cqueue_node_t *circular_queue_map(circular_queue_t *cqueue,
    int (*foo)(cqueue_node_t *, void *), void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cqueue_map(q->queue, foo, data);

end_block:
    circular_queue_unref(q);
    return node;
}

__PUB_API__ cqueue_node_t *circular_queue_map_indexed(circular_queue_t *cqueue,
    int (*foo)(unsigned int, cqueue_node_t *, void *), void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cqueue_map_indexed(q->queue, foo, data);

end_block:
    circular_queue_unref(q);
    return node;
}

__PUB_API__ cqueue_node_t *circular_queue_map_reverse(circular_queue_t *cqueue,
    int (*foo)(cqueue_node_t *, void *), void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cqueue_map_reverse(q->queue, foo, data);

end_block:
    circular_queue_unref(q);
    return node;
}

__PUB_API__ cqueue_node_t *circular_queue_map_reverse_indexed(circular_queue_t *cqueue,
    int (*foo)(unsigned int, cqueue_node_t *, void *), void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);

    if (NULL == foo) {
        cset_errno(CL_NULL_ARG);
        goto end_block;
    }

    node = cqueue_map_reverse_indexed(q->queue, foo, data);

end_block:
    circular_queue_unref(q);
    return node;
}

__PUB_API__ cqueue_node_t *circular_queue_at(circular_queue_t *cqueue,
    unsigned int index)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    node = cqueue_at(q->queue, index);
    circular_queue_unref(q);

    return node;
}

__PUB_API__ int circular_queue_delete(circular_queue_t *cqueue, void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_delete(q->queue, data);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ int circular_queue_delete_indexed(circular_queue_t *cqueue,
    unsigned int index)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_delete_indexed(q->queue, index);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ circular_queue_t *circular_queue_move(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    circular_queue_s *n = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    n = new_circular_queue_s(q->size);

    if (NULL == n) {
        circular_queue_unref(q);
        return NULL;
    }

    n->queue = cqueue_move(q->queue);
    circular_queue_unref(q);

    if (NULL == n->queue) {
        circular_queue_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ circular_queue_t *circular_queue_filter(circular_queue_t *cqueue,
    void *data)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    circular_queue_s *n = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    n = new_circular_queue_s(q->size);

    if (NULL == n) {
        circular_queue_unref(q);
        return NULL;
    }

    n->queue = cqueue_filter(q->queue, data);
    circular_queue_unref(q);

    if (NULL == n->queue) {
        circular_queue_unref(n);
        n = NULL;
    }

    return n;
}

__PUB_API__ int circular_queue_sort(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_sort(q->queue);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ int circular_queue_indexof(circular_queue_t *cqueue, void *element,
    unsigned int size)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_indexof(q->queue, element, size);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ int circular_queue_last_indexof(circular_queue_t *cqueue,
    void *element, unsigned int size)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_last_indexof(q->queue, element, size);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ bool circular_queue_contains(circular_queue_t *cqueue,
    void *element, unsigned int size)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    bool ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, false);
    ret = cqueue_contains(q->queue, element, size);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ cqueue_node_t *circular_queue_front(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    cqueue_node_t *node = NULL;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, NULL);
    node = cqueue_front(q->queue);
    circular_queue_unref(q);

    return node;
}

__PUB_API__ bool circular_queue_is_empty(circular_queue_t *cqueue)
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int size;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, false);
    size = q->size;
    circular_queue_unref(q);

    return (size > 0) ? true : false;
}

__PUB_API__ int circular_queue_set_compare_to(circular_queue_t *cqueue,
    int (*compare_to)(cqueue_node_t *, cqueue_node_t *))
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_set_compare_to(q->queue, compare_to);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ int circular_queue_set_filter(circular_queue_t *cqueue,
    int (*filter)(cqueue_node_t *, void *))
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_set_filter(q->queue, filter);
    circular_queue_unref(q);

    return ret;
}

__PUB_API__ int circular_queue_set_equals(circular_queue_t *cqueue,
    int (*equals)(cqueue_node_t *, cqueue_node_t *))
{
    circular_queue_s *q = circular_queue_ref(cqueue);
    int ret = -1;

    __clib_function_init__(true, cqueue, CIRCULAR_QUEUE, -1);
    ret = cqueue_set_equals(q->queue, equals);
    circular_queue_unref(q);

    return ret;
}

