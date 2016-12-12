
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Sep 3 17:12:17 2016
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA
 */

#ifndef _COLLECTIONS_CL_GLIST_H
#define _COLLECTIONS_CL_GLIST_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
# error "Never use <cl_glist.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cglist_node_content
 * @brief A function to retrieve the content of a list node.
 *
 * This function must be used by the user to get a reference to its own object
 * from events functions, like compare_to, filter, equals and functions passed
 * to _map_ functions.
 *
 * @param [in] node: A list node.
 * @param [in] object: The type of the node.
 *
 * @return On success returns the content of the node or NULL otherwise.
 */
void *cglist_node_content(const void *node, enum cl_object object);

/**
 * @name cglist_node_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] node: The node item.
 * @param [in] object: The type of the node.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
void *cglist_node_ref(void *node, enum cl_object object);

/**
 * @name cglist_node_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] node: The node item.
 * @param [in] object: The type of the node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_node_unref(void *node, enum cl_object object);

/**
 * @name cglist_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] list: The list item.
 * @param [in] object: The type of the list.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
void *cglist_ref(void *list, enum cl_object object);

/**
 * @name cglist_unref
 * @brief Decreases the reference count for a void item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] list: The list item.
 * @param [in] object: The type of the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_unref(void *list, enum cl_object object);

/**
 * @name cglist_create
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
 * int compare_to(void *, void *);
 * int filter(void *, void *);
 * int equals(void *, void *);
 *
 * If the type of a node content if of a cobject_t kind it is not necessary to
 * pass the arguments \a free_data, \a compare_to and \a equals, since them can
 * exist inside a cobject_t object.
 *
 * @param [in] object: The type of the list.
 * @param [in] free_data: The free_data function pointer.
 * @param [in] compare_to: The compare_to function pointer.
 * @param [in] filter: The filter function pointer.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success a void object will be returned or NULL otherwise.
 */
void *cglist_create(enum cl_object object, void (*free_data)(void *),
                    int (*compare_to)(void *, void *),
                    int (*filter)(void *, void *),
                    int (*equals)(void *, void *));

/**
 * @name cglist_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the list, the \a free_data function passed while
 * creating the list is called.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_destroy(void *list, enum cl_object object);

/**
 * @name cglist_size
 * @brief Gets the list size.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return On success returns the size of the list or -1 otherwise.
 */
int cglist_size(const void *list, enum cl_object object);

/**
 * @name cglist_push
 * @brief Pushes a new node onto the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 * @param [in] node_object: The type of the node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_push(void *list, enum cl_object object, const void *node_content,
                unsigned int size, enum cl_object node_object);

/**
 * @name cglist_pop
 * @brief Pop a node from a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
void *cglist_pop(void *list, enum cl_object object);

/**
 * @name cglist_shift
 * @brief Shifts a node from the far end of a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return On success returns the node shifted off the list, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
void *cglist_shift(void *list, enum cl_object object);

/**
 * @name cglist_unshift
 * @brief Shifts a node onto the far end of a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 * @param [in] node_object: The type of the node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_unshift(void *list, enum cl_object object, const void *node_content,
                   unsigned int size, enum cl_object node_object);

/**
 * @name cglist_map
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(void *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
void *cglist_map(const void *list, enum cl_object object,
                 int (*foo)(void *, void *), void *data);

/**
 * @name cglist_map_indexed
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, void *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
void *cglist_map_indexed(const void *list, enum cl_object object,
                         int (*foo)(unsigned int, void *, void *),
                         void *data);

/**
 * @name cglist_map_reverse
 * @brief Maps a functions to every onde on a list from the end to the top.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(void *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
void *cglist_map_reverse(const void *list, enum cl_object object,
                         int (*foo)(void *, void *), void *data);

/**
 * @name cglist_map_reverse_indexed
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, void *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
void *cglist_map_reverse_indexed(const void *list, enum cl_object object,
                                 int (*foo)(unsigned int, void *, void *),
                                 void *data);

/**
 * @name cglist_at
 * @brief Gets a pointer to a specific node inside a list.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] index: The node index inside the list.
 *
 * @return On success returns the node content, and if it is a list of cobject_t
 *         objects returns a new reference to it, or NULL otherwise.
 */
void *cglist_at(const void *list, enum cl_object object, unsigned int index);

/**
 * @name cglist_delete
 * @brief Deletes elements from a lista according a specific filter function.
 *
 * If the filter function returns a positive value the element will be extracted
 * from the list and released from memory. This function uses the \a filter
 * function inside.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_delete(void *list, enum cl_object object, void *data);

/**
 * @name cglist_delete_indexed
 * @brief Deletes an element from a list at a specific position.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] index: The element position on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_delete_indexed(void *list, enum cl_object object, unsigned int index);

/**
 * @name cglist_move
 * @brief Moves all elements from a list to another.
 *
 * @param [in] list: The original void object.
 * @param [in] object: The type of the list.
 *
 * @return Returns the new list.
 */
void *cglist_move(void *list, enum cl_object object);

/**
 * @name cglist_filter
 * @brief Extracts elements from a list according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 * This function uses the \a filter function to apply the filter on all elements
 * from the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a list containing all extracted elements from the original
 *         list.
 */
void *cglist_filter(void *list, enum cl_object object, void *data);

/**
 * @name cglist_sort
 * @brief Sort all elements from a list.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the list and sort them.
 *
 * @param [in,out] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_sort(void *list, enum cl_object object);

/**
 * @name cglist_indexof
 * @brief Gets the index of the first occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] content: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 * @param [in] node_object: The type of the node.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cglist_indexof(const void *list, enum cl_object object, void *content,
                   unsigned int size, enum cl_object node_object);

/**
 * @name cglist_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] content: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 * @param [in] node_object: The type of the node.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cglist_last_indexof(const void *list, enum cl_object object, void *content,
                        unsigned int size, enum cl_object node_object);

/**
 * @name cglist_contains
 * @brief Checks if a list contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] content: The element which will be sought through the list.
 * @param [in] size: The size in bytes of the element.
 * @param [in] node_object: The type of the node.
 *
 * @return Returns true if the element is found or false otherwise.
 */
bool cglist_contains(const void *list, enum cl_object object, void *content,
                     unsigned int size, enum cl_object node_object);

/**
 * @name cglist_peek
 * @brief Retrieves, but does not remove, the head of the list.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return Returns the head of the list on success or NULL otherwise. The head
 *         will be a node from the list, so the user will have to use the
 *         function to get the content of it.
 */
void *cglist_peek(const void *list, enum cl_object object,
                  enum cl_object node_object);

/**
 * @name cglist_is_empty
 * @brief Tests to see if the list is empty or not.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 *
 * @return Returns true if the list is empty or false otherwise.
 */
bool cglist_is_empty(const void *list, enum cl_object object);

/**
 * @name cglist_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_set_compare_to(const void *list, enum cl_object object,
                          int (*compare_to)(void *, void *));

/**
 * @name cglist_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_set_filter(const void *list, enum cl_object object,
                      int (*filter)(void *, void *));

/**
 * @name cglist_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] list: The list object.
 * @param [in] object: The type of the list.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cglist_set_equals(const void *list, enum cl_object object,
                      int (*equals)(void *, void *));

#endif

