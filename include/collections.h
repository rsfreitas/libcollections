
/*
 * Description:
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

#ifndef _COLLECTIONS_H
#define _COLLECTIONS_H          1

#ifdef LIBCOLLECTIONS_COMPILE
# define MAJOR_VERSION          0
# define MINOR_VERSION          0
# define RELEASE                46
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "api/types.h"
#include "api/cfg.h"
#include "api/chat.h"
#include "api/counter.h"
#include "api/cqueue.h"
#include "api/cstack.h"
#include "api/datetime.h"
#include "api/dir.h"
#include "api/error.h"
#include "api/event.h"
#include "api/file.h"
#include "api/hashtable.h"

#ifdef CL_USE_IMAGEAPI
# include "api/image.h"
#endif

#include "api/init.h"
#include "api/intl.h"
#include "api/io.h"
#include "api/json.h"
#include "api/list.h"
#include "api/log.h"
#include "api/mem.h"
#include "api/object.h"
#include "api/queue.h"
#include "api/plugin_macros.h"
#include "api/plugin.h"
#include "api/process.h"
#include "api/random.h"
#include "api/ref.h"
#include "api/specs.h"
#include "api/stack.h"
#include "api/string.h"
#include "api/stringlist.h"
#include "api/thread.h"
#include "api/timeout.h"
#include "api/timer.h"
#include "api/utils.h"

#ifdef LIBCOLLECTIONS_COMPILE
# include "internal/internal.h"
#endif

#ifdef __cplusplus
}
#endif

#endif

