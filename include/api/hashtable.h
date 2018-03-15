
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Feb 20 10:12:11 2017
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

#ifndef _COLLECTIONS_API_HASHTABLE_H
#define _COLLECTIONS_API_HASHTABLE_H          1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <hashtable.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_hashtable_ref
 * @brief Increases the reference count of a cl_hashtable_t object.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_hashtable_t *cl_hashtable_ref(cl_hashtable_t *hashtable);

/**
 * @name cl_hashtable_unref
 * @brief Decreases the reference count for a cl_hashtable_t object.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_hashtable_unref(cl_hashtable_t *hashtable);

/**
 * @name cl_hashtable_init
 * @brief Creates a hash table to store any kind of data.
 *
 * @param [in] size: The hash table size.
 * @param [in] replace_data: A boolean flag to indicate if a collision of key
 *                           happens the data will be overwritten or not.
 * @param [in] compare: A function to compare the item stored inside the hash
 *                      table. It must return true if the two arguments are
 *                      equals or false otherwise.
 * @param [in] release: An optional function to be called over every valid table
 *                      index when releasing it.
 *
 * @return On success returns a cl_hashtable_t object or NULL otherwise.
 */
cl_hashtable_t *cl_hashtable_init(unsigned int size, bool replace_data,
                                  bool (*compare)(void *, void *),
                                  void (*release)(void *));

/**
 * @name cl_hashtable_uninit
 * @brief Releases a hash table from memory.
 *
 * @param [in] hashtable: The cl_hashtable_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_hashtable_uninit(cl_hashtable_t *hashtable);

/**
 * @name cl_hashtable_put
 * @brief Maps the specified key to the specified value in the hashtable.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] key: The hash table key.
 * @param [in] data: The value.
 *
 * @return On success, returns the previous value of the specified key in this
 *         hash table, if replace_data was true when creating the hash table, or
 *         NULL otherwise.
 */
void *cl_hashtable_put(cl_hashtable_t *hashtable, const char *key, void *data);

/**
 * @name cl_hashtable_get
 * @brief Gets the value to which the specified key is mapped.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] key: The key whose associated value is to be returned.
 *
 * @return On success returns the value or NULL otherwise.
 */
void *cl_hashtable_get(cl_hashtable_t *hashtable, const char *key);

/**
 * @name cl_hashtable_delete
 * @brief
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] key: The key that needs to be removed.
 *
 * @return
 */
int cl_hashtable_delete(cl_hashtable_t *hashtable, const char *key);

/**
 * @name cl_hashtable_contains_key
 * @brief Tests if the specified object is a key in this hash table.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] key: Possible key.
 *
 * @return Returns true if and only if the specified object is a key in this
 *         hash table or false otherwise.
 */
bool cl_hashtable_contains_key(cl_hashtable_t *hashtable, const char *key);

/**
 * @name cl_hashtable_size
 * @brief Gets the number of values in this hash table.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 *
 * @return On success returns the number of values or -1 otherwise.
 */
int cl_hashtable_size(cl_hashtable_t *hashtable);

/**
 * @name cl_hashtable_clear
 * @brief Clear this hash table so that it contains no keys.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_hashtable_clear(cl_hashtable_t *hashtable);

/**
 * @name cl_hashtable_contains_value
 * @brief Tests if this hash table maps a key to this value.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] data: Value whose presence in this hash table is to be tested.
 *
 * @return Returns true if this map maps one key to the specified value or
 *         false otherwise.
 */
bool cl_hashtable_contains_value(cl_hashtable_t *hashtable, void *data);

/**
 * @name cl_hashtable_keys
 * @brief Gets the keys in this hash table.
 *
 * @param [in] hashtable: The cl_hashtable_t object.
 * @param [in] dup: A boolean flag to duplicate every key name inside the
 *                  returned list or just a reference to the original one.
 *
 * @return On success returns a cl_list_t object with all keys from this hash
 *         table or NULL otherwise.
 */
cl_list_t *cl_hashtable_keys(cl_hashtable_t *hashtable, bool dup);

#endif

