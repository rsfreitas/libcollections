
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
#define clist_node_content(node)    \
    cglist_node_content((clist_node_t *)node, CLIST_NODE)

/**
 * @name clist_node_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] node: The node item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
#define clist_node_ref(node)        \
    (clist_node_t *)cglist_node_ref((clist_node_t *)node, CLIST_NODE)

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
#define clist_node_unref(node)      \
    cglist_node_unref((clist_node_t *)node, CLIST_NODE)

/**
 * @name clist_ref
 * @brief Increases the reference count for a void item.
 *
 * @param [in,out] list: The list item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
#define clist_ref(list)             \
    (clist_t *)cglist_ref((clist_t *)list, CLIST)

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
#define clist_unref(list)           \
    cglist_unref((clist_t *)list, CLIST)

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
#define clist_create(free_data, compare_to, filter, equals) \
    (clist_t *)cglist_create(CLIST, free_data, compare_to, filter, equals)

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
#define clist_destroy(list)         \
    clist_unref(list)

/**
 * @name clist_size
 * @brief Gets the list size.
 *
 * @param [in] list: The list object.
 *
 * @return On success returns the size of the list or -1 otherwise.
 */
#define clist_size(list)            \
    cglist_size((clist_t *)list, CLIST)

/**
 * @name clist_push
 * @brief Pushes a new node onto the list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define clist_push(list, node_content)                      \
    cglist_push((clist_t *)list, CLIST, node_content, CLIST_NODE)

/**
 * @name clist_pop
 * @brief Pop a node from a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the pop'ed node, and the user is responsible
 *         for releasing it, or NULL otherwise.
 */
#define clist_pop(list)             \
    cglist_pop((clist_t *)list, CLIST)

/**
 * @name clist_shift
 * @brief Shifts a node from the far end of a list.
 *
 * @param [in,out] list: The list object.
 *
 * @return On success returns the node shifted off the list, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
#define clist_shift(list)           \
    cglist_shift((clist_t *)list, CLIST)

/**
 * @name clist_unshift
 * @brief Shifts a node onto the far end of a list.
 *
 * @param [in,out] list: The list object.
 * @param [in] node_content: The content of the new node.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define clist_unshift(list, node_content)                   \
    cglist_unshift((clist_t *)list, CLIST, node_content, CLIST_NODE)

/**
 * @name clist_map
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(clist_node_t *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define clist_map(list, foo, data)  \
    cglist_map((clist_t *)list, CLIST, foo, data)

/**
 * @name clist_map_indexed
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, clist_node_t *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define clist_map_indexed(list, foo, data)                  \
    cglist_map_indexed((clist_t *)list, CLIST, foo, data)

/**
 * @name clist_map_reverse
 * @brief Maps a functions to every onde on a list from the end to the top.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something of this type:
 * int foo(clist_node_t *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define clist_map_reverse(list, foo, data)                  \
    cglist_map_reverse((clist_t *)list, CLIST, foo, data)

/**
 * @name clist_map_reverse_indexed
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * list, a node from the list and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, clist_node_t *, void *);
 *
 * @param [in] list: The list object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a pointer to the current node
 *         content, and if it is list of cobject_t objects returns a new
 *         reference to it. If not returns NULL.
 */
#define clist_map_reverse_indexed(list, foo, data)          \
    cglist_map_reverse_indexed((clist_t *)list, CLIST, foo, data)

/**
 * @name clist_at
 * @brief Gets a pointer to a specific node inside a list.
 *
 * @param [in] list: The list object.
 * @param [in] index: The node index inside the list.
 *
 * @return On success returns the node content, and if it is a list of cobject_t
 *         objects returns a new reference to it, or NULL otherwise.
 */
#define clist_at(list, index)       \
    cglist_at((clist_t *)list, CLIST, index)

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
#define clist_delete(list, data)    \
    cglist_delete((clist_t *)list, CLIST, data)

/**
 * @name clist_delete_indexed
 * @brief Deletes an element from a list at a specific position.
 *
 * @param [in,out] list: The list object.
 * @param [in] index: The element position on the list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
#define clist_delete_indexed(list, index)                   \
    cglist_delete_indexed((clist_t *)list, CLIST, index)

/**
 * @name clist_move
 * @brief Moves all elements from a list to another.
 *
 * @param [in] list: The original void object.
 *
 * @return Returns the new list.
 */
#define clist_move(list)            \
    cglist_move((clist_t *)list, CLIST)

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
#define clist_filter(list, data)    \
    cglist_filter((clist_t *)list, CLIST, data)

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
#define clist_sort(list)            \
    cglist_sort((clist_t *)list, CLIST)

/**
 * @name clist_indexof
 * @brief Gets the index of the first occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] content: The element which will be sought through the list.
 *
 * @return Returns the element index or -1 if it is not found.
 */
#define clist_indexof(list, content)                        \
    cglist_indexof((clist_t *)list, CLIST, content, CLIST_NODE)

/**
 * @name clist_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the list.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] content: The element which will be sought through the list.
 *
 * @return Returns the element index or -1 if it is not found.
 */
#define clist_last_indexof(list, content)                   \
    cglist_last_indexof((clist_t *)list, CLIST, content, CLIST_NODE)

/**
 * @name clist_contains
 * @brief Checks if a list contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the list.
 *
 * @param [in] list: The list object.
 * @param [in] content: The element which will be sought through the list.
 *
 * @return Returns true if the element is found or false otherwise.
 */
#define clist_contains(list, content)                       \
    cglist_contains((clist_t *)list, CLIST, content, CLIST_NODE)

#endif

