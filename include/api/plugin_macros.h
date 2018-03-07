
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
#define cl_plugin_call(cpl, function_name, arg...)   \
    cl_plugin_call_ex(CL_PP_NARG(cpl, function_name, ## arg), \
                      cpl, function_name, ## arg)

/*
 * Macro to make the call to an exported plugin function which was not declared
 * inside the exported API. It is responsible to inform the correct number of
 * arguments is passed to the @argc argument from 'cl_plugin_foreign_call_ex'
 * function.
 */
#define cl_plugin_foreign_call(cpl, function_name, return_type, arg_mode, arg...)  \
    cl_plugin_foreign_call_ex(CL_PP_NARG(cpl, function_name, return_type, arg_mode, ## arg), \
                              cpl, function_name, return_type, arg_mode, ## arg)

/**
 * Macros to define exported plugin functions:
 */
/* Without arguments */
#define CL_PLUGIN_OBJECT_VOID(return_type, foo)       \
    CL_PLUGIN_EXTERN_C() return_type CL_PLUGIN_FNEXPORT foo(void)

/* With only a pointer as argument */
#define CL_PLUGIN_OBJECT_PTR_ONLY(return_type, foo)   \
    CL_PLUGIN_EXTERN_C() return_type CL_PLUGIN_FNEXPORT foo(void *ptr)

/* With arguments different of a pointer as argument */
#define CL_PLUGIN_OBJECT_ARGS_ONLY(return_type, foo)   \
    CL_PLUGIN_EXTERN_C() return_type CL_PLUGIN_FNEXPORT foo(const char *args)

/* With mixed arguments, pointer and others */
#define CL_PLUGIN_OBJECT_ARGS_AND_PTR(return_type, foo)   \
    CL_PLUGIN_EXTERN_C() return_type CL_PLUGIN_FNEXPORT foo(const char *args, void *ptr)

/*
 * Macro to get the pointer argument received from an exported plugin
 * function.
 */
#define CL_PLUGIN_PTR_ARGUMENT()                  \
    ptr

/*
 * Macro to load all arguments, except a pointer, into a JSON structure
 * to easy the extraction of all others.
 */
#define CL_PLUGIN_LOAD_ARGUMENTS()                \
    cl_json_t *___jargs = cl_json_parse_string(args)

/*
 * Macro to release the JSON structure of the exported plugin function
 * arguments.
 */
#define CL_PLUGIN_UNLOAD_ARGUMENTS()              \
    cl_json_delete(___jargs)

/*
 * Macros to get arguments inside an exported plugin function.
 */
#define CL_PLUGIN_ARGUMENT_CHAR(arg_name)          \
    ({ char __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_at(___value, 0); __x; })

#define CL_PLUGIN_ARGUMENT_UCHAR(arg_name)          \
    ({ unsigned char __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (unsigned char)cl_string_at(___value, 0); __x; })

#define CL_PLUGIN_ARGUMENT_INT(arg_name)          \
    ({ int __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_int(___value); __x; })

#define CL_PLUGIN_ARGUMENT_UINT(arg_name)         \
    ({ unsigned int __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (unsigned int)cl_string_to_int(___value); __x; })

#define CL_PLUGIN_ARGUMENT_SINT(arg_name)          \
    ({ short int __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (short int)cl_string_to_int(___value); __x; })

#define CL_PLUGIN_ARGUMENT_USINT(arg_name)        \
    ({ unsigned short int __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (unsigned short int)cl_string_to_int(___value); __x; })

#define CL_PLUGIN_ARGUMENT_LONG(arg_name)          \
    ({ long __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_long(___value); __x; })

#define CL_PLUGIN_ARGUMENT_ULONG(arg_name)        \
    ({ unsigned long __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (unsigned long)cl_string_to_long(___value); __x; })

#define CL_PLUGIN_ARGUMENT_LLONG(arg_name)        \
    ({ long long __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_long_long(___value); __x; })

#define CL_PLUGIN_ARGUMENT_ULLONG(arg_name)       \
    ({ unsigned long long __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (unsigned long long)cl_string_to_long_long(___value); __x; })

#define CL_PLUGIN_ARGUMENT_FLOAT(arg_name)        \
    ({ float __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_float(___value); __x; })

#define CL_PLUGIN_ARGUMENT_DOUBLE(arg_name)       \
    ({ double __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_double(___value); __x; })

#define CL_PLUGIN_ARGUMENT_STRING(arg_name)       \
    ({ char *__x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = (char *)cl_string_valueof(___value); __x; })

#define CL_PLUGIN_ARGUMENT_BOOL(arg_name)          \
    ({ bool __x; cl_json_t *___node = cl_json_get_object_item(___jargs, arg_name); cl_string_t *___value = cl_json_get_object_value(___node); __x = cl_string_to_int(___value); __x; })

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
 * - plugin_api
 *
 * Prototype: const char *plugin_api(void);
 *
 * - plugin_description
 *
 * Prototype: const char *plugin_description(void);
 */

/*
 * Plugin mandatory informations.
 */
#define CL_PLUGIN_SET_INFO(name, version, author, description, api) \
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_name(void) {\
        return #name;\
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
    CL_PLUGIN_EXTERN_C() const char CL_PLUGIN_FNEXPORT *plugin_api(void) {\
        return api;\
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

#endif

