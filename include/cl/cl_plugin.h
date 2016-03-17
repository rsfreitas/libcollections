
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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TODO: Substitute cplugin_internal_data_t for a cvalue_t.
 */

/** Functions argument mode */
enum cplugin_arg {
    CPLUGIN_ARG_FIXED,
    CPLUGIN_ARG_VAR,
    CPLUGIN_NO_ARGS
};

/** Identification of a C/C++ plugin */
struct cplugin_entry_s {
    const char                      *name;
    const char                      *version;
    const char                      *author;
    const char                      *description;
    const char                      *api;
    cplugin_internal_data_t         *(*startup)(CPLUGIN_STARTUP_ARGS);
    int                             (*shutdown)(CPLUGIN_SHUTDOWN_ARGS);
};

/**
 * @name cplugin_load
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
 * @return On success returns a cplugin_t object with the loaded plugin or NULL
 *         otherwise.
 */
cplugin_t *cplugin_load(const char *pathname);

/**
 * @name cplugin_unload
 * @brief Releases a plugin from memory.
 *
 * @param [in] cpl: The cplugin_t object from the plugin which will be
 *                  released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cplugin_unload(cplugin_t *cpl);

/**
 * @name cplugin_get_startup_data
 * @brief Gets the startup function return value.
 *
 * This function gets a pointer to the data returned by the plugin startup
 * function so it can be used from other plugin functions.
 *
 * It is recommended to use the CPLUGIN_GET_STARTUP_DATA instead of a direct
 * call to this function.
 *
 * @param [in] cpl: The cplugin_t object from the loaded plugin.
 *
 * @return On success returns a pointer to the data that was returned by the
 *         plugin startup function or NULL otherwise.
 */
cplugin_internal_data_t *cplugin_get_startup_data(cplugin_t *cpl);

/**
 * @name cplugin_get_shutdown_arg
 * @brief Gets the value passed as argument to the shutdown plugin function.
 *
 * This function gets a pointer to the data passed as argument to the shutdown
 * plugin function. The same data that was returned from the startup plugin
 * function.
 *
 * It is recommended to use de CPLUGIN_GET_SHUTDOWN_ARG macro instead of a
 * direct call to this function. And it should be called from the plugin
 * shutdown function.
 *
 * @param [in] arg: The cplugin_internal_data_t object passed as argument
 *                  to the shutdown function.
 *
 * @return Returns the argument from the plugin shutdown function.
 */
cplugin_internal_data_t *cplugin_get_shutdown_arg(cplugin_internal_data_t *arg);

/**
 * @name cplugin_info
 * @brief Gets a pointer to an object representing all informations a plugin
 *        may have.
 *
 * @param [in] cpl: The loaded plugin object.
 *
 * @return On success returns a cplugin_info_t object with all plugin
 *         informations or NULL otherwise.
 */
cplugin_info_t *cplugin_info(const cplugin_t *cpl);

/**
 * @name cplugin_info_from_file
 * @brief Gets a pointer to an object representing all informations a plugin
 *        may have, directly from its file, without loading it.
 *
 * @param [in] pathname: The complete path and name to the plugin.
 *
 * @return On success returns a cplugin_info_t object with all plugin
 *         informations or NULL otherwise.
 */
cplugin_info_t *cplugin_info_from_file(const char *pathname);

/**
 * @name cplugin_info_unref
 * @brief Release a plugin info object from memory.
 *
 * @param [in,out] info: The plugin info object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cplugin_info_unref(cplugin_info_t *info);

/**
 * @name cplugin_name
 * @brief Gets the internal plugin name.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin name
 *         or NULL otherwise.
 */
const char *cplugin_name(const cplugin_info_t *info);

/**
 * @name cplugin_version
 * @brief Gets the internal plugin version.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin version
 *         or NULL otherwise.
 */
const char *cplugin_version(const cplugin_info_t *info);

/**
 * @name cplugin_author
 * @brief Gets the internal plugin author.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin author
 *         name or NULL otherwise.
 */
const char *cplugin_author(const cplugin_info_t *info);

/**
 * @name cplugin_description
 * @brief Gets the internal plugin description.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a constant string with the internal plugin
 *         description or NULL otherwise.
 */
const char *cplugin_description(const cplugin_info_t *info);

/**
 * @name cplugin_API
 * @brief Gets the plugin API.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a cstring_t with the internal plugin API in its
 *         original JSON format or NULL otherwise.
 */
cstring_t *cplugin_API(const cplugin_info_t *info);

/**
 * @name cplugin_functions
 * @brief Gets the plugin exported function names.
 *
 * @param [in] info: The plugin info object.
 *
 * @return On success returns a cstring_list_t object with all the exported
 *         function names from the plugin or NULL otherwise.
 */
cstring_list_t *cplugin_functions(const cplugin_info_t *info);

/**
 * @name cplugin_function_return_type
 * @brief Gets the return type from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success return the return type (as a number) from the requested
 *         function or -1 otherwise.
 */
enum cl_type cplugin_function_return_type(const cplugin_info_t *info,
                                          const char *function_name);

/**
 * @name cplugin_function_arguments
 * @brief Gets the argument names from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success returns a cstring_list_t object with all the argument
 *         names from the requested function or NULL otherwise.
 */
cstring_list_t *cplugin_function_arguments(const cplugin_info_t *info,
                                           const char *function_name);

/**
 * @name cplugin_function_arg_mode
 * @brief Gets the argument mode from an exported function.
 *
 * @param [in] info: The plugin info object.
 * @param [in] function_name: The name of the function which will be sought.
 *
 * @return On success returns the argument mode (as a number) from the requested
 *         function or -1 otherwise.
 */
enum cplugin_arg cplugin_function_arg_mode(const cplugin_info_t *info,
                                           const char *function_name);

/**
 * @name cplugin_function_arg_type
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
enum cl_type cplugin_function_arg_type(const cplugin_info_t *info,
                                       const char *function_name,
                                       const char *argument_name);

/**
 * @name cplugin_call_ex
 * @brief Makes a call to a function inside a plugin.
 *
 * This function is responsible for making the call of a function inside a
 * plugin. Its arguments are inserted into a list so they can be accessed
 * inside of it.
 *
 * The called function arguments must be passed according with the type
 * defined on the plugin API.
 *
 * To the case of a function with the CPLUGIN_ARG_FIXED arguments type,
 * they must be passed this way: "argument_name", value, "argument_name",
 * "value", NULL. Such as:
 *
 * cplugin_call(7, cpl, "foo", "arg1", value, "arg2", value, NULL);
 *
 * To the case of CPLUGIN_ARG_VAR arguments type, they must be passed in
 * this way: "argument_name", type, value, "argument_name", type, value,
 * NULL. Such as:
 *
 * cplugin_call_ex(9, cpl, "foo", "arg1", type, value, "arg2", type, value,
 *                 NULL);
 *
 * It is recomended to use cplugin_call macro instead of a direct call to
 * this function.
 *
 * @param [in] argc: The total number of arguments passed to the function.
 * @param [in] cpl: The cplugin_t object from the loaded plugin.
 * @param [in] function_name: The function name which will be called.
 * @param [in] ...: Function arguments.
 *
 * @return On success returns a cvalue_t with the called function return
 *         value or NULL otherwise.
 */
cvalue_t *cplugin_call_ex(int argc, cplugin_t *cpl,
                          const char *function_name, ...);

/**
 * @name cplugin_set_return_value
 * @brief Sets up the return value from an exported plugin function.
 *
 * This functions must be called within an exported plugin function. It should be
 * called as a C return statement, so a value may returned to the caller.
 *
 * It will block using a mutex to add the returned value into an internal list
 * of return values.
 *
 * It is recommended that the CPLUGIN_RETURN_VALUE macro is called instead of a
 * direct call to this function.
 *
 * If the return value is a pointer, its size must also be informed, so the
 * library is able to dup it inside. Also a custom function to release memory
 * from this data may be informed.
 *
 * @param [in] cpl: The cplugin_t object.
 * @param [in] function_name: The function name which will have its return
 *                            value set.
 * @param [in] caller_id: Identification function number.
 * @param [in] type: The return type.
 * @param [in] ...: The return value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cplugin_set_return_value(cplugin_t *cpl, const char *function_name,
                             unsigned int caller_id, enum cl_type type, ...);

/**
 * @name cplugin_argument
 * @brief Gets the current argument value from an exported function.
 *
 * It is recomended to use CPLUGIN_ARGUMENT macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return a cvalue_t object with the arguemtn value or NULL
 *         otherwise.
 */
cvalue_t *cplugin_argument(const cplugin_arg_t *args, const char *arg_name);

/**
 * @name cplugin_arg_count
 * @brief Gets the number of arguments of a function.
 *
 * It is recomended to use CPLUGIN_ARG_COUNT instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 *
 * @return On success returns the function number of arguments or -1
 *         otherwise.
 */
int cplugin_arg_count(const cplugin_arg_t *args);

#ifdef __cplusplus
}
#endif

#endif

