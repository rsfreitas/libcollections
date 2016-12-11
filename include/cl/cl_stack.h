
/*
 * Description: API to handle stacks.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Dec  7 10:15:29 BRST 2016
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

#ifndef _COLLECTIONS_CL_STACK_H
#define _COLLECTIONS_CL_STACK_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_stack.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cstack_node_content
 * @brief A function to retrieve the content of a stack node.
 *
 * This function must be used by the user to get a reference to its own object
 * from events functions, like compare_to, filter, equals and functions passed
 * to _map_ functions.
 *
 * @param [in] node: A stack node.
 *
 * @return On success returns the content of the node or NULL otherwise.
 */
#define cstack_node_content(node)   \
    cglist_node_content((cstack_node_t *)node, CSTACK_NODE)

/**
 * @name cstack_node_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
#define cstack_node_ref(node)       \
    (cstack_node_t *)cglist_node_ref((cstack_node_t *)node, CSTACK_NODE)

/**
 * @name cstack_node_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_node_unref(node)     \
    cglist_node_unref((cstack_node_t *)node, CSTACK_NODE)

/**
 * @name cstack_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] stack: The stack item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
#define cstack_ref(stack)           \
    (cstack_t *)cglist_ref((cstack_t *)stack, CSTACK)

/**
 * @name cstack_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] stack: The stack item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_unref(stack)         \
    cglist_unref((cstack_t *)stack, CSTACK)

/**
 * @name cstack_create
 * @brief Creates a new stack object.
 *
 * This function creates a new stack object to manipulate all kind of data. It is
 * not recommended manipulate primitive types.
 *
 * The function receives a few function pointers to be used by its API,
 * \a free_data is a function used to release the content of a stack node,
 * \a compare_to is a function to be used while sorting a stack it must return
 * a negative value, zero or a positive value if the nodes are less then, equal
 * to or, greater then one another, \a filter is a function used to extract
 * elements from the stack if it returns a positive value and \a equals is also a
 * function to compare two nodes except that it must return a positive value
 * when the nodes are equal or zero otherwise.
 *
 * Their prototypes are the following:
 *
 * void free_data(void *);
 * int compare_to(cstack_node_t *, cstack_node_t *);
 * int filter(cstack_node_t *, void *);
 * int equals(cstack_node_t *, cstack_node_t *);
 *
 * If the type of a node content if of a cobject_t kind it is not necessary to
 * pass the arguments \a free_data, \a compare_to and \a equals, since them can
 * exist inside a cobject_t object.
 *
 * @param [in] free_data: The free_data function pointer.
 * @param [in] compare_to: The compare_to function pointer.
 * @param [in] filter: The filter function pointer.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success a void object will be returned or NULL otherwise.
 */
#define cstack_create(free_data, compare_to, filter, equals)    \
    (cstack_t *)cglist_create(CSTACK, free_data, compare_to, filter, equals)

/**
 * @name cstack_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the stack, the \a free_data function passed while
 * creating the stack is called.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_destroy(stack)       \
    cstack_unref(stack)

/**
 * @name cstack_size
 * @brief Gets the stack size.
 *
 * @param [in] stack: The stack object.
 *
 * @return On success returns the size of the stack or -1 otherwise.
 */
#define cstack_size(stack)          \
    cglist_size((cstack_t *)stack, CSTACK)

/**
 * @name cstack_push
 * @brief Pushes a new node onto the stack.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the element.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_push(stack, node_content, size)                      \
    cglist_push((cstack_t *)stack, CSTACK, node_content, size, CSTACK_NODE)

/**
 * @name cstack_pop
 * @brief Pop a node from a stack.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
#define cstack_pop(stack)           \
    (cstack_node_t *)cglist_shift((cstack_t *)stack, CSTACK)

/**
 * @name cstack_map
 * @brief Maps a function to every node on a stack.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cstack_node_t *, void *);
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         element, and if it is stack of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define cstack_map(stack, foo, data)                            \
    cglist_map((cstack_t *)stack, CSTACK, foo, data)

/**
 * @name cstack_map_indexed
 * @brief Maps a function to every node on a stack.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cstack_node_t *, void *);
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         element, and if it is stack of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define cstack_map_indexed(stack, foo, data)                    \
    cglist_map_indexed((cstack_t *)stack, CSTACK, foo, data)

/**
 * @name cstack_map_reverse
 * @brief Maps a functions to every onde on a stack from the end to the top.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cstack_node_t *, void *);
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         element, and if it is stack of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define cstack_map_reverse(stack, foo, data)                    \
    cglist_map_reverse((cstack_t *)stack, CSTACK, foo, data)

/**
 * @name cstack_map_reverse_indexed
 * @brief Maps a function to every node on a stack from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cstack_node_t *, void *);
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         element, and if it is stack of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define cstack_map_reverse_indexed(stack, foo, data)            \
    cglist_map_reverse_indexed((cstack_t *)stack, CSTACK, foo, data)

/**
 * @name cstack_at
 * @brief Gets a pointer to a specific node inside a stack.
 *
 * @param [in] stack: The stack object.
 * @param [in] index: The node index inside the stack.
 *
 * @return On success returns the node element, and if it is a stack of cobject_t
 *         objects returns a new reference to it, or NULL otherwise.
 */
#define cstack_at(stack, index)     \
    cglist_at((cstack_t *)stack, CSTACK, index)

/**
 * @name cstack_delete
 * @brief Deletes elements from a lista according a specific filter function.
 *
 * If the filter function returns a positive value the element will be extracted
 * from the stack and released from memory. This function uses the \a filter
 * function inside.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_delete(stack, data)  \
    cglist_delete((cstack_t *)stack, CSTACK, data)

/**
 * @name cstack_delete_indexed
 * @brief Deletes an element from a stack at a specific position.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] index: The element position on the stack.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_delete_indexed(stack, index)                     \
    cglist_delete_indexed((cstack_t *)stack, CSTACK, index)

/**
 * @name cstack_move
 * @brief Moves all elements from a stack to another.
 *
 * @param [in] stack: The original void object.
 *
 * @return Returns the new stack.
 */
#define cstack_move(stack)          \
    cglist_move((cstack_t *)stack, CSTACK)

/**
 * @name cstack_filter
 * @brief Extracts elements from a stack according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 * This function uses the \a filter function to apply the filter on all elements
 * from the stack.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a stack containing all extracted elements from the original
 *         stack.
 */
#define cstack_filter(stack, data)  \
    cglist_filter((cstack_t *)stack, CSTACK, data)

/**
 * @name cstack_sort
 * @brief Sort all elements from a stack.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the stack and sort them.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define cstack_sort(stack)          \
    cglist_sort((cstack_t *)stack, CSTACK)

/**
 * @name cstack_indexof
 * @brief Gets the index of the first occurrence of an element inside the stack.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] stack: The stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
#define cstack_indexof(stack, element, size)                    \
    cglist_indexof((cstack_t *)stack, CSTACK, element, size, CSTACK_NODE)

/**
 * @name cstack_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the stack.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] stack: The stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
#define cstack_last_indexof(stack, element, size)                   \
    cglist_last_indexof((cstack_t *)stack, CSTACK, element, size, CSTACK_NODE)

/**
 * @name cstack_contains
 * @brief Checks if a stack contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] stack: The stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns true if the element is found or false otherwise.
 */
#define cstack_contains(stack, element, size)                       \
    cglist_contains((cstack_t *)stack, CSTACK, element, size, CSTACK_NODE)

/**
 * @name cstack_peek
 * @brief Retrieves, but does not remove, the head of the stack.
 *
 * @param [in] stack: The stack object.
 *
 * @return Returns NULL if the stack is empty or the head of it.
 */
#define cstack_peek(stack)          \
    (cstack_node_t *)cglist_peek((cstack_t *)stack, CSTACK, CSTACK_NODE)

/**
 * @name cstack_is_empty
 * @brief Tests to see if the stack is empty or not.
 *
 * @param [in] stack: The stack object.
 *
 * @return Returns true if the list is empty or false otherwise.
 */
#define cstack_is_empty(stack)      \
    cglist_is_empty((cstack_t *)stack, CSTACK)

#endif

