
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Jan 29 11:10:45 2016
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

#ifndef _COLLECTIONS_PLUGIN_H
#define _COLLECTIONS_PLUGIN_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <plugin.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _STDINT_H
# include <stdint.h>
#endif

#ifndef _PTHREAD_H
# include <pthread.h>
#endif

/** Defines */

/* Maximum number of arguments in a function call */
#define MAX_ARGUMENTS                           2048

/* Default number of arguments to functions that calls plugin functions */
#define CL_PLUGIN_FOREIGN_CALL_DEF_ARGUMENTS    4

/*
 * Number of informations of arguments passed by the user while calling a plugin
 * function.
 *
 *  1 - The argument name
 *  2 - The argument type
 *  3 - The argument value
 */
#define CL_FOREIGN_CALL_USER_ARGUMENT_INFO      3

enum cl_plugin_info {
    CL_PLUGIN_INFO_NAME,
    CL_PLUGIN_INFO_VERSION,
    CL_PLUGIN_INFO_DESCRIPTION,
    CL_PLUGIN_INFO_FUNCTION_LIST,
    CL_PLUGIN_INFO_FUNCTION_ARG_LIST,
    CL_PLUGIN_INFO_AUTHOR
};

/** Structures */

struct cplugin_function_s;

// TODO: To be removed
struct function_argument {
    char                *jargs;
    void                *ptr;
};

struct dl_plugin_driver {
    enum cl_plugin_type type;
    bool                enabled;
    bool                (*plugin_test)(const cl_string_t *);
    void                *(*library_init)(void);
    void                (*library_uninit)(void *);
    cl_plugin_info_t    *(*load_info)(void *, void *);
    void                *(*open)(void *, const char *);
    int                 (*close)(void *, void *);
    cl_object_t         *(*call)(void *, struct cplugin_function_s *,
                                 cl_plugin_t *);

    int                 (*plugin_startup)(void *, void *, cl_plugin_info_t *);
    int                 (*plugin_shutdown)(void *, void *, cl_plugin_info_t *);
    int                 (*load_function)(void *, void *,
                                         struct cplugin_function_s *);

    /* Plugin driver custom data */
    void                *data;
};

/*
 * A structure to hold informations about a function argument so we use it
 * while looking for it on some platforms, such as those which have methods
 * signature.
 */
struct cl_arg_type {
    enum cl_type    type;
    struct cl_ref_s ref;
};

/* A structure to describe a plugin function */
struct cplugin_function_s {
    /* Function name */
    char                        *name;

    /* Function type and return value */
    enum cl_type                return_value;

    /*
     * Here we deal with the function arguments and its respective values where
     * this hashtable stores all arguments received in the original call as a
     * [argument_name]value. It's good to remember here that every value will
     * live inside a cl_object_t.
     */
    cl_hashtable_t              *arguments;

    /*
     * We algo hold a list with all arguments type, so drivers which need to
     * know them to build the function's signature to find it.
     */
    cl_list_t                   *arg_types;

    /* Pointer to the real function */
    void                        *symbol;
};

/* The plugin structure */
#define cplugin_members                             \
    cl_struct_member(enum cl_plugin_type, type)     \
    cl_struct_member(void *, handle)                \
    cl_struct_member(cl_plugin_info_t *, info)      \
    cl_struct_member(struct dl_plugin_driver *, dl) \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cplugin_s, cplugin_members);
#define cplugin_s           cl_struct(cplugin_s)

/*
 * A structure to hold every mandatory function that a plugin must have.
 * That way we can call them and store their results.
 */
struct plugin_internal_function {
    char    *name;
    char    *return_value;
};

/* dl.c */
void dl_enable_plugin_types(enum cl_plugin_type types);
struct dl_plugin_driver *dl_get_plugin_driver(const char *pathname);
void *dl_open(struct dl_plugin_driver *drv, const char *pathname);
int dl_close(struct dl_plugin_driver *drv, void *handle);
cl_plugin_info_t *dl_load_info(struct dl_plugin_driver *drv, void *handle);
cl_object_t * dl_call(cplugin_s *cpl, struct cplugin_function_s *foo);
int dl_plugin_shutdown(cplugin_s *cpl);
int dl_plugin_startup(struct dl_plugin_driver *drv, void *handle,
                      cl_plugin_info_t *info);

struct cplugin_function_s *dl_load_function(cplugin_s *cpl, const char *name,
                                            enum cl_type return_type,
                                            int argc, va_list ap);

/* info.c */
cl_plugin_info_t *info_ref(cl_plugin_info_t *info);
void info_unref(cl_plugin_info_t *info);
cl_json_t *info_get_api(const cl_plugin_info_t *info);
char *info_get_name(const cl_plugin_info_t *info);
char *info_get_version(const cl_plugin_info_t *info);
char *info_get_description(const cl_plugin_info_t *info);
char *info_get_author(const cl_plugin_info_t *info);
cl_plugin_info_t *info_create_from_data(const char *name, const char *version,
                                        const char *author,
                                        const char *description);

void info_set_custom_data(cl_plugin_info_t *info, void *ptr);
void *info_get_custom_data(cl_plugin_info_t *info);

/* utils.c */
struct cl_arg_type *new_arg_type(enum cl_type type);
cplugin_s *new_cplugin_s(void);
int destroy_cplugin_s(cplugin_s *cpl);

struct cplugin_function_s *new_cplugin_function_s(const char *name,
                                                  enum cl_type return_value);

void destroy_cplugin_function_s_list(struct cplugin_function_s *flist);
struct cplugin_list_s *new_cplugin_list_s(const char *name, enum cl_type type);
void destroy_cplugin_list_s(struct cplugin_list_s *l);
struct cplugin_entry_s *new_cplugin_entry_s(void);
void destroy_cplugin_entry_s(struct cplugin_entry_s *e);
void destroy_cplugin_function_s(void *a);

/* arguments.c */
int arguments_parse(struct cplugin_function_s *foo, int argc, va_list ap);

#endif

