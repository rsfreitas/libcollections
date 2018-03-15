
/*
 * Description: API to handle linked lists.
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

#ifndef _COLLECTIONS_API_LIST_H
#define _COLLECTIONS_API_LIST_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <list.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_list_node_content
 * @brief A function to retrieve the content of a list node.
 *
 * This function must be used by the user to get a reference to its own object
 * from events functions, like compare_to, filter, equals and functions passed
 * to _map_ functions.
 *
 * @param [in] node: A list node.
 *
 * @return On success returns the content of the node or NULL otherwise.
 */
void *cl_list_node_content(cl_list_node_t *node);

/**
 * @name cl_list_node_ref
 * @brief Increases the reference count for a cl_list_node_t item.
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_list_node_t *cl_list_node_ref(cl_list_node_t *node);

/**
 * @name cl_list_node_unref
 * @brief Decreases the reference count for a cl_list_node_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_node_unref(cl_list_node_t *node);

/**
 * @name cl_list_ref
 * @brief Increases the reference count for a cl_list_t item.
 *
 * @param [in,out] list: The list item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_list_t *cl_list_ref(cl_list_t *list);

/**
 * @name cl_list_unref
 * @brief Decreases the reference count for a cl_list_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] list: The list item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_unref(cl_list_t *list);

/**
 * @name cl_list_create
 * @brief Creates a new list object.
 *
 * This function creates a new list object to manipulate all kind of data.
 *
 * The function receives a few function pointers to be used by its API,
 * \a free_data is a function used to release the content of a list node,
 * \a compare_to is a function to be used while sorting a list, it must return
 * a negative value, zero or a positive value if the nodes are less then, equal
 * to or, greater then one another, \a filter is a function used to extract
 * elements from the list if it returns a positive value and \a equals is also a
 * function to compare two nodes except that it must return a positive value
 * when the nodes are equal or zero otherwise.
 *
 * Their prototypes are the following:
 *
 * void free_data(void *);
 * int compare_to(cl_list_node_t *, cl_list_node_t *);
 * int filter(cl_list_node_t *, void *);
 * int equals(cl_list_node_t *, cl_list_node_t *);
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
cl_list_t *cl_list_create(void (*free_data)(void *),
                          int (*compare_to)(cl_list_node_t *, cl_list_node_t *),
                          int (*filter)(cl_list_node_t *, void *),
                          int (*equals)(cl_list_node_t *, cl_list_node_t *));

/**
 * @name cl_list_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the list, the \a free_data function passed while
 * creating the list is called.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_destroy(cl_list_t *list);

/**
 * @name cl_list_size
 * @brief Gets the list size.
 *
 * @param [in] list: The list object.
 *
 * @return On success returns the size of the list or -1 otherwise.
 */
int cl_list_size(const cl_list_t *list);

/**
 * @name cl_list_push
 * @brief Pushes a new node onto the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_push(cl_list_t *list, const void *node_content, unsigned int size);

/**
 * @name cl_list_pop
 * @brief Pop a node from a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
cl_list_node_t *cl_list_pop(cl_list_t *list);

/**
 * @name cl_list_shift
 * @brief Shifts a node from the far end of a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the node shifted off the list, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
cl_list_node_t *cl_list_shift(cl_list_t *list);

/**
 * @name cl_list_unshift
 * @brief Shifts a node onto the far end of a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_unshift(cl_list_t *list, const void *node_content, unsigned int size);

/**
 * @name cl_list_map
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_list_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_list_node_t *cl_list_map(const cl_list_t *list,
                            int (*foo)(cl_list_node_t *, void *), void *data);

/**
 * @name cl_list_map_indexed
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_list_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_list_node_t *cl_list_map_indexed(const cl_list_t *list,
                                    int (*foo)(unsigned int, cl_list_node_t *,
                                               void *),
                                    void *data);

/**
 * @name cl_list_map_reverse
 * @brief Maps a functions to every onde on a list from the end to the top.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_list_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_list_node_t *cl_list_map_reverse(const cl_list_t *list,
                                    int (*foo)(cl_list_node_t *, void *),
                                    void *data);

/**
 * @name cl_list_map_reverse_indexed
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_list_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_list_node_t *cl_list_map_reverse_indexed(const cl_list_t *list,
                                            int (*foo)(unsigned int,
                                                       cl_list_node_t *,
                                                       void *),
                                            void *data);

/**
 * @name cl_list_at
 * @brief Gets a pointer to a specific node inside a list.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] index: The node index inside the list.
 *
 * @return On success returns a reference to the node or NULL otherwise.
 */
cl_list_node_t *cl_list_at(const cl_list_t *list, unsigned int index);

/**
 * @name cl_list_delete
 * @brief Deletes elements from a list according a specific filter function.
 *
 * If the filter function returns a positive value the element will be extracted
 * from the list and released from memory. This function uses the \a filter
 * function inside.
 *
 * @param [in,out] list: The list object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_delete(cl_list_t *list, void *data);

/**
 * @name cl_list_delete_indexed
 * @brief Deletes an element from a list at a specific position.
 *
 * @param [in,out] list: The list object.
 * @param [in] index: The element position on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_delete_indexed(cl_list_t *list, unsigned int index);

/**
 * @name cl_list_move
 * @brief Moves all elements from a list to another.
 *
 * @param [in] list: The original void object.
 *
 * @return Returns the new list.
 */
cl_list_t *cl_list_move(cl_list_t *list);

/**
 * @name cl_list_filter
 * @brief Extracts elements from a list according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 * This function uses the \a filter function to apply the filter on all elements
 * from the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a list containing all extracted elements from the original
 *         list.
 */
cl_list_t *cl_list_filter(cl_list_t *list, void *data);

/**
 * @name cl_list_sort
 * @brief Sort all elements from a list.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the list and sort them.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_sort(cl_list_t *list);

/**
 * @name cl_list_indexof
 * @brief Gets the index of the first occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] element: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_list_indexof(const cl_list_t *list, void *element, unsigned int size);

/**
 * @name cl_list_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] element: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_list_last_indexof(const cl_list_t *list, void *element, unsigned int size);

/**
 * @name cl_list_contains
 * @brief Checks if a list contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] element: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns true if the element is found or false otherwise.
 */
bool cl_list_contains(const cl_list_t *list, void *element, unsigned int size);

/**
 * @name cl_list_peek
 * @brief Retrieves, but does not remove, the head of the list.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 *
 * @return Returns a reference to the head of the list on success or NULL
 *         otherwise. The head will be a node from the list, so the user will
 *         have to use the function to get the content of it.
 */
cl_list_node_t *cl_list_peek(const cl_list_t *list);

/**
 * @name cglist_is_empty
 * @brief Tests to see if the list is empty or not.
 *
 * @param [in] list: The list object.
 *
 * @return Returns true if the list is empty or false otherwise.
 */
bool cl_list_is_empty(const cl_list_t *list);

/**
 * @name cl_list_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] list: The list object.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_set_compare_to(const cl_list_t *list,
                           int (*compare_to)(cl_list_node_t *, cl_list_node_t *));

/**
 * @name cl_list_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] list: The list object.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_set_filter(const cl_list_t *list,
                       int (*filter)(cl_list_node_t *, void *));

/**
 * @name cl_list_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] list: The list object.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_set_equals(const cl_list_t *list,
                       int (*equals)(cl_list_node_t *, cl_list_node_t *));

/**
 * @name cl_list_middle
 * @brief Gives the element from the middle of the list.
 *
 * @param [in] list: The list object.
 *
 * @return On success returns the middle element or NULL otherwise.
 */
cl_list_node_t *cl_list_middle(const cl_list_t *list);

/**
 * @name cl_list_rotate
 * @brief Rotate the list elements by n positions.
 *
 * @param [in] list: The list object.
 * @param [in] n: The number of rotated positions.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_list_rotate(cl_list_t *list, unsigned int n);

#endif

