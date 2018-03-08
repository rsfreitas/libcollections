
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Mar  8 08:06:47 2018
 * Project: libcollections
 *
 * Copyright (C) 2018 Rodrigo Freitas All rights reserved.
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

#ifndef _COLLECTIONS_INTERNAL_INTL_H
#define _COLLECTIONS_INTERNAL_INTL_H

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <intl.h> directly; include <collections.h> instead."
# endif
#endif

int intl_start(const char *package, const char *locale_dir);

#endif
