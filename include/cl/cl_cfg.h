
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
 * @name cfg_load
 * @brief Loads a INI configuration file to a cfg_file_t object.
 *
 * @param [in] filename: File name that will be loaded.
 *
 * @return On success returns a cfg_file_t object or NULL otherwise.
 */
cfg_file_t *cfg_load(const char *filename);

/**
 * @name cfg_unload
 * @brief Frees all memory previously allocated on a cfg_file_t object.
 *
 * @param [in,out] file: cfg_file_t object that will be freed.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cfg_unload(cfg_file_t *file);

/**
 * @name cfg_sync
 * @brief Write the contents of a cfg_file_t object to a file.
 *
 * All file content will be overwritten.
 *
 * @param [in] file: cfg_file_t object that will be written to a file.
 * @param [in] filename: Optional output file name. If set to NULL the name will
 *                       be the same as that was previously loaded.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cfg_sync(const cfg_file_t *file, const char *filename);

/**
 * @name cfg_set_value
 * @brief Sets a value to a specific key - section.
 *
 * Sets the value of a item, inside a section, to the value passed as
 * an argument.
 *
 * If the section is not found, a new one will be created. The same rule
 * applies in case of the key.
 *
 * @param [in,out] file: The cfg_file_t object previously loaded.
 * @param [in] section: The section name.
 * @param [in] key: The key name.
 * @param [in] fmt: The format of the new value.
 * @param [in] ...: The "real" value.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cfg_set_value(cfg_file_t *file, const char *section, const char *key,
                  const char *fmt, ...) __attribute__((format(printf, 4, 5)));

/**
 * @name cfg_get_section
 * @brief Search and get a pointer to a specific section from a cfg_file_t object.
 *
 * @param [in] file: cfg_file_t object previously loaded.
 * @param [in] section: The name of the section which will be sought.
 *
 * @return On success returns a cfg_section_t object containing the section and
 *         its respective keys. Returns NULL otherwise.
 */
cfg_section_t *cfg_get_section(const cfg_file_t *file, const char *section);

/**
 * @name cfg_get_key
 * @brief Search and get a pointer to a specific key from a cfg_file_t object.
 *
 * @param [in] file: cfg_gile_t object.
 * @param [in] section: The name of the section.
 * @param [in] key: The name of the key wich will be sought.
 *
 * @return On success returns a cfg_key_t object containing the key and its
 *         respective value. Returns NULL otherwise.
 */
cfg_key_t *cfg_get_key(const cfg_file_t *file, const char *section,
                       const char *key);

/**
 * @name cfg_get_key_from_section
 * @brief Search and get a pointer to a specific key from a cfg_section_t object.
 *
 * @param [in] section: cfg_section_t object.
 * @param [in] key: The name of the key which will be sought.
 *
 * @return On success returns a cfg_key_t object containing the key and its
 *         respective value. Returns NULL otherwise.
 */
cfg_key_t *cfg_get_key_from_section(const cfg_section_t *section,
                                    const char *key);

/**
 * @name cfg_section_name
 * @brief Gets the section name from a cfg_section_t object.
 *
 * @param [in] section: cfg_section_t object.
 *
 * @return On success returns a cstring_t object containing the section
 *         name. Returns NULL otherwise.
 */
cstring_t *cfg_section_name(const cfg_section_t *section);

/**
 * @name cfg_key_name
 * @brief Gets the key name from a cfg_key_t object.
 *
 * @param [in] key: cfg_key_t object.
 *
 * @return On success returns a cstring_t object containing the key
 *         name. Returns NULL otherwise.
 */
cstring_t *cfg_key_name(const cfg_key_t *key);

/**
 * @name cfg_key_value
 * @brief Gets the actual value from a cfg_key_t object.
 *
 * @param [in] key: cfg_key_t object.
 *
 * @return On success returns a reference to a cvalue_t object containing the key
 *         value or NULL otherwise.
 */
cvalue_t *cfg_key_value(const cfg_key_t *key);

/**
 * @name cfg_to_string
 * @brief Converts a cfg_file_t object into a human readable string.
 *
 * @param [in] file: The cfg_file_t object.
 *
 * @return On success returns a cstring_t containing the cfg_file_t object.
 */
cstring_t *cfg_to_string(const cfg_file_t *file);

#endif

