
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:19 2015
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

#ifndef _COLLECTIONS_CL_DLL_H
#define _COLLECTIONS_CL_DLL_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_dll.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_dll_size
 * @brief Returns the number of nodes of a list.
 *
 * @param [in] root: The list.
 *
 * @return Returns the number of nodes of a list.
 */
int cl_dll_size(void *root);

/**
 * @name cl_dll_push
 * @brief Pushes a new node onto the list.
 *
 * @param [in,out] root: The list.
 * @param [in] node: The new node.
 *
 * @return Returns the new list.
 */
void *cl_dll_push(void *root, void *node);

/**
 * @name cl_dll_pop
 * @brief Pop a node from a list.
 *
 * @param [in,out] root: The list.
 *
 * @return On success returns the pop'ed node or NULL otherwise.
 */
void *cl_dll_pop(void *root);

/**
 * @name cl_dll_unshift
 * @brief Shifts a node onto the far end of a list.
 *
 * @param [in,out] root: The list.
 * @param [in] node: The new node.
 *
 * @return Returns the new list.
 */
void *cl_dll_unshift(void *root, void *node);

/**
 * @name cl_dll_shift
 * @brief Shifts a node from the far end of a list.
 *
 * @param [in,out] root: The list.
 *
 * @return On success returns the node shifted off the list or NULL otherwise.
 */
void *cl_dll_shift(void *root);

/**
 * @name cl_dll_map
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something like this:
 * int foo(void *a, void *b);
 *
 * @param [in] root: The list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns non-zero returns a pointer to the current node.
 *         If not returns NULL.
 */
void *cl_dll_map(void *root, int (*foo)(void *, void *), void *data);

/**
 * @name cl_dll_map_indexed
 * @brief Maps a function to every node on a list.
 *
 * The \a foo function receives as arguments the current node index inside
 * the list, a node from the list and some custom \a data. Its prototype must
 * be something like this: int foo(unsigned int index, void *a, void *b);
 *
 * @param [in] root: The list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns non-zero returns a pointer to the current node.
 *         If not returns NULL.
 */
void *cl_dll_map_indexed(void *root, int (*foo)(unsigned int, void *, void *),
                         void *data);

/**
 * @name cl_dll_map_reverse
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments a node from the list and some
 * \a data. Its prototype must be something like this:
 * int foo(void *a, void *b);
 *
 * @param [in] root: The list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns non-zero returns a pointer to the current node.
 *         If not returns NULL.
 */
void *cl_dll_map_reverse(void *root, int (*foo)(void *, void *), void *data);

/**
 * @name cl_dll_map_indexed_reverse
 * @brief Maps a function to every node on a list from the end to the top.
 *
 * The \a foo function receives as arguments the current node index inside
 * the list, a node from the list and some custom \a data. Its prototype must
 * be something like this: int foo(unsigned int index, void *a, void *b);
 *
 * @param [in] root: The list.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns non-zero returns a pointer to the current node.
 *         If not returns NULL.
 */
void *cl_dll_map_indexed_reverse(void *root,
                                 int (*foo)(unsigned int, void *, void *),
                                 void *data);

/**
 * @name cl_dll_free
 * @brief Frees a list of nodes.
 *
 * If \a foo is NULL the functions uses the libc free function to deallocate
 * every node from the list.
 *
 * @param [in] root: The list.
 * @param [in] foo: The custom free function.
 */
void cl_dll_free(void *root, void (*foo)(void *));

/**
 * @name cl_dll_at
 * @brief Gets a pointer to a specific node inside a list.
 *
 * @param [in] root: The list.
 * @param [in] index: The node index inside the list.
 *
 * @return On success returns the node from the list or NULL otherwise.
 */
void *cl_dll_at(void *root, unsigned int index);

/**
 * @name cl_dll_move
 * @brief Moves all elements from a list to another.
 *
 * @param [in] root: The list.
 *
 * @return Returns the new list.
 */
void *cl_dll_move(void *root);

/**
 * @name cl_dll_filter
 * @brief Extracts elements from a list according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 *
 * @param [in] root: The list.
 * @param [in] foo: The filter function.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a list containing all extracted elements from the original
 *         list.
 */
void *cl_dll_filter(void *root, int (*foo)(void *, void *), void *data);

/**
 * @name cl_dll_delete
 * @brief Deletes elements from a list according a specific filter function.
 *
 * If the filter function returns a positive value the element will be
 * extracted. If \a foo is not NULL it will be used as a function to release
 * memory of extracted elements.
 *
 * @param [in] root: The list.
 * @param [in] filter: The filter function.
 * @param [in] data: Some custom data passed to the filter function.
 * @param [in] foo: Optional free function.
 *
 * @return Returns the extracted elements from the list or NULL if \a foo is
 *         not NULL.
 */
void *cl_dll_delete(void *root, int (*filter)(void *, void *), void *data,
                    void (*foo)(void *));

/**
 * @name cl_dll_delete_indexed
 * @brief Delete an element from a list at a specific position.
 *
 * If \a foo is not NULL it will be used as a function to release memory of the
 * extracted element.
 *
 * @param [in] root: The list
 * @param [in] index: The element position on the list.
 * @param [in] foo: Optional free function.
 *
 * @return Return the extracted element from the list or NULL if \a foo is not
 *         NULL.
 */
void *cl_dll_delete_indexed(void *root, unsigned int index, void (*foo)(void *));

/**
 * @name cl_dll_mergesort
 * @brief Sort all elements from a list using mergesort algorithm.
 *
 * @param [in] root: The list.
 * @param [in] cmp: The function used to compare two elements from the list.
 *
 * @return Returns the sorted list.
 */
void *cl_dll_mergesort(void *root, int (*cmp)(void *, void *));

/**
 * @name cl_dll_contains
 * @brief Checks if a list contains a specific element.
 *
 * @param [in] root: The list.
 * @param [in] p: The element which will be sought.
 * @param [in] foo: A function used to compare elements from the list. \a p will
 *                  be passed as the second argument.
 *
 * @return Returns true if the element is found or false otherwise.
 */
bool cl_dll_contains(void *root, void *p, int (*foo)(void *, void *));

/**
 * @name cl_dll_indexof
 * @brief Gets the index of the first occurrence of an element inside a list.
 *
 * @param [in] root: The list.
 * @param [in] n: The element which will be sought.
 * @param [in] foo: A function used to compare elements from the list. \a n will
 *                  be passed as the second argument.
 *
 * @return Returns the index of the element or -1 if it is not found.
 */
int cl_dll_indexof(void *root, void *n, int (*foo)(void *, void *));

/**
 * @name cl_dll_last_indexof
 * @brief Gets the index of the last occurrence of an element inside a list.
 *
 * @param [in] root: The list.
 * @param [in] n: The element which will be sought.
 * @param [in] foo: A function used to compare elements from the list. \a n will
 *                  be passed as the second argument.
 *
 * @return Returns the index of the element or -1 if it is not found.
 */
int cl_dll_last_indexof(void *root, void *n, int (*foo)(void *, void *));

/**
 * @name cl_dll_peek
 * @brief Retrieves, but does not remove, the head of the list.
 *
 * @param [in] list: The list.
 *
 * @return Returns NULL if the list is empty or the head of it otherwise.
 */
void *cl_dll_peek(void *root);

#endif

