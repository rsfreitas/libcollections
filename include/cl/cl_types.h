
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 16 23:50:46 2015
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

#ifndef _COLLECTIONS_CL_TYPES_H
#define _COLLECTIONS_CL_TYPES_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_types.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _SIGNAL_H
# include <signal.h>
#endif

#ifndef _STDBOOL_H
# include <stdbool.h>
#endif

#ifndef _ASM_X86_TYPES_H
# include <asm/types.h>
#endif

#define cl_fourcc(a, b, c, d)           \
    (((__u32)(a) << 0) |                \
     ((__u32)(b) << 8) |                \
     ((__u32)(c) << 16) |               \
     ((__u32)(d) << 24))

#define CL_PP_RSEQ_N()                  \
    63,62,61,60,                        \
    59,58,57,56,55,54,53,52,51,50,      \
    49,48,47,46,45,44,43,42,41,40,      \
    39,38,37,36,35,34,33,32,31,30,      \
    29,28,27,26,25,24,23,22,21,20,      \
    19,18,17,16,15,14,13,12,11,10,      \
    9,8,7,6,5,4,3,2,1,0

#define CL_PP_ARG_N(                     \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,   \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,  \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30,  \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40,  \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50,  \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60,  \
    _61,_62,_63,N,...) N

#define CL_PP_NARG_(...)                 \
     CL_PP_ARG_N(__VA_ARGS__)

#define CL_PP_NARG(...)                  \
     CL_PP_NARG_(__VA_ARGS__, CL_PP_RSEQ_N())

/** types */
enum cl_type {
    CL_VOID,
    CL_CHAR,
    CL_UCHAR,
    CL_INT,
    CL_UINT,
    CL_SINT,
    CL_USINT,
    CL_FLOAT,
    CL_DOUBLE,
    CL_LONG,
    CL_ULONG,
    CL_LLONG,
    CL_ULLONG,
    CL_POINTER,
    CL_STRING,      /* 'char *' strings */
    CL_BOOLEAN,
    CL_CSTRING      /* collections strings */
};

/** cobject token types */
#define CL_OBJECT_CHAR          "b"
#define CL_OBJECT_UCHAR         "B"
#define CL_OBJECT_INT           "i"
#define CL_OBJECT_UINT          "I"
#define CL_OBJECT_SINT          "h"
#define CL_OBJECT_USINT         "H"
#define CL_OBJECT_FLOAT         "f"
#define CL_OBJECT_DOUBLE        "d"
#define CL_OBJECT_LONG          "l"
#define CL_OBJECT_ULONG         "k"
#define CL_OBJECT_LLONG         "L"
#define CL_OBJECT_ULLONG        "K"
#define CL_OBJECT_BOOLEAN       "Z"
#define CL_OBJECT_CSTRING       "z"
#define CL_OBJECT_STRING        "s"
#define CL_OBJECT_POINTER       "P"

/** cobject sizeof */
#define CL_OBJECT_SIZEOF        \
    sizeof(unsigned long long) + sizeof(unsigned long long)

/** error type */
typedef int                     cl_errno;

/** config types */
typedef void                    cl_cfg_file_t;      /** config file */
typedef void                    cl_cfg_block_t;     /** config section block */
typedef void                    cl_cfg_entry_t;     /** config block entry */

/** doubly linked list entry type */
typedef void                    cl_list_entry_t;

/** string types */
typedef void                    cl_string_t;        /** string */
typedef void                    cl_stringlist_t;    /** list of strings */

/** JSON type */
typedef void                    cl_json_t;

/** datetime types */
typedef void                    cl_datetime_t;
typedef void                    cl_timeout_t;

/** thread type */
typedef void                    cl_thread_t;

/** event type */
typedef void                    cl_event_t;

/** timer types */
typedef void                    cl_timer_t;
typedef void                    cl_timer_info_t;
typedef union sigval            cl_timer_arg_t;

/** chat type */
typedef void                    cl_chat_t;

/** value type */
typedef void                    cl_object_t;

/** generic parameters specifications */
typedef void                    cl_spec_t;

/** counter type */
typedef void                    cl_counter_t;

/** plugin types */
typedef void                    cl_plugin_t;
typedef void                    cl_plugin_info_t;

/** log type */
typedef void                    cl_log_t;

/** list type */
typedef void                    cl_list_t;
typedef void                    cl_list_node_t;

/** image type */
typedef void                    cl_image_t;

/** stack type */
typedef void                    cl_stack_t;
typedef void                    cl_stack_node_t;

/** queue type */
typedef void                    cl_queue_t;
typedef void                    cl_queue_node_t;

/** image text */
typedef void                    cl_caption_t;

/** hashtable type */
typedef void                    cl_hashtable_t;

/** circular queue type */
typedef void                    cl_cqueue_t;

/** circular stack type */
typedef void                    cl_cstack_t;

#endif

