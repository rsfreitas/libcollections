
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  2 09:38:13 2016
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

#ifndef _COLLECTIONS_CL_TYPEOF_H
#define _COLLECTIONS_CL_TYPEOF_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_typeof.h> directly; include <collections.h> instead."
# endif
#endif

enum cl_object {
    CL_OBJ_UNKNOWN = -1,
    CL_OBJ_STRING,
    CL_OBJ_STRINGLIST,
    CL_OBJ_CFG_FILE,
    CL_OBJ_CFG_BLOCK,
    CL_OBJ_CFG_ENTRY,
    CL_OBJ_JSON,
    CL_OBJ_DATETIME,
    CL_OBJ_TIMEOUT,
    CL_OBJ_THREAD,
    CL_OBJ_TIMER,
    CL_OBJ_TIMER_INFO,
    CL_OBJ_TIMER_ARG,     /* This is not our type, so we can't validate it */
    CL_OBJ_CHAT,
    CL_OBJ_LIST,
    CL_OBJ_EVENT,
    CL_OBJ_OBJECT,
    CL_OBJ_SPEC,
    CL_OBJ_COUNTER,
    CL_OBJ_PLUGIN,
    CL_OBJ_PLUGIN_ARG,
    CL_OBJ_PLUGIN_INFO,
    CL_OBJ_LOG,
    CL_OBJ_IMAGE,
    CL_OBJ_LIST_NODE,
    CL_OBJ_STACK,
    CL_OBJ_STACK_NODE,
    CL_OBJ_QUEUE,
    CL_OBJ_QUEUE_NODE,
    CL_OBJ_CAPTION,
    CL_OBJ_HASHTABLE,
    CL_OBJ_CIRCULAR_QUEUE,
    CL_OBJ_CIRCULAR_STACK
};

struct cl_object_hdr {
    unsigned long long  lib_id;
    enum cl_object      object;
};

#define cl_struct_member(type, name)        \
    type name;

#define cl_struct_declare(name, members)    \
    struct cl_obj_##name {                  \
        struct cl_object_hdr  hdr;          \
        members                             \
    }

#define cl_struct(name)                     \
    struct cl_obj_##name

#define CL_OBJECT_HEADER_ID_SIZE            \
    sizeof(struct cl_object_hdr)

void typeof_set(enum cl_object type, void *p);
void typeof_set_with_offset(enum cl_object type, void *p, unsigned int offset);
bool typeof_validate_object(const void *p, enum cl_object type);
bool typeof_validate_object_with_offset(const void *p, enum cl_object type,
                                        unsigned int offset);

enum cl_object typeof_guess_object(const void *ptr);

#endif

