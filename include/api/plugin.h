
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

#ifndef _COLLECTIONS_API_PLUGIN_H
#define _COLLECTIONS_API_PLUGIN_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <plugin.h> directly; include <collections.h> instead."
# endif
#endif

/** Plugin supported languages */
enum cl_plugin_type {
    CL_PLUGIN_UNKNOWN = 0,
    CL_PLUGIN_ELF     = (1 << 0),   /* C, C++, Rust and Go */
    CL_PLUGIN_PYTHON  = (1 << 1),
    CL_PLUGIN_JAVA    = (1 << 2)
};

/**
 * @name cl_plugin_load
 * @brief Loads a plugin to the memory.
 *
 * The function will load a plugin to memory, pulling information from it. If
 * the plugin is a C or C++ plugin, it will use an internal information structure
 * to load some information, otherwise it will use a specific plugin API (either
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
 * @brief Gets a pointer to an object representing all information a plugin
 *        may have.
 *
 * @param [in] cpl: The loaded plugin object.
 *
 * @return On success returns a cl_plugin_info_t object with all plugin
 *         information or NULL otherwise.
 */
cl_plugin_info_t *cl_plugin_info(const cl_plugin_t *cpl);

/**
 * @name cl_plugin_info_from_file
 * @brief Gets a pointer to an object representing all information a plugin
 *        may have, directly from its file, without loading it.
 *
 * @param [in] pathname: The complete path and name to the plugin.
 *
 * @return On success returns a cl_plugin_info_t object with all plugin
 *         information or NULL otherwise.
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
 * @name cl_plugin_call_ex
 * @brief Makes a call to a function inside a plugin.
 *
 * This function is responsible for making the call of a function inside a
 * plugin. Which is made like this:
 *
 * cl_plugin_call(cpl, "foo", CL_VOID, "arg1", CL_INT, 42,
 *                                     "arg2", CL_STRING, "bar",
 *                                     NULL);
 *
 * Notice that, every argument must have at least 3 variables, one representing
 * its name, the other its type and the third one its value. These arguments
 * passing should follow the same arguments passing to the function
 * cl_plugin_set, from the cl_plugin_t's API.
 *
 * It is recomended to use cl_plugin_call macro instead of a direct call
 * to this function, since you need to know the exactly number of arguments
 * to pass in @argc and the macro already will do this for you.
 *
 * @param [in] argc: The total number of arguments passed to the function.
 * @param [in] cpl: The cl_plugin_t object from the loaded plugin.
 * @param [in] function_name: The function name which will be called.
 * @param [in] return_type: The function return type.
 * @param [in] ...: Function arguments.
 *
 * @return On success returns a cl_plugin_t with the called function return
 *         object or NULL otherwise.
 */
cl_plugin_t *cl_plugin_call_ex(int argc, cl_plugin_t *cpl,
                               const char *function_name,
                               enum cl_type return_type, ...);

/**
 * @name cl_plugin_argument_char
 * @brief Gets a char argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
char cl_plugin_argument_char(cl_plugin_arg_t *args,
                             const char *argument_name);

/**
 * @name cl_plugin_argument_uchar
 * @brief Gets an unsigned char argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
unsigned char cl_plugin_argument_uchar(cl_plugin_arg_t *args,
                                       const char *argument_name);

/**
 * @name cl_plugin_argument_int
 * @brief Gets an int argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
int cl_plugin_argument_int(cl_plugin_arg_t *args,
                           const char *argument_name);

/**
 * @name cl_plugin_argument_uint
 * @brief Gets an unsigned int argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
unsigned int cl_plugin_argument_uint(cl_plugin_arg_t *args,
                                     const char *argument_name);

/**
 * @name cl_plugin_argument_sint
 * @brief Gets a short int argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
short int cl_plugin_argument_sint(cl_plugin_arg_t *args,
                                  const char *argument_name);

/**
 * @name cl_plugin_argument_usint
 * @brief Gets an unsigned short int argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
unsigned short int cl_plugin_argument_usint(cl_plugin_arg_t *args,
                                            const char *argument_name);

/**
 * @name cl_plugin_argument_long
 * @brief Gets a long argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
long cl_plugin_argument_long(cl_plugin_arg_t *args,
                             const char *argument_name);

/**
 * @name cl_plugin_argument_ulong
 * @brief Gets an unsigned long argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
unsigned long cl_plugin_argument_ulong(cl_plugin_arg_t *args,
                                       const char *argument_name);

/**
 * @name cl_plugin_argument_llong
 * @brief Gets a long long argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
long long cl_plugin_argument_llong(cl_plugin_arg_t *args,
                                   const char *argument_name);

/**
 * @name cl_plugin_argument_ullong
 * @brief Gets an unsigned long long argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
unsigned long long cl_plugin_argument_ullong(cl_plugin_arg_t *args,
                                             const char *argument_name);

/**
 * @name cl_plugin_argument_float
 * @brief Gets a float argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
float cl_plugin_argument_float(cl_plugin_arg_t *args,
                               const char *argument_name);

/**
 * @name cl_plugin_argument_double
 * @brief Gets a double argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
double cl_plugin_argument_double(cl_plugin_arg_t *args,
                                 const char *argument_name);

/**
 * @name cl_plugin_argument_bool
 * @brief Gets a bool argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
bool cl_plugin_argument_bool(cl_plugin_arg_t *args,
                             const char *argument_name);

/**
 * @name cl_plugin_argument_string
 * @brief Gets a string (char *) argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
char *cl_plugin_argument_string(cl_plugin_arg_t *args,
                                const char *argument_name);

/**
 * @name cl_plugin_argument_cstring
 * @brief Gets a cl_string_t argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
cl_string_t *cl_plugin_argument_cstring(cl_plugin_arg_t *args,
                                        const char *argument_name);

/**
 * @name cl_plugin_argument_pointer
 * @brief Gets a pointer argument from within a plugin function.
 *
 * @param [in] args: The arguments received in the function.
 * @param [in] argument_name: The desired argument name.
 *
 * @return On success returns the required argument value or -1 otherwise.
 */
int cl_plugin_argument_pointer(cl_plugin_arg_t *args,
                               const char *argument_name, void **ptr);

/**
 * @name cl_plugin_ref
 * @brief Increases the reference count for a cl_plugin_t item.
 *
 * @param [in,out] object: The cl_plugin_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_plugin_t *cl_plugin_ref(cl_plugin_t *cpl);

/**
 * @name cl_plugin_unref
 * @brief Decreases the reference count for a cl_plugin_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] object: The cl_plugin_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_plugin_unref(cl_plugin_t *cpl);

#endif

