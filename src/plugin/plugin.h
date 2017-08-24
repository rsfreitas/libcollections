
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

/* Default number of arguments to 'cl_plugin_call_ex' function */
#define CL_PLUGIN_CALL_DEF_ARGUMENTS          3

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
    int                 (*load_functions)(void *,
                                          struct cplugin_function_s *,
                                          void *);

    void                (*unload_functions)(void *,
                                            struct cplugin_function_s *,
                                            void *);

    void                *(*open)(void *, const char *);
    int                 (*close)(void *, void *);
    cl_object_t         *(*call)(void *, struct cplugin_function_s *,
                                 cl_plugin_t *, struct function_argument *);

    int                 (*plugin_startup)(void *, void *,
                                          cl_plugin_info_t *);

    int                 (*plugin_shutdown)(void *, void *,
                                           cl_plugin_info_t *);

    /* Plugin driver custom data */
    void                *data;
};

struct cplugin_fdata_s {
    cl_list_entry_t         *prev;
    cl_list_entry_t         *next;
    struct cl_object_hdr    hdr;
    char                    *name;
    enum cl_type            type;
};

#define CL_PLUGIN_ARG_OBJECT_OFFSET   \
    (sizeof(cl_list_entry_t *) + sizeof(cl_list_entry_t *))

struct cplugin_function_s {
    cl_list_entry_t             *prev;
    cl_list_entry_t             *next;

    /* Function name */
    char                        *name;

    /* Function type and return value */
    enum cl_type                return_value;

    /* Function arguments */
    enum cl_plugin_arg_mode     arg_mode;
    struct cplugin_fdata_s      *args;

    /* Pointer to the real function */
    void                        *symbol;
};

#define cplugin_members                                         \
    cl_struct_member(enum cl_plugin_type, type)                 \
    cl_struct_member(void *, handle)                            \
    cl_struct_member(struct cplugin_function_s *, functions)    \
    cl_struct_member(cl_plugin_info_t *, info)                  \
    cl_struct_member(struct dl_plugin_driver *, dl)

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

/* api_parser.c */
cl_json_t *api_load(const char *api_data);
void api_unload(cl_json_t *api);
struct cplugin_function_s *api_parse(cl_plugin_info_t *info);
cl_string_t *api_to_cstring(cl_json_t *api);
cl_stringlist_t *api_functions(const cl_plugin_info_t *info);
cl_stringlist_t *api_function_arguments(const cl_plugin_info_t *info,
                                        const char *function_name);

enum cl_type api_function_return_type(const cl_plugin_info_t *info,
                                      const char *function_name);

enum cl_plugin_arg_mode api_function_arg_mode(const cl_plugin_info_t *info,
                                            const char *function_name);

enum cl_type api_function_arg_type(const cl_plugin_info_t *info,
                                   const char *function_name,
                                   const char *argument_name);

/* call.c */
int adjust_arguments(struct cplugin_function_s *foo,
                     struct function_argument *args, int argc, va_list ap);

/* dl.c */
void dl_enable_plugin_types(enum cl_plugin_type types);
struct dl_plugin_driver *dl_get_plugin_driver(const char *pathname);
void *dl_open(struct dl_plugin_driver *drv, const char *pathname);
int dl_close(struct dl_plugin_driver *drv, void *handle);
int dl_load_functions(struct dl_plugin_driver *drv,
                      struct cplugin_function_s *flist, void *handle);

void dl_unload_functions(cplugin_s *cpl);
cl_plugin_info_t *dl_load_info(struct dl_plugin_driver *drv, void *handle);
cl_object_t * dl_call(cplugin_s *cpl, struct cplugin_function_s *foo,
                    struct function_argument *args);

int dl_plugin_shutdown(cplugin_s *cpl);
int dl_plugin_startup(struct dl_plugin_driver *drv, void *handle,
                      cl_plugin_info_t *info);

/* dl_java.c */
cl_string_t *type_to_jni_type(enum cl_type type);

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
                                        const char *description,
                                        const char *api);

void info_set_custom_data(cl_plugin_info_t *info, void *ptr);
void *info_get_custom_data(cl_plugin_info_t *info);

/* plugin_misc.c */
struct cplugin_fdata_s *new_cplugin_fdata_s(const char *name, enum cl_type type);
void destroy_cplugin_fdata_s(void *a);
cplugin_s *new_cplugin_s(void);
int destroy_cplugin_s(cplugin_s *cpl);

struct cplugin_function_s *new_cplugin_function_s(const char *name,
                                                  enum cl_type return_value,
                                                  enum cl_plugin_arg_mode arg_mode,
                                                  struct cplugin_fdata_s *args);

void destroy_cplugin_function_s_list(struct cplugin_function_s *flist);
int search_cplugin_function_s(void *a, void *b);
int search_cplugin_fdata_s(void *a, void *b);
struct cplugin_list_s *new_cplugin_list_s(const char *name, enum cl_type type);
void destroy_cplugin_list_s(struct cplugin_list_s *l);

struct cplugin_entry_s *new_cplugin_entry_s(void);
void destroy_cplugin_entry_s(struct cplugin_entry_s *e);

#endif

