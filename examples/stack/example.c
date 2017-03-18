
/*
 * Description: Example showing the use of the stack API.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Dec  7 13:47:18 BRST 2016
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
    int value;
};

static struct node_example *new_node_example(int value)
{
    struct node_example *e = NULL;

    e = calloc(1, sizeof(struct node_example));

    if (NULL == e)
        return NULL;

    e->value = value;

    return e;
}

static void destroy_node_example(void *a)
{
    struct node_example *e = (struct node_example *)a;

    if (NULL == a)
        return;

    free(e);
}

static int compare_node_example(cl_stack_node_t *n1, cl_stack_node_t *n2)
{
    struct node_example *e1 = (struct node_example *)cl_stack_node_content(n1);
    struct node_example *e2 = (struct node_example *)cl_stack_node_content(n2);

    if (e1->value > e2->value)
        return 1;
    else if (e1->value < e2->value)
        return -1;

    return 0;
}

static int equals_node_example(cl_stack_node_t *n1, cl_stack_node_t *n2)
{
    if (compare_node_example(n1, n2) == 0)
        return 1;

    return 0;
}

static int print_node(cl_stack_node_t *node, void *data)
{
    struct node_example *e = (struct node_example *)cl_stack_node_content(node);

    printf("%s: %d\n", __FUNCTION__, e->value);

    return 0;
}

int main(void)
{
    int numbers[] = {1, 5};//, 0, 0, 2, 4, 5, 7};
    cl_stack_t *stack = NULL;
    cl_stack_node_t *node;
    int i, j, size;
    struct node_example e = { 7 };

    size = (sizeof(numbers) / sizeof(numbers[0]));
    cl_init(NULL);

    /* Test 2 */
    stack = cl_stack_create(destroy_node_example, compare_node_example,
                          NULL, equals_node_example);

    for (i = 0; i < (sizeof(numbers) / sizeof(numbers[0])); i++)
        cl_stack_push(stack, new_node_example(numbers[i]),
                    sizeof(struct node_example));

    printf("Stack size = %d\n", cl_stack_size(stack));
    cl_stack_map(stack, print_node, NULL);

    printf("Stack top:\n");
    node = cl_stack_peek(stack);
    print_node(node, NULL);
    cl_stack_node_unref(node);

    printf("Sorted stack\n");
    cl_stack_sort(stack);
    cl_stack_map(stack, print_node, NULL);

    if (cl_stack_contains(stack, &e, sizeof(struct node_example)) == true) {
        printf("Index of item with value = %d => %d\n", e.value,
               cl_stack_indexof(stack, &e, sizeof(struct node_example)));
    }

    cl_stack_destroy(stack);

    /* Test 2 */
    printf("Insert the first half of the array\n");
    stack = cl_stack_create(destroy_node_example, compare_node_example,
                          NULL, equals_node_example);

    for (i = 0; i < (size / 2); i++)
        cl_stack_push(stack, new_node_example(numbers[i]),
                       sizeof(struct node_example));

    printf("Queue size = %d\n", cl_stack_size(stack));
    printf("Map:\n");
    cl_stack_map(stack, print_node, NULL);

    printf("Queue top:\n");
    node = cl_stack_peek(stack);
    print_node(node, NULL);
    cl_stack_node_unref(node);

    printf("Inserts the other half, one by one and removing from the stack\n");

    for (i = (size / 2); i < size; i++) {
        node = cl_stack_pop(stack);
        cl_stack_push(stack, new_node_example(numbers[i]),
                       sizeof(struct node_example));

        cl_stack_node_unref(node);
    }

    printf("Queue top:\n");
    node = cl_stack_peek(stack);
    print_node(node, NULL);
    cl_stack_node_unref(node);

    printf("Map:\n");
    cl_stack_map(stack, print_node, NULL);
    cl_stack_destroy(stack);

    cl_uninit();

    return 0;
}

