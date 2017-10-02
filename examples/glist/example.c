
/*
 * Description: Example showing the use of the cl_list_t API.
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

#define LIST_SIZE           9

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

static int compare_node_example(cl_list_node_t *n1, cl_list_node_t *n2)
{
    struct node_example *e1 = (struct node_example *)cl_list_node_content(n1);
    struct node_example *e2 = (struct node_example *)cl_list_node_content(n2);

    if (e1->x > e2->x)
        return 1;
    else if (e1->x < e2->x)
        return -1;

    return 0;
}

static int filter_node_example(cl_list_node_t *n, void *filter_data)
{
    struct node_example *e = (struct node_example *)cl_list_node_content(n);
    int limit = *((int *)filter_data);

    if (e->y < limit)
        return 1;

    return 0;
}

static int equals_node_example(cl_list_node_t *n1, cl_list_node_t *n2)
{
    if (compare_node_example(n1, n2) == 0)
        return 1;

    return 0;
}

static int print_node(cl_list_node_t *node, void *data)
{
    struct node_example *e = (struct node_example *)cl_list_node_content(node);

    printf("%s: %d, %d\n", __FUNCTION__, e->x, e->y);

    return 0;
}

int main(void)
{
    cl_list_t *list = NULL, *lfilter = NULL;
    cl_list_node_t *node = NULL;
    int i, limit = 40;
    unsigned int n, k = 11;
    struct node_example e = { 42, 43 };

    cl_init(NULL);
    list = cl_list_create(destroy_node_example, compare_node_example,
                          filter_node_example, equals_node_example);

    for (i = 0; i < LIST_SIZE; i++) {
        n = cl_rand(100);
        cl_list_push(list, new_node_example(n, n + 1), -1);
    }

    printf("List size = %d\n", cl_list_size(list));
    cl_list_map(list, print_node, NULL);

    node = cl_list_middle(list);

    if (node != NULL) {
        printf("Middle element: ");
        print_node(node, NULL);
    } else
        printf("Middle not found\n");

    printf("Rotate list by: %d\n", k);
    cl_list_rotate(list, k);
    cl_list_map(list, print_node, NULL);

    printf("Sorted list\n");
    cl_list_sort(list);
    cl_list_map(list, print_node, NULL);

    printf("Filter elements with y < %d\n", limit);
    lfilter = cl_list_filter(list, &limit);

    printf("Filtered list size = %d\n", cl_list_size(lfilter));
    cl_list_map(lfilter, print_node, NULL);

    printf("First list size = %d\n", cl_list_size(list));
    cl_list_map(list, print_node, NULL);

    if (cl_list_contains(list, &e, -1) == true)
        printf("Index of item with x = %d, y = %d => %d\n", e.x, e.y,
               cl_list_indexof(list, &e, -1));

    cl_list_destroy(list);

    if (lfilter != NULL)
        cl_list_destroy(lfilter);

    cl_uninit();

    /* This makes valgrind report no memory leaks. */
    cl_exit();

    return 0;
}

