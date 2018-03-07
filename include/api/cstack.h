
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Mar  1 21:52:59 BRT 2017
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

#ifndef _COLLECTIONS_API_CSTACK_H
#define _COLLECTIONS_API_CSTACK_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cstack.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_cstack_ref
 * @brief Increases the reference count for a cl_cstack_t item.
 *
 * @param [in,out] cstack: The circular stack item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cstack_t *cl_cstack_ref(cl_cstack_t *cstack);

/**
 * @name cl_cstack_unref
 * @brief Decreases the reference count for a cl_cstack_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] cstack: The circular stack item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_unref(cl_cstack_t *cstack);

/**
 * @name cl_cstack_create
 * @brief Creates a new circular stack object.
 *
 * This function creates a new circular stack to manipulate all kind of data
 * que size \a size.
 *
 * The function receives a few function pointers to be used by its API,
 * \a free_data is a function used to release the content of a circular stack
 * node, \a compare_to is a function to be used while sorting the stack, it must
 * return a negative value, zero or a positive value if the nodes are less then,
 * equal to or, greater then one another, \a filter is a function used to extract
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
 * @param [in] size: The maximum circular stack size.
 * @param [in] free_data: The free_data function pointer.
 * @param [in] compare_to: The compare_to function pointer.
 * @param [in] filter: The filter function pointer.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success a void object will be returned or NULL otherwise.
 */
cl_cstack_t *cl_cstack_create(unsigned int size,
                              void (*unref_node)(void *),
                              int (*compare_to)(cl_stack_node_t *,
                                                cl_stack_node_t *),
                              int (*filter)(cl_stack_node_t *, void *),
                              int (*equals)(cl_stack_node_t *,
                                            cl_stack_node_t *));

/**
 * @name cl_cstack_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the stack, the \a free_data function passed while
 * creating the stack is called.
 *
 * @param [in,out] cstack:  The circular stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_destroy(cl_cstack_t *cstack);

/**
 * @name cl_cstack_size
 * @brief Gets the circular stack size.
 *
 * @param [in] cstack:  The circular stack object.
 *
 * @return On success returns the size of the stack or -1 otherwise.
 */
int cl_cstack_size(cl_cstack_t *cstack);

/**
 * @name cl_cstack_push
 * @brief Inserts an element into the circular stack.
 *
 * Whe the circular stack reaches its limit, the older element will be removed
 * to insert a new one.
 *
 * @param [in,out] cstack:  The circular stack object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_push(cl_cstack_t *cstack, const void *data,
                   unsigned int data_size);

/**
 * @name cl_cstack_pop
 * @brief Retrieves and removes the head of the circular stack.
 *
 * @param [in,out] cstack:  The circular stack object.
 *
 * @return On success returns the node shifted off the stack, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
cl_stack_node_t *cl_cstack_pop(cl_cstack_t *cstack);

/**
 * @name cl_cstack_map
 * @brief Maps a function to every node on a circular stack.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_cstack_map(cl_cstack_t *cstack,
                               int (*foo)(cl_stack_node_t *, void *),
                                          void *data);

/**
 * @name cl_cstack_map_indexed
 * @brief Maps a function to every node on a circular stack.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_cstack_map_indexed(cl_cstack_t *cstack,
                                       int (*foo)(unsigned int,
                                                  cl_stack_node_t *,
                                                  void *),
                                       void *data);

/**
 * @name cl_cstack_map_reverse
 * @brief Maps a functions to every onde on a circular stack from the end to
 *        the top.
 *
 * The \a foo function receives as arguments a node from the stack and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_cstack_map_reverse(cl_cstack_t *cstack,
                                       int (*foo)(cl_stack_node_t *,
                                                  void *),
                                       void *data);

/**
 * @name cl_cstack_map_reverse_indexed
 * @brief Maps a function to every node on a circular stack from the end to the
 *        top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * stack, a node from the stack and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_stack_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_stack_node_t *cl_cstack_map_reverse_indexed(cl_cstack_t *cstack,
                                               int (*foo)(unsigned int,
                                                          cl_stack_node_t *,
                                                          void *),
                                               void *data);

/**
 * @name cl_cstack_at
 * @brief Gets a pointer to a specific node inside a circular stack.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] index: The node index inside the stack.
 *
 * @return On success returns a reference to the node or NULL otherwise.
 */
cl_stack_node_t *cl_cstack_at(cl_cstack_t *cstack, unsigned int index);

/**
 * @name cl_cstack_delete
 * @brief Deletes elements from a circular stack according a specific filter
 *        function.
 *
 * If the filter function returns a positive value the element will be extracted
 * from the stack and released from memory. This function uses the \a filter
 * function inside.
 *
 * @param [in,out] cstack:  The circular stack object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_delete(cl_cstack_t *cstack, void *data);

/**
 * @name cl_cstack_delete_indexed
 * @brief Deletes an element from a circular stack at a specific position.
 *
 * @param [in,out] cstack:  The circular stack object.
 * @param [in] index: The element position on the stack.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_delete_indexed(cl_cstack_t *cstack, unsigned int index);

/**
 * @name cl_cstack_move
 * @brief Moves all elements from a circular stack to another.
 *
 * @param [in] cstack: The original void object.
 *
 * @return Returns the new circular stack.
 */
cl_cstack_t *cl_cstack_move(cl_cstack_t *cstack);

/**
 * @name cl_cstack_filter
 * @brief Extracts elements from a circular stack according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 * This function uses the \a filter function to apply the filter on all elements
 * from the stack.
 *
 * @param [in,out] cstack:  The circular stack object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a circular stack containing all extracted elements from the
 *         original stack.
 */
cl_cstack_t *cl_cstack_filter(cl_cstack_t *cstack, void *data);

/**
 * @name cl_cstack_sort
 * @brief Sort all elements from a circular stack.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the stack and sort them.
 *
 * @param [in,out] cstack:  The circular stack object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_sort(cl_cstack_t *cstack);

/**
 * @name cl_cstack_indexof
 * @brief Gets the index of the first occurrence of an element inside the
 *        circular stack.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_cstack_indexof(cl_cstack_t *cstack, void *element, unsigned int size);

/**
 * @name cl_cstack_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the
 *        circular stack.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_cstack_last_indexof(cl_cstack_t *cstack, void *element,
                           unsigned int size);

/**
 * @name cl_cstack_contains
 * @brief Checks if a circular stack contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the stack.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] element: The element which will be sought through the stack.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns true if the element is found or false otherwise.
 */
bool cl_cstack_contains(cl_cstack_t *cstack, void *element, unsigned int size);

/**
 * @name cl_cstack_peek
 * @brief Retrieves, but does not remove, the head of the circular stack.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cstack:  The circular stack object.
 *
 * @return Returns NULL if the stack is empty or a new reference to the head
 *         of it.
 */
cl_stack_node_t *cl_cstack_peek(cl_cstack_t *cstack);

/**
 * @name cl_cstack_is_empty
 * @brief Tests to see if the circular stack is empty or not.
 *
 * @param [in] cstack:  The circular stack object.
 *
 * @return Returns true if the stack is empty or false otherwise.
 */
bool cl_cstack_is_empty(cl_cstack_t *cstack);

/**
 * @name cl_cstack_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_set_compare_to(cl_cstack_t *cstack,
                             int (*compare_to)(cl_stack_node_t *,
                                               cl_stack_node_t *));

/**
 * @name cl_cstack_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_set_filter(cl_cstack_t *cstack,
                         int (*filter)(cl_stack_node_t *, void *));

/**
 * @name cl_cstack_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] cstack:  The circular stack object.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cstack_set_equals(cl_cstack_t *cstack,
                         int (*equals)(cl_stack_node_t *, cl_stack_node_t *));

#endif

