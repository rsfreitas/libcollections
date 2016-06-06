
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
    CSTRING,
    CSTRINGLIST,
    CFG_FILE,
    CFG_SECTION,
    CFG_KEY,
    CJSON,
    CDATETIME,
    CTIMEOUT,
    CTHREAD,
    CTHREAD_DATA,
    CTIMER,
    CTIMER_INFO,
    CTIMER_ARG,
    CHAT,
    CLIST,
    CEVENT,
    CVALUE,
    CSPEC,
    COUNTER,
    CPLUGIN,
    CPLUGIN_ARG,
    CPLUGIN_INFO,
    CLOG
};

struct cobject_hdr {
    unsigned short int  lib_id;
    enum cl_object      object;
};

#define cl_struct_member(type, name)        \
    type name;

#define cl_struct_declare(name, members)    \
    struct cl_obj_##name {                  \
        struct cobject_hdr  hdr;            \
        members                             \
    }

#define cl_struct(name)                     \
    struct cobject_##name

#endif

