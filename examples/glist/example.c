
/*
 * Description: Example showing the use of the clist_t API.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Sep  3 17:23:49 2016
 * Project: example.c
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <collections.h>

struct node_example {
    int x;
    int y;
};

static struct node_example *new_node_example(int x, int y)
{
    struct node_example *e = NULL;

    e = calloc(1, sizeof(struct node_example));

    if (NULL == e)
        return NULL;

    e->x = x;
    e->y = y;

    return e;
}

static void destroy_node_example(void *a)
{
    struct node_example *e = (struct node_example *)a;

    if (NULL == a)
        return;

    free(e);
}

static int compare_node_example(clist_node_t *n1, clist_node_t *n2)
{
    struct node_example *e1 = (struct node_example *)clist_node_content(n1);
    struct node_example *e2 = (struct node_example *)clist_node_content(n2);

    if (e1->x > e2->x)
        return 1;
    else if (e1->x < e2->x)
        return -1;

    return 0;
}

static int filter_node_example(clist_node_t *n, void *filter_data)
{
    struct node_example *e = (struct node_example *)clist_node_content(n);
    int limit = *((int *)filter_data);

    if (e->y < limit)
        return 1;

    return 0;
}

static int equals_node_example(clist_node_t *n1, clist_node_t *n2)
{
    if (compare_node_example(n1, n2) == 0)
        return 1;

    return 0;
}

static int print_node(clist_node_t *node, void *data)
{
    struct node_example *e = (struct node_example *)clist_node_content(node);

    printf("%s: %d, %d\n", __FUNCTION__, e->x, e->y);

    return 0;
}

int main(void)
{
    clist_t *list = NULL, *lfilter = NULL;
    int i, limit = 40;
    unsigned int n;
    struct node_example e = { 42, 43 };

    collections_init();
    list = clist_create(destroy_node_example, compare_node_example,
                        filter_node_example, equals_node_example);

    for (i = 0; i < 20; i++) {
        n = crand(100);
        clist_push(list, new_node_example(n, n + 1));
    }

    printf("List size = %d\n", clist_size(list));
    clist_map(list, print_node, NULL);

    printf("Sorted list\n");
    clist_sort(list);
    clist_map(list, print_node, NULL);

    printf("Filter elements with y < %d\n", limit);
    lfilter = clist_filter(list, &limit);

    printf("Filtered list size = %d\n", clist_size(lfilter));
    clist_map(lfilter, print_node, NULL);

    printf("First list size = %d\n", clist_size(list));
    clist_map(list, print_node, NULL);

    if (clist_contains(list, &e) == true)
        printf("Index of item with x = %d, y = %d => %d\n", e.x, e.y,
               clist_indexof(list, &e));

    clist_destroy(list);

    if (lfilter != NULL)
        clist_destroy(lfilter);

    collections_uninit();

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

