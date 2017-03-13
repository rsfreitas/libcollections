
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

#define CPLUGIN_FNEXPORT        __attribute__((visibility("default")))

/*
 * Macros to be used with libcollections while manipulating plugins.
 *
 * Lower case macros must be used from a 'plugin manager'.
 * Upper case macros must be used from within a plugin.
 */

/*
 * Macro to make the call to an exported plugin function, so the correct
 * number of arguments is passed to the @argc argument from 'cplugin_call_ex'
 * function.
 */
#define cplugin_call(cpl, function_name, arg...)   \
    cplugin_call_ex(CL_PP_NARG(cpl, function_name, ## arg), \
                    cpl, function_name, ## arg)

/* Macro to identify an exported plugin function */
/*#define CPLUGIN_OBJECT_EXPORT(foo)              \
    CPLUGIN_EXTERN_C() void CPLUGIN_FNEXPORT foo(uint32_t caller_id, \
                                                 cplugin_t *cpl, \
                                                 cplugin_arg_t *args)
*/
#define CPLUGIN_OBJECT_VOID(return_type, foo)       \
    CPLUGIN_EXTERN_C() return_type CPLUGIN_FNEXPORT foo(void)

#define CPLUGIN_OBJECT_PTR_ONLY(return_type, foo)   \
    CPLUGIN_EXTERN_C() return_type CPLUGIN_FNEXPORT foo(void *ptr)

#define CPLUGIN_OBJECT_ARGS_ONLY(return_type, foo)   \
    CPLUGIN_EXTERN_C() return_type CPLUGIN_FNEXPORT foo(const char *args)

#define CPLUGIN_OBJECT_ARGS_AND_PTR(return_type, foo)   \
    CPLUGIN_EXTERN_C() return_type CPLUGIN_FNEXPORT foo(const char *args, void *ptr)

/*
 * Macro to set the return value from an exported plugin function. To be used
 * instead of a 'return' call.
 */
/*#define CPLUGIN_SET_RETURN_VALUE(type, arg...)      \
    cplugin_set_return_value(cpl, __FUNCTION__, caller_id, type, ## arg)
*/
/* Macro to set the return value from an exported function as void */
/*#define CPLUGIN_SET_RETURN_VALUE_AS_VOID()      \
    {\
        CPLUGIN_SET_RETURN_VALUE(CL_VOID, 0);\
    }\
*/
/* Macro to get the number of arguments from an exported plugin function */
/*#define CPLUGIN_ARG_COUNT()                     \
    cplugin_arg_count(args)
*/
/*
 * Macro to get the actual value from an argument of an exported plugin
 * function.
 */
/*#define CPLUGIN_ARGUMENT(arg_name)              \
    cplugin_argument(args, arg_name)
*/
/*
 * Macro to release an argument loaded inside a plugin function.
 */
/*#define CPLUGIN_RELEASE_ARGUMENT(argument)      \
    cobject_unref(argument)
*/
/*
 * Macro to access the argument @args from an exported function and
 * remove the GCC warning.
 */
/*#define CPLUGIN_SET_VOID_ARGS()                 \
    {\
        (void)args;\
    }\
*/
#define CPLUGIN_PTR_ARGUMENT()                  \
    ptr

#define CPLUGIN_LOAD_ARGUMENTS()                \
    cjson_t *___jargs = cjson_parse_string(args)

#define CPLUGIN_UNLOAD_ARGUMENTS()              \
    cjson_delete(___jargs)

/*
 * Macros to get arguments inside an exported plugin function.
 */
#define CPLUGIN_ARGUMENT_CHAR(arg_name)          \
    ({ char __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_at(___value, 0); __x; })

#define CPLUGIN_ARGUMENT_UCHAR(arg_name)          \
    ({ unsigned char __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (unsigned char)cstring_at(___value, 0); __x; })

#define CPLUGIN_ARGUMENT_INT(arg_name)          \
    ({ int __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_int(___value); __x; })

#define CPLUGIN_ARGUMENT_UINT(arg_name)         \
    ({ unsigned int __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (unsigned int)cstring_to_int(___value); __x; })

#define CPLUGIN_ARGUMENT_SINT(arg_name)          \
    ({ short int __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (short int)cstring_to_int(___value); __x; })

#define CPLUGIN_ARGUMENT_USINT(arg_name)        \
    ({ unsigned short int __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (unsigned short int)cstring_to_int(___value); __x; })

#define CPLUGIN_ARGUMENT_LONG(arg_name)          \
    ({ long __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_long(___value); __x; })

#define CPLUGIN_ARGUMENT_ULONG(arg_name)        \
    ({ unsigned long __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (unsigned long)cstring_to_long(___value); __x; })

#define CPLUGIN_ARGUMENT_LLONG(arg_name)        \
    ({ long long __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_long_long(___value); __x; })

#define CPLUGIN_ARGUMENT_ULLONG(arg_name)       \
    ({ unsigned long long __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (unsigned long long)cstring_to_long_long(___value); __x; })

#define CPLUGIN_ARGUMENT_FLOAT(arg_name)        \
    ({ float __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_float(___value); __x; })

#define CPLUGIN_ARGUMENT_DOUBLE(arg_name)       \
    ({ double __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_double(___value); __x; })

#define CPLUGIN_ARGUMENT_STRING(arg_name)       \
    ({ char *__x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = (char *)cstring_valueof(___value); __x; })

#define CPLUGIN_ARGUMENT_BOOL(arg_name)          \
    ({ bool __x; cjson_t *___node = cjson_get_object_item(___jargs, arg_name); cstring_t *___value = cjson_get_object_value(___node); __x = cstring_to_int(___value); __x; })

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
#define CPLUGIN_SET_INFO(name, version, author, description, api) \
    CPLUGIN_EXTERN_C() const char CPLUGIN_FNEXPORT *plugin_name(void) {\
        return #name;\
    }\
\
    CPLUGIN_EXTERN_C() const char CPLUGIN_FNEXPORT *plugin_version(void) {\
        return version;\
    }\
\
    CPLUGIN_EXTERN_C() const char CPLUGIN_FNEXPORT *plugin_author(void) {\
        return author;\
    }\
\
    CPLUGIN_EXTERN_C() const char CPLUGIN_FNEXPORT *plugin_api(void) {\
        return api;\
    }\
\
    CPLUGIN_EXTERN_C() const char CPLUGIN_FNEXPORT *plugin_description(void) {\
        return description;\
    }\

/* Plugin init function */
#define CPLUGIN_INIT()  \
    CPLUGIN_EXTERN_C() int CPLUGIN_FNEXPORT plugin_init(void)

/* Plugin uninit function */
#define CPLUGIN_UNINIT()    \
    CPLUGIN_EXTERN_C() void CPLUGIN_FNEXPORT plugin_uninit(void)

#endif

