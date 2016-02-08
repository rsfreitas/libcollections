
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

/*
 * TODO: Substitute cplugin_internal_data_t for a cvalue_t.
 *       Rename cplugin_value_t to cplugin_return_value_t.
 */

/** Functions argument mode */
enum cplugin_arg {
    CPLUGIN_ARG_FIXED,
    CPLUGIN_ARG_VAR,
    CPLUGIN_NO_ARGS
};

/** plugin types */
/* TODO: Remove from here */
typedef struct cplugin_s            cplugin_t;
typedef struct cplugin_fdata_s      cplugin_value_t;
typedef struct cplugin_fdata_s      cplugin_arg_t;
typedef void                        cplugin_internal_data_t;
typedef struct cplugin_info_s       cplugin_info_t;

/** C/C++ plugin info */
struct cplugin_info_s {
    char                    *name;
    char                    *version;
    char                    *creator;
    char                    *description;
    cjson_t                 *api;
    cplugin_internal_data_t *(*startup)(CPLUGIN_STARTUP_ARGS);
    int                     (*shutdown)(CPLUGIN_SHUTDOWN_ARGS);
};

/** Identification of a C/C++ plugin */
struct cplugin_entry_s {
    const char                      *name;
    const char                      *version;
    const char                      *creator;
    const char                      *description;
    const char                      *api;
    cplugin_internal_data_t         *(*startup)(CPLUGIN_STARTUP_ARGS);
    int                             (*shutdown)(CPLUGIN_SHUTDOWN_ARGS);
};

/**
 * @name cplugin_get_arg_c
 * @brief Gets current argument value of 'char' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_c instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'char' value of the argument or -1 otherwise.
 */
char cplugin_get_arg_c(const cplugin_arg_t *args, const char *arg_name);

/**
 * @name cplugin_get_arg_i
 * @brief Gets current argument value of 'int' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_i instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'int' value of the argument or -1 otherwise.
 */
int cplugin_get_arg_i(const cplugin_arg_t *args, const char *arg_name);

/**
 * @name cplugin_get_arg_p
 * @brief Gets current argument value of 'void *' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_p instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'void *' value of the argument or NULL
 *         otherwise.
 */
void *cplugin_get_arg_p(const cplugin_arg_t *args, const char *arg_name);

/**
 * @name cplugin_get_arg_f
 * @brief Gets current argument value of 'float' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_f instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'float' value of the argument or -1 otherwise.
 */
float cplugin_get_arg_f(const cplugin_arg_t *args, const char *arg_name);

/**
 * @name cplugin_arg_count
 * @brief Gets the number of arguments of a function.
 *
 * It is recomended to use CPLUGIN_ARG_COUNTinstead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 *
 * @return On success returns the function number of arguments or -1
 *         otherwise.
 */
int cplugin_arg_count(const cplugin_arg_t *args);

cplugin_value_t *cplugin_call_ex(int argc, cplugin_t *cpl,
                                 const char *function_name, ...);

int cplugin_set_return_value(cplugin_t *cpl, const char *function_name,
                             unsigned int caller_id, enum cl_type type,
                             ...);

char cplugin_get_value_c(cplugin_value_t *v);
int cplugin_get_value_i(cplugin_value_t *v);
void *cplugin_get_value_p(cplugin_value_t *v);
float cplugin_get_value_f(cplugin_value_t *v);
int cplugin_destroy_value(cplugin_value_t *v);

#endif

