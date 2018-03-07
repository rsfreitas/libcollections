
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

#ifndef _COLLECTIONS_API_STACK_H
#define _COLLECTIONS_API_STACK_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <stack.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_stack_node_content
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
void *cl_stack_node_content(cl_stack_node_t *node);

/**
 * @name cl_stack_node_ref
 * @brief Increases the reference count for a cl_stack_node_t item.
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_stack_node_t *cl_stack_node_ref(cl_stack_node_t *node);

/**
 * @name cl_stack_node_unref
 * @brief Decreases the reference count for a cl_stack_node_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_node_unref(cl_stack_node_t *node);

/**
 * @name cl_stack_ref
 * @brief Increases the reference count for a cl_stack_t item.
 *
 * @param [in,out] stack: The stack item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_stack_t *cl_stack_ref(cl_stack_t *stack);

/**
 * @name cl_stack_unref
 * @brief Decreases the reference count for a cl_stack_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] stack: The stack item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_unref(cl_stack_t *stack);

/**
 * @name cl_stack_create
 * @brief Creates a new stack object.
 *
 * This function creates a new stack object to manipulate all kind of data.
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
 * int compare_to(cl_stack_node_t *, cl_stack_node_t *);
 * int filter(cl_stack_node_t *, void *);
 * int equals(cl_stack_node_t *, cl_stack_node_t *);
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
cl_stack_t *cl_stack_create(void (*free_data)(void *),
                            int (*compare_to)(cl_stack_node_t *,
                                              cl_stack_node_t *),
                            int (*filter)(cl_stack_node_t *, void *),
                            int (*equals)(cl_stack_node_t *,
                                          cl_stack_node_t *));

/**
 * @name cl_stack_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the stack, the \a free_data function passed while
 * creating the stack is called.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_destroy(cl_stack_t *stack);

/**
 * @name cl_stack_size
 * @brief Gets the stack size.
 *
 * @param [in] stack: The stack object.
 *
 * @return On success returns the size of the stack or -1 otherwise.
 */
int cl_stack_size(const cl_stack_t *stack);

/**
 * @name cl_stack_push
 * @brief Pushes a new node onto the stack.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the element.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_push(cl_stack_t *stack, const void *node_content, unsigned int size);

/**
 * @name cl_stack_pop
 * @brief Pop a node from a stack.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
cl_stack_node_t *cl_stack_pop(cl_stack_t *stack);

/**
 * @name cl_stack_map
 * @brief Maps a function to every node on a stack.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_stack_map(const cl_stack_t *stack,
                              int (*foo)(cl_stack_node_t *, void *), void *data);

/**
 * @name cl_stack_map_indexed
 * @brief Maps a function to every node on a stack.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_stack_map_indexed(const cl_stack_t *stack,
                                      int (*foo)(unsigned int, cl_stack_node_t *,
                                                 void *),
                                      void *data);

/**
 * @name cl_stack_map_reverse
 * @brief Maps a functions to every onde on a stack from the end to the top.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_stack_map_reverse(const cl_stack_t *stack,
                                      int (*foo)(cl_stack_node_t *, void *),
                                      void *data);

/**
 * @name cl_stack_map_reverse_indexed
 * @brief Maps a function to every node on a stack from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_stack_map_reverse_indexed(const cl_stack_t *stack,
                                              int (*foo)(unsigned int,
                                                         cl_stack_node_t *,
                                                         void *),
                                              void *data);

/**
 * @name cl_stack_at
 * @brief Gets a pointer to a specific node inside a stack.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 * @param [in] index: The node index inside the stack.
 *
 * @return On success returns a reference to the node or NULL otherwise.
 */
cl_stack_node_t *cl_stack_at(const cl_stack_t *stack, unsigned int index);

/**
 * @name cl_stack_delete
 * @brief Deletes elements from a stacka according a specific filter function.
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
int cl_stack_delete(cl_stack_t *stack, void *data);

/**
 * @name cl_stack_delete_indexed
 * @brief Deletes an element from a stack at a specific position.
 *
 * @param [in,out] stack: The stack object.
 * @param [in] index: The element position on the stack.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_delete_indexed(cl_stack_t *stack, unsigned int index);

/**
 * @name cl_stack_move
 * @brief Moves all elements from a stack to another.
 *
 * @param [in] stack: The original void object.
 *
 * @return Returns the new stack.
 */
cl_stack_t *cl_stack_move(cl_stack_t *stack);

/**
 * @name cl_stack_filter
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
cl_stack_t *cl_stack_filter(cl_stack_t *stack, void *data);

/**
 * @name cl_stack_sort
 * @brief Sort all elements from a stack.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the stack and sort them.
 *
 * @param [in,out] stack: The stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_sort(cl_stack_t *stack);

/**
 * @name cl_stack_indexof
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
int cl_stack_indexof(const cl_stack_t *stack, void *element, unsigned int size);

/**
 * @name cl_stack_last_indexof
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
int cl_stack_last_indexof(const cl_stack_t *stack, void *element,
                          unsigned int size);

/**
 * @name cl_stack_contains
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
bool cl_stack_contains(const cl_stack_t *stack, void *element, unsigned int size);

/**
 * @name cl_stack_peek
 * @brief Retrieves, but does not remove, the head of the stack.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] stack: The stack object.
 *
 * @return Returns NULL if the stack is empty or a new reference to the head
 *         of it.
 */
cl_stack_node_t *cl_stack_peek(const cl_stack_t *stack);

/**
 * @name cl_stack_is_empty
 * @brief Tests to see if the stack is empty or not.
 *
 * @param [in] stack: The stack object.
 *
 * @return Returns true if the stack is empty or false otherwise.
 */
bool cl_stack_is_empty(const cl_stack_t *stack);

/**
 * @name cl_stack_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] stack: The stack object.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_set_compare_to(const cl_stack_t *stack,
                            int (*compare_to)(cl_stack_node_t *,
                                              cl_stack_node_t *));

/**
 * @name cl_stack_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] stack: The stack object.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_set_filter(const cl_stack_t *stack,
                        int (*filter)(cl_stack_node_t *, void *));

/**
 * @name cl_stack_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] stack: The stack object.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_stack_set_equals(const cl_stack_t *stack,
                        int (*equals)(cl_stack_node_t *, cl_stack_node_t *));

#endif

