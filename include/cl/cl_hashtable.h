
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

#ifndef _COLLECTIONS_CL_HASHTABLE_H
#define _COLLECTIONS_CL_HASHTABLE_H          1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_hashtable.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name chashtable_ref
 * @brief Increases the reference count of a chashtable_t object.
 *
 * @param [in] hashtable: The chashtable_t object.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
chashtable_t *chashtable_ref(chashtable_t *hashtable);

/**
 * @name chashtable_unref
 * @brief Decreases the reference count for a chashtable_t object.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in] hashtable: The chashtable_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int chashtable_unref(chashtable_t *hashtable);

/**
 * @name chashtable_init
 * @brief Creates a hash table to store any kind of data.
 *
 * @param [in] size: The hash table size.
 * @param [in] replace_data: A boolean flag to indicate if a collision of key
 *                           happens the data will be overwritten or not.
 * @param [in] compare: A function to compare the item stored inside the hash
 *                      table. It must return true if the two arguments are
 *                      equals or false otherwise.
 *
 * @return On success returns a chashtable_t object or NULL otherwise.
 */
chashtable_t *chashtable_init(unsigned int size, bool replace_data,
                              bool (*compare)(void *, void *));

/**
 * @name chashtable_uninit
 * @brief Releases a hash table from memory.
 *
 * @param [in] hashtable: The chashtable_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int chashtable_uninit(chashtable_t *hashtable);

/**
 * @name chashtable_put
 * @brief Maps the specified key to the specified value in the hashtable.
 *
 * @param [in] hashtable: The chashtable_t object.
 * @param [in] key: The hash table key.
 * @param [in] data: The value.
 *
 * @return On success, returns the previous value of the specified key in this
 *         hash table, if replace_data was true when creating the hash table, or
 *         NULL otherwise.
 */
void *chashtable_put(chashtable_t *hashtable, const char *key, void *data);

/**
 * @name chashtable_get
 * @brief Gets the value to which the specified key is mapped.
 *
 * @param [in] hashtable: The chashtable_t object.
 * @param [in] key: The key whose associated value is to be returned.
 *
 * @return On success returns the value or NULL otherwise.
 */
void *chashtable_get(chashtable_t *hashtable, const char *key);

/**
 * @name chashtable_delete
 * @brief
 *
 * @param [in] hashtable: The chashtable_t object.
 * @param [in] key: The key that needs to be removed.
 *
 * @return
 */
int chashtable_delete(chashtable_t *hashtable, const char *key);

/**
 * @name chashtable_contains_key
 * @brief Tests if the specified object is a key in this hash table.
 *
 * @param [in] hashtable: The chashtable_t object.
 * @param [in] key: Possible key.
 *
 * @return Returns true if and only if the specified object is a key in this
 *         hash table or false otherwise.
 */
bool chashtable_contains_key(chashtable_t *hashtable, const char *key);

/**
 * @name chashtable_size
 * @brief Gets the number of values in this hash table.
 *
 * @param [in] hashtable: The chashtable_t object.
 *
 * @return On success returns the number of values or -1 otherwise.
 */
int chashtable_size(chashtable_t *hashtable);

/**
 * @name chashtable_clear
 * @brief Clear this hash table so that it contains no keys.
 *
 * @param [in] hashtable: The chashtable_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int chashtable_clear(chashtable_t *hashtable);

/**
 * @name chashtable_contains_value
 * @brief Tests if this hash table maps a key to this value.
 *
 * @param [in] hashtable: The chashtable_t object.
 * @param [in] data: Value whose presence in this hash table is to be tested.
 *
 * @return Returns true if this map maps one key to the specified value or
 *         false otherwise.
 */
bool chashtable_contains_value(chashtable_t *hashtable, void *data);

/**
 * @name chashtable_keys
 * @brief Gets the keys in this hash table.
 *
 * @param [in] hashtable: The chashtable_t object.
 *
 * @return On success returns a clist_t object with all keys from this hash
 *         table or NULL otherwise.
 */
clist_t *chashtable_keys(chashtable_t *hashtable);

#endif

