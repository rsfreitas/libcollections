
/*
 * Description: Internal declarations for libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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

#ifndef _COLLECTIONS_INTERNAL_H
#define _COLLECTIONS_INTERNAL_H         1

/*
 * A prefix that MUST be included at the beginning of an internal library
 * function, i.e., one that can be accessed from other source file.
 *
 * Example:
 * CL_INTERNAL_API
 * const char *function(int arg)
 * {
 *      // Body
 * }
 */
#define CL_INTERNAL_API     __attribute__((visibility("hidden")))

/* Internal includes */
#include "typeof.h"
#include "error.h"
#include "value.h"
#include "utils.h"
#include "plugin.h"
#include "init.h"
#include "glist.h"
#include "random.h"
#include "intl.h"
#include "dll.h"

#endif

