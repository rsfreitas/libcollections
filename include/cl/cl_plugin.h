
/*
 * Description:
 * Author: Rodrigo Freitas
 * Created at: Fri Jan 29 11:27:41 2016
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

#ifndef _COLLECTIONS_CL_PLUGIN_H
#define _COLLECTIONS_CL_PLUGIN_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_plugin.h> directly; include <collections.h> instead."
# endif
#endif

/** Plugin supported languages */
enum cl_plugin_type {
    CL_PLUGIN_UNKNOWN = 0,
    CL_PLUGIN_ELF     = (1 << 0),         /* C, C++, Rust and Go */
    CL_PLUGIN_PYTHON  = (1 << 1),
    CL_PLUGIN_JAVA    = (1 << 2)
};

/** Functions argument mode */
enum cl_plugin_arg_mode {
    CL_PLUGIN_ARGS_VOID               = 0,
    CL_PLUGIN_ARGS_COMMON             = (1 << 0),
    CL_PLUGIN_ARGS_POINTER            = (1 << 1)
};

/**
 * @name cl_plugin_load
 * @brief Loads a plugin to the memory.
 *
 * The function will load a plugin to memory, pulling informations from it. If
 * the plugin is a C or C++ plugin, it will use an internal information structure
 * to load some informations, otherwise it will use a specific plugin API (either
 * Python or Java) to extract these.
 *
 * It will look on some default Linux path to load the plugin. But the full path
 * may also be informed with its name.
 *
 * By performing a successful initialization, an initialization function will be
 * called and executed.
 *
 * @param [in] pathname: The plugin name (with or without its path).
 *
 * @return On success returns a cl_plugin_t object with the loaded plugin or NULL
 *         otherwise.
 */
cl_plugin_t *cl_plugin_load(const char *pathname);

/**
 * @name cl_plugin_unload
 * @brief Releases a plugin from memory.
 *
 * @param [in] cpl: The cl_plugin_t object from the plugin which will be
 *                  released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_plugin_unload(cl_plugin_t *cpl);

/**
 * @name cl_plugin_info
 * @brief Gets a pointer to an object representing all informations a plugin
 *        may have.
 *
 * @param [in] cpl: The loaded plugin object.
 *
 * @return On success returns a cl_plugin_info_t object with all plugin
 *         informations or NULL otherwise.
 */
cl_plugin_info_t *cl_plugin_info(const cl_plugin_t *cpl);

/**
 * @name cl_plugin_info_from_file
 * @brief Gets a pointer to an object representing all informations a plugin
 *        may have, directly from its file, without loading it.
 *
 * @param [in] pathname: The complete path and name to the plugin.
 *
 * @return On success returns a cl_plugin_info_t object with all plugin
 *         informations or NULL otherwise.
 */
cl_plugin_info_t *cl_plugin_info_from_file(const char *pathname);

/**
 * @name cl_plugin_info_unref
 * @brief Release a plugin info object from memory.
 *
 * @param [in,out] info: The plugin info object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_plugin_info_unref(cl_plugin_info_t *info);

/**
 * @name cl_plugin_name
 * @brief Gets the internal plugin name.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin name
 *         or NULL otherwise.
 */
const char *cl_plugin_name(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_version
 * @brief Gets the internal plugin version.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin version
 *         or NULL otherwise.
 */
const char *cl_plugin_version(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_author
 * @brief Gets the internal plugin author.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin author
 *         name or NULL otherwise.
 */
const char *cl_plugin_author(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_description
 * @brief Gets the internal plugin description.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin
 *         description or NULL otherwise.
 */
const char *cl_plugin_description(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_API
 * @brief Gets the plugin API.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a cl_string_t with the internal plugin API in its
 *         original JSON format or NULL otherwise.
 */
cl_string_t *cl_plugin_API(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_functions
 * @brief Gets the plugin exported function names.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a cl_stringlist_t object with all the exported
 *         function names from the plugin or NULL otherwise.
 */
cl_stringlist_t *cl_plugin_functions(const cl_plugin_info_t *info);

/**
 * @name cl_plugin_function_return_type
 * @brief Gets the return type from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success return the return type (as a number) from the requested
 *         function or -1 otherwise.
 */
enum cl_type cl_plugin_function_return_type(const cl_plugin_info_t *info,
                                            const char *function_name);

/**
 * @name cl_plugin_function_arguments
 * @brief Gets the argument names from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success returns a cl_stringlist_t object with all the argument
 *         names from the requested function or NULL otherwise.
 */
cl_stringlist_t *cl_plugin_function_arguments(const cl_plugin_info_t *info,
                                              const char *function_name);

/**
 * @name cl_plugin_function_arg_mode
 * @brief Gets the argument mode from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success returns the argument mode (as a number) from the requested
 *         function or -1 otherwise.
 */
enum cl_plugin_arg_mode cl_plugin_function_arg_mode(const cl_plugin_info_t *info,
                                                    const char *function_name);

/**
 * @name cl_plugin_function_arg_type
 * @brief Gets the argument type from a specific argument from an exported
 *        function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 * @param [in] argument_name: The argument name which will be sought.
 *
 * @return On success returns the argument type (as a number) from the
 *         requested function or -1 otherwise.
 */
enum cl_type cl_plugin_function_arg_type(const cl_plugin_info_t *info,
                                         const char *function_name,
                                         const char *argument_name);

/**
 * @name cl_plugin_call_ex
 * @brief Makes a call to a function inside a plugin.
 *
 * This function is responsible for making the call of a function inside a
 * plugin. Its arguments are inserted into a list so they can be accessed
 * inside of it.
 *
 * The called function arguments must be passed according with the type
 * defined on the plugin API.
 *
 * To the case of a function with the CL_PLUGIN_ARG_FIXED arguments type,
 * they must be passed this way: "argument_name", object, "argument_name",
 * "value", NULL. Such as:
 *
 * cl_plugin_call(7, cpl, "foo", "arg1", value, "arg2", value, NULL);
 *
 * To the case of CL_PLUGIN_ARG_VAR arguments type, they must be passed in
 * this way: "argument_name", type, object, "argument_name", type, object,
 * NULL. Such as:
 *
 * cl_plugin_call_ex(9, cpl, "foo", "arg1", type, value, "arg2", type, value,
 *                 NULL);
 *
 * It is recomended to use cl_plugin_call macro instead of a direct call to
 * this function.
 *
 * @param [in] argc: The total number of arguments passed to the function.
 * @param [in] cpl: The cl_plugin_t object from the loaded plugin.
 * @param [in] function_name: The function name which will be called.
 * @param [in] ...: Function arguments.
 *
 * @return On success returns a cl_object_t with the called function return
 *         object or NULL otherwise.
 */
cl_object_t *cl_plugin_call_ex(int argc, cl_plugin_t *cpl,
                               const char *function_name, ...);

/**
 * @name cl_plugin_set_supported_types
 * @brief Enables plugin supported by an application.
 *
 * This function must be called before all cl_plugin_load calls.
 *
 * @param [in] types: A bitfield with all supported plugin types.
 */
void cl_plugin_set_supported_types(enum cl_plugin_type types);

#endif

