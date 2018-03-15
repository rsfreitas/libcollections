
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Feb  6 10:28:39 2017
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

#ifndef _COLLECTIONS_API_DIR_H
#define _COLLECTIONS_API_DIR_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <dir.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_mkdir
 * @brief Creates a directory.
 *
 * The function creates the \a dname directory. It also creates any needed
 * subdirectory.
 *
 * @param [in] dname: Directory name.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_mkdir(const char *dname);

#endif

