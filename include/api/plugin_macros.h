
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

#ifndef _COLLECTIONS_API_PLUGIN_MACROS_H
#define _COLLECTIONS_API_PLUGIN_MACROS_H			1

#ifndef _STDINT_H
# include <stdint.h>
#endif

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <plugin_macros.h> directly; include <collections.h> instead."
# endif
#endif

#ifdef __cplusplus
# define CL_PLUGIN_EXTERN_C()     extern "C"
#else
# define CL_PLUGIN_EXTERN_C()
#endif

#define CL_PLUGIN_FNEXPORT        __attribute__((visibility("default")))

/*
 * Macros to be used with libcollections while manipulating plugins.
 *
 * Lower case macros must be used from a 'plugin manager'.
 * Upper case macros must be used from within a plugin.
 */

/*
 * Macro to make the call to an exported plugin function, so the correct
 * number of arguments is passed to the @argc argument from 'cl_plugin_call_ex'
 * function.
 */
#define cl_plugin_call(cpl, function_name, return_type, arg...)  \
    cl_plugin_call_ex(CL_PP_NARG(cpl, function_name, return_type, ## arg), \
                      cpl, function_name, return_type, ## arg)

/**
 * Macro to define exported plugin functions:
 */
#define CL_PLUGIN_FUNCTION(return_type, name)       \
    CL_PLUGIN_EXTERN_C() return_type CL_PLUGIN_FNEXPORT name(void *args)

/**
 * Macros to mandatory plugin functions.
 *
 * A plugin must have a few functions so we may be able to call them from
 * the library. These functions are the way that a plugin manager will find
 * its information. They all must return a constant string, and their
 * prototypes are:
 *
 * - plugin_name
 *
 * Prototype: const char *plugin_name(void);
 *
 * - plugin_version
 *
 * Prototype: const char *plugin_version(void);
 *
 * - plugin_author
 *
 * Prototype: const char *plugin_author(void);
 *
 * - plugin_description
 *
 * Prototype: const char *plugin_description(void);
 */

/*
 * Plugin mandatory information.
 */
#define CL_PLUGIN_SET_INFO(name, version, author, description) \
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_name(void) {\
        return name;\
    }\
\
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_version(void) {\
        return version;\
    }\
\
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_author(void) {\
        return author;\
    }\
\
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_description(void) {\
        return description;\
    }\

/* Plugin init function */
#define CL_PLUGIN_INIT()  \
    CL_PLUGIN_EXTERN_C() int CL_PLUGIN_FNEXPORT plugin_init(void)

/* Plugin uninit function */
#define CL_PLUGIN_UNINIT()    \
    CL_PLUGIN_EXTERN_C() void CL_PLUGIN_FNEXPORT plugin_uninit(void)

/*
 * When a plugin function receives no argument (a void one) this should be used
 * to avoid compilation warnings.
 */
#define CL_PLUGIN_IGNORE_ARGUMENTS()    \
    (void)args

#endif

