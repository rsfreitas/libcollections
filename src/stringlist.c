
/*
 * Description: API to handle string lists using cstring_list_t object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>

#include "collections.h"

struct cstring_list_node_s {
    clist_t     *prev;
    clist_t     *next;
    cstring_t   *s;
};

struct cstring_list_s {
    struct cstring_list_node_s  *list;
    unsigned int                size;
};

static struct cstring_list_node_s *new_list_node_s(cstring_t *v)
{
    struct cstring_list_node_s *l = NULL;

    l = calloc(1, sizeof(struct cstring_list_node_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->s = cstring_ref(v);

    return l;
}

static void destroy_list_node_s(void *ptr)
{
    struct cstring_list_node_s *l = (struct cstring_list_node_s *)ptr;

    if (NULL == ptr)
        return;

    cstring_unref(l->s);
    free(l);
}

cstring_list_t LIBEXPORT *cstring_list_new(void)
{
    struct cstring_list_s *l = NULL;

    cerrno_clear();
    l = calloc(1, sizeof(struct cstring_list_s));

    if (NULL == l) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    l->list = NULL;
    l->size = 0;

    return l;
}

int LIBEXPORT cstring_list_free(cstring_list_t *l)
{
    struct cstring_list_s *p = (struct cstring_list_s *)l;

    cerrno_clear();

    if (NULL == l) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (p->size > 0)
        cdll_free(p->list, destroy_list_node_s);

    free(l);

    return 0;
}

int LIBEXPORT cstring_list_size(const cstring_list_t *l)
{
    struct cstring_list_s *p = (struct cstring_list_s *)l;

    cerrno_clear();

    if (NULL == l) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return p->size;
}

int LIBEXPORT cstring_list_add(cstring_list_t *l, cstring_t *s)
{
    struct cstring_list_s *p = (struct cstring_list_s *)l;
    struct cstring_list_node_s *n = NULL;

    cerrno_clear();

    if ((NULL == l) || (NULL == s)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    n = new_list_node_s(s);

    if (NULL == n)
        return -1;

    p->list = cdll_unshift(p->list, n);
    p->size++;

    return 0;
}

cstring_t LIBEXPORT *cstring_list_get(const cstring_list_t *l,
    unsigned int index)
{
    struct cstring_list_s *p = (struct cstring_list_s *)l;
    struct cstring_list_node_s *n = NULL;

    cerrno_clear();

    if (NULL == l) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (index >= (unsigned int)cstring_list_size(p))
        return NULL;

    n = cdll_at(p->list, index);

    return cstring_ref(n->s);
}

