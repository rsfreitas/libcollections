
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Feb 21 09:38:41 2017
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

#ifndef _COLLECTIONS_API_CQUEUE_H
#define _COLLECTIONS_API_CQUEUE_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cqueue.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_cqueue_ref
 * @brief Increases the reference count for a cl_cqueue_t item.
 *
 * @param [in,out] cqueue: The circular queue item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cqueue_t *cl_cqueue_ref(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_unref
 * @brief Decreases the reference count for a cl_cqueue_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] cqueue: The circular queue item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_unref(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_create
 * @brief Creates a new circular queue object.
 *
 * This function creates a new circular queue to manipulate all kind of data
 * que size \a size.
 *
 * The function receives a few function pointers to be used by its API,
 * \a free_data is a function used to release the content of a circular queue
 * node, \a compare_to is a function to be used while sorting the queue, it must
 * return a negative value, zero or a positive value if the nodes are less then,
 * equal to or, greater then one another, \a filter is a function used to extract
 * elements from the queue if it returns a positive value and \a equals is also a
 * function to compare two nodes except that it must return a positive value
 * when the nodes are equal or zero otherwise.
 *
 * Their prototypes are the following:
 *
 * void free_data(void *);
 * int compare_to(cl_queue_node_t *, cl_queue_node_t *);
 * int filter(cl_queue_node_t *, void *);
 * int equals(cl_queue_node_t *, cl_queue_node_t *);
 *
 * If the type of a node content if of a cobject_t kind it is not necessary to
 * pass the arguments \a free_data, \a compare_to and \a equals, since them can
 * exist inside a cobject_t object.
 *
 * @param [in] size: The maximum circular queue size.
 * @param [in] free_data: The free_data function pointer.
 * @param [in] compare_to: The compare_to function pointer.
 * @param [in] filter: The filter function pointer.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success a void object will be returned or NULL otherwise.
 */
cl_cqueue_t *cl_cqueue_create(unsigned int size, void (*unref_node)(void *),
                              int (*compare_to)(cl_queue_node_t *,
                                                cl_queue_node_t *),
                              int (*filter)(cl_queue_node_t *, void *),
                              int (*equals)(cl_queue_node_t *,
                                            cl_queue_node_t *));

/**
 * @name cl_cqueue_destroy
 * @brief Releases a void from memory.
 *
 * When releasing a node from the queue, the \a free_data function passed while
 * creating the queue is called.
 *
 * @param [in,out] cqueue:  The circular queue object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_destroy(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_size
 * @brief Gets the circular queue size.
 *
 * @param [in] cqueue:  The circular queue object.
 *
 * @return On success returns the size of the queue or -1 otherwise.
 */
int cl_cqueue_size(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_enqueue
 * @brief Inserts an element into the circular queue.
 *
 * Whe the circular queue reaches its limit, the older element will be removed
 * to insert a new one.
 *
 * @param [in,out] cqueue:  The circular queue object.
 * @param [in] node_content: The content of the new node.
 * @param [in] size: The size in bytes of the content.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_enqueue(cl_cqueue_t *cqueue, const void *data,
                      unsigned int data_size);

/**
 * @name cl_cqueue_dequeue
 * @brief Retrieves and removes the head of the circular queue.
 *
 * @param [in,out] cqueue:  The circular queue object.
 *
 * @return On success returns the node shifted off the queue, and the user is
 *         responsible for releasing it, or NULL otherwise.
 */
cl_queue_node_t *cl_cqueue_dequeue(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_map
 * @brief Maps a function to every node on a circular queue.
 *
 * The \a foo function receives as arguments a node from the queue and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_queue_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_queue_node_t *cl_cqueue_map(cl_cqueue_t *cqueue,
                               int (*foo)(cl_queue_node_t *, void *),
                               void *data);

/**
 * @name cl_cqueue_map_indexed
 * @brief Maps a function to every node on a circular queue.
 *
 * The \a foo function receives as arguments the current node index inside the
 * queue, a node from the queue and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_queue_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_queue_node_t *cl_cqueue_map_indexed(cl_cqueue_t *cqueue,
                                       int (*foo)(unsigned int,
                                                  cl_queue_node_t *, void *),
                                       void *data);

/**
 * @name cl_cqueue_map_reverse
 * @brief Maps a functions to every onde on a circular queue from the end to
 *        the top.
 *
 * The \a foo function receives as arguments a node from the queue and some
 * \a data. Its prototype must be something of this type:
 * int foo(cl_queue_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_queue_node_t *cl_cqueue_map_reverse(cl_cqueue_t *cqueue,
                                       int (*foo)(cl_queue_node_t *, void *),
                                       void *data);

/**
 * @name cl_cqueue_map_reverse_indexed
 * @brief Maps a function to every node on a circular queue from the end to the
 *        top.
 *
 * The \a foo function receives as arguments the current node index inside the
 * queue, a node from the queue and some custom \a data. Its prototype must be
 * something of this kind: int foo(unsigned int, cl_queue_node_t *, void *);
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] foo: The function.
 * @param [in] data: The custom data passed to the map function.
 *
 * @return If \a foo returns a non-zero returns a new reference to the current
 *         node. If not returns NULL.
 */
cl_queue_node_t *cl_cqueue_map_reverse_indexed(cl_cqueue_t *cqueue,
                                               int (*foo)(unsigned int,
                                                          cl_queue_node_t *,
                                                          void *),
                                               void *data);

/**
 * @name cl_cqueue_at
 * @brief Gets a pointer to a specific node inside a circular queue.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] index: The node index inside the queue.
 *
 * @return On success returns a reference to the node or NULL otherwise.
 */
cl_queue_node_t *cl_cqueue_at(cl_cqueue_t *cqueue, unsigned int index);

/**
 * @name cl_cqueue_delete
 * @brief Deletes elements from a circular queue according a specific filter
 *        function.
 *
 * If the filter function returns a positive value the element will be extracted
 * from the queue and released from memory. This function uses the \a filter
 * function inside.
 *
 * @param [in,out] cqueue:  The circular queue object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_delete(cl_cqueue_t *cqueue, void *data);

/**
 * @name cl_cqueue_delete_indexed
 * @brief Deletes an element from a circular queue at a specific position.
 *
 * @param [in,out] cqueue:  The circular queue object.
 * @param [in] index: The element position on the queue.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_delete_indexed(cl_cqueue_t *cqueue, unsigned int index);

/**
 * @name cl_cqueue_move
 * @brief Moves all elements from a circular queue to another.
 *
 * @param [in] cqueue: The original void object.
 *
 * @return Returns the new circular queue.
 */
cl_cqueue_t *cl_cqueue_move(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_filter
 * @brief Extracts elements from a circular queue according a specific filter.
 *
 * If the filter function returns a positive value the element will be extracted.
 * This function uses the \a filter function to apply the filter on all elements
 * from the queue.
 *
 * @param [in,out] cqueue:  The circular queue object.
 * @param [in] data: Some custom data passed to the filter function.
 *
 * @return Returns a circular queue containing all extracted elements from the
 *         original queue.
 */
cl_cqueue_t *cl_cqueue_filter(cl_cqueue_t *cqueue, void *data);

/**
 * @name cl_cqueue_sort
 * @brief Sort all elements from a circular queue.
 *
 * This function uses the \a compare_to function to compare two elements from
 * the queue and sort them.
 *
 * @param [in,out] cqueue:  The circular queue object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_sort(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_indexof
 * @brief Gets the index of the first occurrence of an element inside the
 *        circular queue.
 *
 * This function uses the \a equals function to compare objects from the queue.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] element: The element which will be sought through the queue.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_cqueue_indexof(cl_cqueue_t *cqueue, void *element,
                      unsigned int size);

/**
 * @name cl_cqueue_last_indexof
 * @brief Gets the index of the last occurrence of an element inside the
 *        circular queue.
 *
 * This function uses the \a equals function to compare objects from the queue.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] element: The element which will be sought through the queue.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns the element index or -1 if it is not found.
 */
int cl_cqueue_last_indexof(cl_cqueue_t *cqueue, void *element,
                           unsigned int size);

/**
 * @name cl_cqueue_contains
 * @brief Checks if a circular queue contains a specific element.
 *
 * This function uses the \a equals function to compare objects from the queue.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] element: The element which will be sought through the queue.
 * @param [in] size: The size in bytes of the element.
 *
 * @return Returns true if the element is found or false otherwise.
 */
bool cl_cqueue_contains(cl_cqueue_t *cqueue, void *element,
                        unsigned int size);

/**
 * @name cl_cqueue_front
 * @brief Retrieves, but does not remove, the head of the circular queue.
 *
 * On a successful call the node reference must be 'unreferenced'.
 *
 * @param [in] cqueue:  The circular queue object.
 *
 * @return Returns NULL if the queue is empty or a new reference to the head
 *         of it.
 */
cl_queue_node_t *cl_cqueue_front(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_is_empty
 * @brief Tests to see if the circular queue is empty or not.
 *
 * @param [in] cqueue:  The circular queue object.
 *
 * @return Returns true if the queue is empty or false otherwise.
 */
bool cl_cqueue_is_empty(cl_cqueue_t *cqueue);

/**
 * @name cl_cqueue_set_compare_to
 * @brief Updates the internal object compare function.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] compare_to: The compare function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_set_compare_to(cl_cqueue_t *cqueue,
                             int (*compare_to)(cl_queue_node_t *,
                                               cl_queue_node_t *));

/**
 * @name cl_cqueue_set_filter
 * @brief Updates the internal filter function.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] filter: The filter function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_set_filter(cl_cqueue_t *cqueue, int (*filter)(cl_queue_node_t *,
                                                            void *));

/**
 * @name cl_cqueue_set_equals
 * @brief Updates the internal equals function.
 *
 * @param [in] cqueue:  The circular queue object.
 * @param [in] equals: The equals function pointer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cqueue_set_equals(cl_cqueue_t *cqueue, int (*equals)(cl_queue_node_t *,
                                                            cl_queue_node_t *));

#endif

