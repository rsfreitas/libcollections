
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

#ifndef _COLLECTIONS_API_CFG_H
#define _COLLECTIONS_API_CFG_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cfg.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_cfg_load
 * @brief Loads a INI configuration file to a cl_cfg_file_t object.
 *
 * @param [in] filename: File name that will be loaded.
 *
 * @return On success returns a cl_cfg_file_t object or NULL otherwise.
 */
cl_cfg_file_t *cl_cfg_load(const char *filename);

/**
 * @name cl_cfg_create
 * @brief Creates a INI structure to hold configurations.
 *
 * @return On success returns a cl_cfg_file_t object or NULL otherwise.
 */
cl_cfg_file_t *cl_cfg_create(void);

/**
 * @name cl_cfg_unload
 * @brief Frees all memory previously allocated on a cl_cfg_file_t object.
 *
 * @param [in,out] file: cl_cfg_file_t object that will be freed.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_cfg_unload(cl_cfg_file_t *file);

/**
 * @name cl_cfg_sync
 * @brief Write the contents of a cl_cfg_file_t object to a file.
 *
 * All file content will be overwritten.
 *
 * @param [in] file: cl_cfg_file_t object that will be written to a file.
 * @param [in] filename: Optional output file name. If set to NULL the name will
 *                       be the same as that was previously loaded.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_cfg_sync(const cl_cfg_file_t *file, const char *filename);

/**
 * @name cl_cfg_set_value
 * @brief Sets a value to a specific entry - block.
 *
 * Sets the value of a item, inside a block, to the value passed as
 * an argument.
 *
 * If the block is not found, a new one will be created. The same rule
 * applies in case of the entry.
 *
 * @param [in,out] file: The cl_cfg_file_t object previously loaded.
 * @param [in] block: The block name.
 * @param [in] entry: The entry name.
 * @param [in] fmt: The format of the new value.
 * @param [in] ...: The "real" value.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_cfg_set_value(cl_cfg_file_t *file, const char *block, const char *entry,
                     const char *fmt, ...) __attribute__((format(printf, 4, 5)));

/**
 * @name cl_cfg_block
 * @brief Search and get a pointer to a specific block from a cl_cfg_file_t
 *        object.
 *
 * @param [in] file: cl_cfg_file_t object previously loaded.
 * @param [in] block: The name of the block which will be sought.
 *
 * @return On success returns a cl_cfg_block_t object containing the block and
 *         its respective keys. Returns NULL otherwise.
 */
cl_cfg_block_t *cl_cfg_block(const cl_cfg_file_t *file, const char *block);

/**
 * @name cl_cfg_entry
 * @brief Search and get a pointer to a specific entry from a cl_cfg_file_t object.
 *
 * @param [in] file: cfg_gile_t object.
 * @param [in] block: The name of the block.
 * @param [in] entry: The name of the entry wich will be sought.
 *
 * @return On success returns a cl_cfg_entry_t object containing the entry and its
 *         respective value. Returns NULL otherwise.
 */
cl_cfg_entry_t *cl_cfg_entry(const cl_cfg_file_t *file, const char *block,
                             const char *entry);

/**
 * @name cl_cfg_block_entry
 * @brief Search and get a pointer to a specific entry from a cl_cfg_block_t
 *        object.
 *
 * @param [in] block: cl_cfg_block_t object.
 * @param [in] entry: The name of the entry which will be sought.
 *
 * @return On success returns a cl_cfg_entry_t object containing the entry and its
 *         respective value. Returns NULL otherwise.
 */
cl_cfg_entry_t *cl_cfg_block_entry(const cl_cfg_block_t *block,
                                   const char *entry);

/**
 * @name cl_cfg_block_name
 * @brief Gets the block name from a cl_cfg_block_t object.
 *
 * @param [in] block: cl_cfg_block_t object.
 *
 * @return On success returns a cl_string_t object containing the block
 *         name. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_block_name(const cl_cfg_block_t *block);

/**
 * @name cl_cfg_entry_name
 * @brief Gets the entry name from a cl_cfg_entry_t object.
 *
 * @param [in] entry: cl_cfg_entry_t object.
 *
 * @return On success returns a cl_string_t object containing the entry
 *         name. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_entry_name(const cl_cfg_entry_t *entry);

/**
 * @name cl_cfg_entry_value
 * @brief Gets the actual value from a cl_cfg_entry_t object.
 *
 * @param [in] entry: cl_cfg_entry_t object.
 *
 * @return On success returns a reference to a cl_object_t object containing the
 *         entry value or NULL otherwise.
 */
cl_object_t *cl_cfg_entry_value(const cl_cfg_entry_t *entry);

/**
 * @name cl_cfg_to_cstring
 * @brief Converts a cl_cfg_file_t object into a human readable string.
 *
 * @param [in] file: The cl_cfg_file_t object.
 *
 * @return On success returns a cl_string_t containing the cl_cfg_file_t object.
 */
cl_string_t *cl_cfg_to_cstring(const cl_cfg_file_t *file);

/**
 * @name cl_cfg_all_entry_names
 * @brief Get all entry names from a block.
 *
 * @param [in] file: The cl_cfg_file_t object.
 * @param [in] block: The block name.
 *
 * @return On success returns a cl_stringlist_t object with all entry names or
 *         NULL otherwise.
 */
cl_stringlist_t *cl_cfg_all_entry_names(const cl_cfg_file_t *file,
                                        const char *block);

/**
 * @name cl_cfg_block_entry_names
 * @brief Get all entry names from a block.
 *
 * @param [in] block: The cl_cfg_block_t object.
 *
 * @return On success returns a cl_stringlist_t object with all entry names or
 *         NULL otherwise.
 */
cl_stringlist_t *cl_cfg_block_entry_names(const cl_cfg_block_t *block);

/**
 * @name cl_cfg_all_block_names
 * @brief Get all block names from a configuration file.
 *
 * @param [in] file: The cl_cfg_file_t object.
 *
 * @return On success returns a cl_stringlist_t object with all block names
 *         or NULL otherwise.
 */
cl_stringlist_t *cl_cfg_all_block_names(const cl_cfg_file_t *file);

/**
 * @name cl_cfg_entry_comment
 * @brief Gets the comment value from a cl_cfg_entry_t object.
 *
 * @param [in] entry: cl_cfg_entry_t object.
 *
 * @return On success returns a cl_string_t object containing the entry
 *         comment. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_entry_comment(const cl_cfg_entry_t *entry);

/**
 * @name cl_cfg_entry_ref
 * @brief Increases the reference count for a cl_cfg_entry_t item.
 *
 * @param [in,out] entry: The entry item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_entry_t *cl_cfg_entry_ref(cl_cfg_entry_t *entry);

/**
 * @name cl_cfg_entry_unref
 * @brief Decreases the reference count for a cl_cfg_entry_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] entry: The entry item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_entry_unref(cl_cfg_entry_t *entry);

/**
 * @name cl_cfg_block_ref
 * @brief Increases the reference count for a cl_cfg_block_t item.
 *
 * @param [in,out] block: The block item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_block_t *cl_cfg_block_ref(cl_cfg_block_t *block);

/**
 * @name cl_cfg_block_unref
 * @brief Decreases the reference count for a cl_cfg_block_titem.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] block: The block item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_block_unref(cl_cfg_block_t *block);

/**
 * @name cl_cfg_ref
 * @brief Increases the reference count for a cl_cfg_file_t item.
 *
 * @param [in,out] file: The file item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_file_t *cl_cfg_ref(cl_cfg_file_t *file);

/**
 * @name cl_cfg_unref
 * @brief Decreases the reference count for a cl_cfg_file_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] file: The file item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_unref(cl_cfg_file_t *file);

/**
 * @name cl_cfg_set_value_comment
 * @brief Sets/Updates an entry comment to be saved.
 *
 * The entry must exist to do this.
 *
 * @param [in,out] file: The file item.
 * @param [in] block: The block name.
 * @param [in] entry: The entry name.
 * @param [in] fmt: The comment format.
 * @param [in] ...: The comment values.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_set_value_comment(cl_cfg_file_t *file, const char *block,
                             const char *entry, const char *fmt, ...);

/**
 * @name cl_cfg_get_value
 * @brief Gets the value from an entry inside a configuration file.
 *
 * @param [in] file: The file object.
 * @param [in] block: The block name.
 * @param [in] entry: The entry name.
 *
 * @return On success returns a new reference to the current entry value or
 *         NULL otherwise.
 */
cl_object_t *cl_cfg_get_value(const cl_cfg_file_t *file,
                              const char *block, const char *entry);

#endif

