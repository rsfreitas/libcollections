
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _COLLECTIONS_H
#define _COLLECTIONS_H          1

#ifdef LIBCOLLECTIONS_COMPILE
# define MAJOR_VERSION          0
# define MINOR_VERSION          1
# define BUILD                  4
#endif

#include <cl/cl_types.h>
#include <cl/cl_cfg.h>
#include <cl/cl_chat.h>
#include <cl/cl_datetime.h>
#include <cl/cl_dll.h>
#include <cl/cl_error.h>
#include <cl/cl_event.h>
#include <cl/cl_file.h>
#include <cl/cl_io.h>
#include <cl/cl_json.h>
#include <cl/cl_mem.h>
#include <cl/cl_process.h>
#include <cl/cl_random.h>
#include <cl/cl_specs.h>
#include <cl/cl_string.h>
#include <cl/cl_stringlist.h>
#include <cl/cl_thread.h>
#include <cl/cl_timer.h>
#include <cl/cl_value.h>

#ifdef LIBCOLLECTIONS_COMPILE
# include <cl/cl_internal.h>
#endif

#endif

