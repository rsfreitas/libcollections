
/*
 * Description: Macros for C and C++ plugins.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:33:09 BRST 2015
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

#ifndef _COLLECTIONS_CL_PLUGIN_MACROS_H
#define _COLLECTIONS_CL_PLUGIN_MACROS_H			1

#ifndef _STDINT_H
# include <stdint.h>
#endif

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_plugin_macros.h> directly; include <collections.h> instead."
# endif
#endif

#ifdef __cplusplus
# define CPLUGIN_EXTERN_C()     extern "C"
#else
# define CPLUGIN_EXTERN_C()
#endif

#define OBJEXPORT               __attribute__((visibility("default")))

/*
 * Macros to be used with libcollections while manipulating plugins.
 *
 * Upper case macros must be used from within a plugin.
 * Lower case macros must be used from a 'plugin manager'.
 */

/* Macro to identify an exported plugin function */
#define CPLUGIN_OBJECT_EXPORT(foo)              \
    CPLUGIN_EXTERN_C() void OBJEXPORT foo(uint32_t caller_id, cplugin_t *cpl, \
                                          cplugin_arg_t *args)

/*
 * Macro to set the return value from an exported plugin function. To be used
 * instead of a 'return' call.
 */
#define CPLUGIN_SET_RETURN_VALUE(type, arg...)      \
    cplugin_set_return_value(cpl, __FUNCTION__, caller_id, type, ## arg)

/* Macro to set the return value from an exported function as void */
#define CPLUGIN_SET_RETURN_VALUE_AS_VOID()      \
    {\
        CPLUGIN_SET_RETURN_VALUE(CL_VOID, 0);\
    }\

/*
 * Macro to set the plugin name internally, so a plugin manager may known
 * it.
 */
#define CPLUGIN_SET_PLUGIN_ENTRY_NAME(name)                \
    CPLUGIN_EXTERN_C() struct cplugin_entry_s OBJEXPORT *cplugin_set_plugin_entry_name(void)\
    {\
        return &name##_plugin_entry;\
    }\

/* Macro to get the number of arguments from an exported plugin function */
#define CPLUGIN_ARG_COUNT()                     \
    cplugin_arg_count(args)

/*
 * Macro to get the actual value from an argument of an exported plugin
 * function.
 */
#define CPLUGIN_ARGUMENT(arg_name)              \
    cplugin_argument(args, arg_name)

/*
 * Macro to access the argument @args from an exported function and
 * remove the GCC warning.
 */
#define CPLUGIN_SET_VOID_ARGS()                 \
    {\
        (void)args;\
    }\

/*
 * Macro to make the call to an exported plugin function, so the correct
 * number of arguments is passed to the @argc argument from 'cplugin_call_ex'
 * function.
 */
#define cplugin_call(cpl, function_name, arg...)   \
    cplugin_call_ex(CL_PP_NARG(cpl, function_name, ## arg), \
                    cpl, function_name, ## arg)

#endif

