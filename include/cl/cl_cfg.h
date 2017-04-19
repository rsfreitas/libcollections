
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

#ifndef _COLLECTIONS_CL_CFG_H
#define _COLLECTIONS_CL_CFG_H	    1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_cfg.h> directly; include <collections.h> instead."
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
 * @brief Sets a value to a specific key - section.
 *
 * Sets the value of a item, inside a section, to the value passed as
 * an argument.
 *
 * If the section is not found, a new one will be created. The same rule
 * applies in case of the key.
 *
 * @param [in,out] file: The cl_cfg_file_t object previously loaded.
 * @param [in] section: The section name.
 * @param [in] key: The key name.
 * @param [in] fmt: The format of the new value.
 * @param [in] ...: The "real" value.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_cfg_set_value(cl_cfg_file_t *file, const char *section, const char *key,
                     const char *fmt, ...) __attribute__((format(printf, 4, 5)));

/**
 * @name cl_cfg_get_section
 * @brief Search and get a pointer to a specific section from a cl_cfg_file_t
 *        object.
 *
 * @param [in] file: cl_cfg_file_t object previously loaded.
 * @param [in] section: The name of the section which will be sought.
 *
 * @return On success returns a cl_cfg_section_t object containing the section and
 *         its respective keys. Returns NULL otherwise.
 */
cl_cfg_section_t *cl_cfg_get_section(const cl_cfg_file_t *file,
                                     const char *section);

/**
 * @name cl_cfg_get_key
 * @brief Search and get a pointer to a specific key from a cl_cfg_file_t object.
 *
 * @param [in] file: cfg_gile_t object.
 * @param [in] section: The name of the section.
 * @param [in] key: The name of the key wich will be sought.
 *
 * @return On success returns a cl_cfg_key_t object containing the key and its
 *         respective value. Returns NULL otherwise.
 */
cl_cfg_key_t *cl_cfg_get_key(const cl_cfg_file_t *file, const char *section,
                             const char *key);

/**
 * @name cl_cfg_get_key_from_section
 * @brief Search and get a pointer to a specific key from a cl_cfg_section_t
 *        object.
 *
 * @param [in] section: cl_cfg_section_t object.
 * @param [in] key: The name of the key which will be sought.
 *
 * @return On success returns a cl_cfg_key_t object containing the key and its
 *         respective value. Returns NULL otherwise.
 */
cl_cfg_key_t *cl_cfg_get_key_from_section(const cl_cfg_section_t *section,
                                          const char *key);

/**
 * @name cl_cfg_section_name
 * @brief Gets the section name from a cl_cfg_section_t object.
 *
 * @param [in] section: cl_cfg_section_t object.
 *
 * @return On success returns a cl_string_t object containing the section
 *         name. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_section_name(const cl_cfg_section_t *section);

/**
 * @name cl_cfg_key_name
 * @brief Gets the key name from a cl_cfg_key_t object.
 *
 * @param [in] key: cl_cfg_key_t object.
 *
 * @return On success returns a cl_string_t object containing the key
 *         name. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_key_name(const cl_cfg_key_t *key);

/**
 * @name cl_cfg_key_value
 * @brief Gets the actual value from a cl_cfg_key_t object.
 *
 * @param [in] key: cl_cfg_key_t object.
 *
 * @return On success returns a reference to a cl_object_t object containing the
 *         key value or NULL otherwise.
 */
cl_object_t *cl_cfg_key_value(const cl_cfg_key_t *key);

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
 * @name cl_cfg_get_key_names
 * @brief Get all key names from a section.
 *
 * @param [in] file: The cl_cfg_file_t object.
 * @param [in] section: The section name.
 *
 * @return On success returns a cl_string_list_t object with all key names or
 *         NULL otherwise.
 */
cl_string_list_t *cl_cfg_get_key_names(const cl_cfg_file_t *file,
                                       const char *section);

/**
 * @name cl_cfg_get_key_names_from_section
 * @brief Get all key names from a section.
 *
 * @param [in] section: The cl_cfg_section_t object.
 *
 * @return On success returns a cl_string_list_t object with all key names or
 *         NULL otherwise.
 */
cl_string_list_t *cl_cfg_get_key_names_from_section(const cl_cfg_section_t *section);

/**
 * @name cl_cfg_get_sections
 * @brief Get all section names from a configuration file.
 *
 * @param [in] file: The cl_cfg_file_t object.
 *
 * @return On success returns a cl_string_list_t object with all section names
 *         or NULL otherwise.
 */
cl_string_list_t *cl_cfg_get_sections(const cl_cfg_file_t *file);

/**
 * @name cl_cfg_key_comment
 * @brief Gets the comment value from a cl_cfg_key_t object.
 *
 * @param [in] key: cl_cfg_key_t object.
 *
 * @return On success returns a cl_string_t object containing the key
 *         comment. Returns NULL otherwise.
 */
cl_string_t *cl_cfg_key_comment(const cl_cfg_key_t *key);

/**
 * @name cl_cfg_key_ref
 * @brief Increases the reference count for a cl_cfg_key_t item.
 *
 * @param [in,out] key: The key item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_key_t *cl_cfg_key_ref(cl_cfg_key_t *key);

/**
 * @name cl_cfg_key_unref
 * @brief Decreases the reference count for a cl_cfg_key_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] key: The key item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_key_unref(cl_cfg_key_t *key);

/**
 * @name cl_cfg_section_ref
 * @brief Increases the reference count for a cl_cfg_section_t item.
 *
 * @param [in,out] section: The section item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_section_t *cl_cfg_section_ref(cl_cfg_section_t *section);

/**
 * @name cl_cfg_section_unref
 * @brief Decreases the reference count for a cl_cfg_section_titem.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] section: The section item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_section_unref(cl_cfg_section_t *section);

/**
 * @name cl_cfg_file_ref
 * @brief Increases the reference count for a cl_cfg_file_t item.
 *
 * @param [in,out] file: The file item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_cfg_file_t *cl_cfg_file_ref(cl_cfg_file_t *file);

/**
 * @name cl_cfg_file_unref
 * @brief Decreases the reference count for a cl_cfg_file_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] file: The file item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_cfg_file_unref(cl_cfg_file_t *file);

#endif

