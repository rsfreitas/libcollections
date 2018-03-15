
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Sep  7 10:13:24 2016
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

#ifndef _COLLECTIONS_API_INIT_H
#define _COLLECTIONS_API_INIT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <init.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_init
 * @brief The function to initialize all library internals.
 *
 * This function must be the first library function called inside a code.
 * Otherwise all other functions won't work.
 *
 * \a arg may be a file name pointing to a file with JSON configuration supported
 * by the library or a JSON string with the configuration. If it is NULL, some
 * default values will be used.
 *
 * The supported JSON must look like this:
 *
 * {
 *      "package": string,      // The application name.
 *      "locale_dir": string    // The root directory of translation files.
 * }
 *
 * @param [in] arg: The library configuration or a file name with the
 *                  configuration.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_init(const char *arg);

/**
 * @name cl_uninit
 * @brief The function to end all library internals.
 */
void cl_uninit(void);

#endif

