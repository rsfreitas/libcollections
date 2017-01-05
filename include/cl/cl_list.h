
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

#ifndef _COLLECTIONS_CL_LIST_H
#define _COLLECTIONS_CL_LIST_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_list.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name clist_node_content
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
void *clist_node_content(clist_node_t *node);

/**
 * @name clist_node_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
clist_node_t *clist_node_ref(clist_node_t *node);

/**
 * @name clist_node_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_node_unref(clist_node_t *node);

/**
 * @name clist_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] list: The list item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
clist_t *clist_ref(clist_t *list);

/**
 * @name clist_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] list: The list item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_unref(clist_t *list);

/**
 * @name clist_create
 * @brief Creates a new list object.
 *
 * This function creates a new list object to manipulate all kind of data. It is
 * not recommended manipulate primitive types.
 *
 * The function receives a few function pointers to be used by its API,
 * \a free_data is a function used to release the content of a list node,
 * \a compare_to is a function to be used while sorting a list it must return
 * a negative value, zero or a positive value if the nodes are less then, equal
 * to or, greater then one another, \a filter is a function used to extract
 * elements from the list if it returns a positive value and \a equals is also a
 * function to compare two nodes except that it must return a positive value
 * when the nodes are equal or zero otherwise.
 *
 * Their prototypes are the following:
 *
 * void free_data(void *);
 * int compare_to(clist_node_t *, clist_node_t *);
 * int filter(clist_node_t *, void *);
 * int equals(clist_node_t *, clist_node_t *);
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
clist_t *clist_create(void (*free_data)(void *),
                      int (*compare_to)(clist_node_t *, clist_node_t *),
                      int (*filter)(clist_node_t *, void *),
                      int (*equals)(clist_node_t *, clist_node_t *));

/**
 * @name clist_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the list, the \a free_data function passed while
 * creating the list is called.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_destroy(clist_t *list);

/**
 * @name clist_size
 * @brief Gets the list size.
 *
 * @param [in] list: The list object.
 *
 * @return On success returns the size of the list or -1 otherwise.
 */
int clist_size(const clist_t *list);

/**
 * @name clist_push
 * @brief Pushes a new node onto the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_push(clist_t *list, const void *node_content,
               unsigned int size);

/**
 * @name clist_pop
 * @brief Pop a node from a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
clist_node_t *clist_pop(clist_t *list);

/**
 * @name clist_shift
 * @brief Shifts a node from the far end of a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the node shifted off the list, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
clist_node_t *clist_shift(clist_t *list);

/**
 * @name clist_unshift
 * @brief Shifts a node onto the far end of a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_unshift(clist_t *list, const void *node_content,
                  unsigned int size);

/**
 * @name clist_map
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(clist_node_t *, void *);
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
clist_node_t *clist_map(const clist_t *list,
                        int (*foo)(clist_node_t *, void *), void *data);

/**
 * @name clist_map_indexed
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, clist_node_t *, void *);
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
clist_node_t *clist_map_indexed(const clist_t *list,
                                int (*foo)(unsigned int, clist_node_t *, void *),
                                void *data);

/**
 * @name clist_map_reverse
 * @brief Maps a functions to every onde on a list from the end to the top.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(clist_node_t *, void *);
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
clist_node_t *clist_map_reverse(const clist_t *list,
                                int (*foo)(clist_node_t *, void *), void *data);

/**
 * @name clist_map_reverse_indexed
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, clist_node_t *, void *);
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
clist_node_t *clist_map_reverse_indexed(const clist_t *list,
                                        int (*foo)(unsigned int, clist_node_t *,
                                                   void *),
                                        void *data);

/**
 * @name clist_at
 * @brief Gets a pointer to a specific node inside a list.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] list: The list object.
 * @param [in] index: The node index inside the list.
 *
 * @return On success returns a reference to the node or NULL otherwise.
 */
clist_node_t *clist_at(const clist_t *list, unsigned int index);

/**
 * @name clist_delete
 * @brief Deletes elements from a lista according a specific filter function.
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
int clist_delete(clist_t *list, void *data);

/**
 * @name clist_delete_indexed
 * @brief Deletes an element from a list at a specific position.
 *
 * @param [in,out] list: The list object.
 * @param [in] index: The element position on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_delete_indexed(clist_t *list, unsigned int index);

/**
 * @name clist_move
 * @brief Moves all elements from a list to another.
 *
 * @param [in] list: The original void object.
 *
 * @return Returns the new list.
 */
clist_t *clist_move(clist_t *list);

/**
 * @name clist_filter
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
clist_t *clist_filter(clist_t *list, void *data);

/**
 * @name clist_sort
 * @brief Sort all elements from a list.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the list and sort them.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_sort(clist_t *list);

/**
 * @name clist_indexof
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
int clist_indexof(const clist_t *list, void *element,
                  unsigned int size);

/**
 * @name clist_last_indexof
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
int clist_last_indexof(const clist_t *list, void *element,
                       unsigned int size);

/**
 * @name clist_contains
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
bool clist_contains(const clist_t *list, void *element,
                    unsigned int size);

/**
 * @name clist_peek
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
clist_node_t *clist_peek(const clist_t *list);

/**
 * @name cglist_is_empty
 * @brief Tests to see if the list is empty or not.
 *
 * @param [in] list: The list object.
 *
 * @return Returns true if the list is empty or false otherwise.
 */
bool clist_is_empty(const clist_t *list);

/**
 * @name clist_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] list: The list object.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_set_compare_to(const clist_t *list,
                         int (*compare_to)(clist_node_t *, clist_node_t *));

/**
 * @name clist_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] list: The list object.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_set_filter(const clist_t *list,
                     int (*filter)(clist_node_t *, void *));

/**
 * @name clist_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] list: The list object.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int clist_set_equals(const clist_t *list,
                     int (*equals)(clist_node_t *, clist_node_t *));

#endif

