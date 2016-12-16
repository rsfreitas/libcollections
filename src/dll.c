
/*
 * Description: API to handle doubly linked lists. This API is based on
 *              libsimplelist by Stig Brautaset.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
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

#include "collections.h"

struct cdll_node {
    struct cdll_node *prev;
    struct cdll_node *next;
};

/*
 * Pushes a new node onto the list.
 */
void LIBEXPORT *cdll_push(void *root, void *node)
{
    struct cdll_node *l = root, *p = node;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == root) {
        p->prev = NULL;
        p->next = NULL;

        return node;
    }

    p->prev = NULL;
    p->next = l;
    l->prev = p;

    return p;
}

/*
 * Pop a node from a list.
 */
void LIBEXPORT *cdll_pop(void *root)
{
    struct cdll_node **pp = root;
    struct cdll_node *p = *pp;

    __clib_function_init__(false, NULL, -1, NULL);

    if ((NULL == p) || (NULL == p->next)) {
        *pp = NULL;
        return p;
    }

    *pp = p->next;
    (*pp)->prev = p->prev;

    p->next = NULL;
    p->prev = NULL;

    return p;
}

/*
 * Shifts a node onto the far end of a list.
 */
void LIBEXPORT *cdll_unshift(void *root, void *node)
{
    struct cdll_node *l = root;
    struct cdll_node *p = node;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == node)
        return root;

    if (NULL == root) {
        p->prev = NULL;
        p->next = NULL;

        return node;
    }

    /* Move to the last node */
    while (l->next)
        l = l->next;

    p->prev = l;
    p->next = NULL;
    l->next = p;

    return root;
}

/*
 * Shifts a node from the far end of a list.
 */
void LIBEXPORT *cdll_shift(void *root)
{
    struct cdll_node **pp = root;
    struct cdll_node *p = *pp;
    struct cdll_node *q;

    __clib_function_init__(false, NULL, -1, NULL);

    if ((NULL == p) || (NULL == p->next)) {
        *pp = NULL;
        return p;
    }

    q = p;

    while (p->next) {
        q = p;
        p = p->next;
    }

    q->next = NULL;
    p->next = NULL;
    p->prev = NULL;

    return p;
}

/*
 * Returns the number of nodes of a list.
 */
int LIBEXPORT cdll_size(void *root)
{
    struct cdll_node *p = root;
    int c = 1;

    __clib_function_init__(false, NULL, -1, -1);

    if (NULL == root)
        return 0;

    for (; p->next != NULL; p = p->next)
        c++;

    return c++;
}

/*
 * Maps a function to every node on a list.
 */
void LIBEXPORT *cdll_map(void *root, int (*foo)(void *, void *), void *data)
{
    struct cdll_node *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    for (p = root; p != NULL; p = p->next)
        if (foo(p, data))
            return p;

    return NULL;
}

/*
 * Maps a function to every node on a list.
 */
void LIBEXPORT *cdll_map_indexed(void *root,
    int (*foo)(unsigned int, void *, void *), void *data)
{
    struct cdll_node *p = NULL;
    unsigned int i;

    __clib_function_init__(false, NULL, -1, NULL);

    for (p = root, i = 0; p != NULL; p = p->next, i++)
        if (foo(i, p, data))
            return p;

    return NULL;
}

/*
 * Maps a function to every node on a list from the end to the top.
 */
void LIBEXPORT *cdll_map_reverse(void *root, int (*foo)(void *, void *),
    void *data)
{
    struct cdll_node *l = root, *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    /* Move to the last node */
    while (l->next)
        l = l->next;

    for (p = l; p != NULL; p = p->prev)
        if (foo(p, data))
            return p;

    return NULL;
}

/*
 * Maps a function to every node on a list from the end to the top.
 */
void LIBEXPORT *cdll_map_indexed_reverse(void *root,
    int (*foo)(unsigned int, void *, void *), void *data)
{
    struct cdll_node *l = root, *p = NULL;
    unsigned int i;

    __clib_function_init__(false, NULL, -1, NULL);

    /* Move to the last node */
    while (l->next)
        l = l->next;

    for (p = l, i = 0; p != NULL; p = p->prev, i++)
        if (foo(i, p, data))
            return p;

    return NULL;
}

/*
 * Frees a list of nodes.
 */
void LIBEXPORT cdll_free(void *root, void (*foo)(void *))
{
    struct cdll_node *p;

    __clib_function_init_ex2__(false, NULL, -1);

    if (NULL == foo)
        foo = free;

    while ((p = cdll_pop(&root)))
        foo(p);
}

/*
 * Gets a pointer to a specific node inside a list.
 */
void LIBEXPORT *cdll_at(void *root, unsigned int index)
{
    struct cdll_node *p;
    unsigned int i;

    __clib_function_init__(false, NULL, -1, NULL);

    for (p = root, i = 0; p != NULL; p = p->next, i++)
        if (i == index)
            return p;

    return NULL;
}

/*
 * Moves all elements from a list to another
 */
void LIBEXPORT *cdll_move(void *root)
{
    struct cdll_node *n = NULL, *p;

    __clib_function_init__(false, NULL, -1, NULL);

    while ((p = cdll_pop(&root)))
        n = cdll_unshift(n, p);

    return n;
}

/*
 * Extracts elements from a list according a specific filter.
 */
void LIBEXPORT *cdll_filter(void *root, int (*foo)(void *, void *), void *data)
{
    struct cdll_node **pp = root;
    struct cdll_node *p = *pp;
    struct cdll_node *r = NULL, *head = NULL, *q = NULL;
    int v;

    __clib_function_init__(false, NULL, -1, NULL);

    while (p) {
        q = cdll_pop(&p);
        v = foo(q, data);

        if (v == 0)
            head = cdll_unshift(head, q);
        else {
            r = cdll_unshift(r, q);

            if (v < 0)
                break;
        }
    }

    *pp = cdll_unshift(head, p);

    return r;
}

/*
 * Deletes elements from a list according a specific filter function.
 */
void LIBEXPORT *cdll_delete(void *root, int (*filter)(void *, void *),
    void *data, void (*foo)(void *))
{
    struct cdll_node *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cdll_filter(root, filter, data);

    if ((p != NULL) && (foo != NULL)) {
        cdll_free(p, foo);
        p = NULL;
    }

    return p;
}

/*
 * Delete an element from a list at a specific position.
 */
void LIBEXPORT *cdll_delete_indexed(void *root, unsigned int index,
    void (*foo)(void *))
{
    struct cdll_node **pp = root;
    struct cdll_node *p = *pp;
    struct cdll_node *q = NULL, *r = NULL;
    int size = 0;
    unsigned int i;
    bool first = false, last = false;

    __clib_function_init__(false, NULL, -1, NULL);
    size = cdll_size(p);

    /* Takes a few decisions first */
    if (index >= (unsigned int)size)
        return NULL;
    else if (index == 0)
        first = true;
    else if (index == (unsigned int)(size - 1))
        last = true;

    if (first == true) {
        r = p;
        p = p->next;
        p->prev = NULL;

        goto end_block;
    }

    for (q = p, i = 0; q; q = q->next, i++) {
        if (i == index) {
            r = q;

            if (last == true)
                q->prev->next = NULL;
            else {
                q->prev->next = q->next;
                q->next->prev = q->prev;
            }

            q = q->next;
            break;
        }
    }

end_block:
    if (r != NULL) {
        *pp = p;
        r->next = NULL;
        r->prev = NULL;

        if (foo != NULL) {
            foo(r);
            r = NULL;
        }
    }

    return r;
}

static void *cdll_split(void *root)
{
    struct cdll_node *f, *s, *tmp;

    f = s = root;

    while (f->next && f->next->next) {
        f = f->next->next;
        s = s->next;
    }

    tmp = s->next;
    s->next = NULL;

    return tmp;
}

static void *cdll_merge(void *p1, void *p2, int (*cmp)(void *, void*))
{
    struct cdll_node *f = p1, *s = p2, *q;
    int ret;

    if (NULL == p1)
        return p2;

    if (NULL == p2)
        return p1;

    /* The cmp function must return < 0 if @p1 is less than @p2 */
    ret = cmp(p1, p2);

    if (ret < 0) {
        f->next = cdll_merge(f->next, s, cmp);
        f->next->prev = f;
        f->prev = NULL;
        q = f;
    } else {
        s->next = cdll_merge(f, s->next, cmp);
        s->next->prev = s;
        s->prev = NULL;
        q = s;
    }

    return q;
}

/*
 * Sort all elements from a list using mergesort algorithm.
 */
void LIBEXPORT *cdll_mergesort(void *root, int (*cmp)(void *, void *))
{
    struct cdll_node *p = root;

    __clib_function_init__(false, NULL, -1, NULL);

    if (p && p->next) {
        p = cdll_split(root);
        root = cdll_merge(cdll_mergesort(root, cmp), cdll_mergesort(p, cmp),
                          cmp);
    }

    return root;
}

bool LIBEXPORT cdll_contains(void *root, void *p,
    int (*foo)(void *, void *))
{
    struct cdll_node *q = NULL;

    __clib_function_init__(false, NULL, -1, false);
    q = cdll_map(root, foo, p);

    if (NULL == q)
        return false;

    return true;
}

int LIBEXPORT cdll_indexof(void *root, void *n, int (*foo)(void *, void *))
{
    struct cdll_node *p = NULL;
    int i;

    __clib_function_init__(false, NULL, -1, -1);

    for (p = root, i = 0; p != NULL; p = p->next, i++)
        if (foo(p, n))
            return i;

    return -1;
}

int LIBEXPORT cdll_last_indexof(void *root, void *n,
    int (*foo)(void *, void *))
{
    struct cdll_node *l = root, *p = NULL;
    int i, size = 0;

    __clib_function_init__(false, NULL, -1, -1);

    /* Move to the last node */
    while (l->next) {
        l = l->next;
        size++;
    }

    for (p = l, i = size; p != NULL; p = p->prev, i--)
        if (foo(p, n))
            return i;

    return -1;
}

void LIBEXPORT *cdll_peek(void *root)
{
    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == root)
        return NULL;

    return root;
}

