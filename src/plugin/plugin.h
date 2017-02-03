
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

/* Default number of arguments to 'cplugin_call_ex' function */
#define CPLUGIN_CALL_DEF_ARGUMENTS          3

enum cplugin_info {
    CPLUGIN_INFO_NAME,
    CPLUGIN_INFO_VERSION,
    CPLUGIN_INFO_DESCRIPTION,
    CPLUGIN_INFO_FUNCTION_LIST,
    CPLUGIN_INFO_FUNCTION_ARG_LIST,
    CPLUGIN_INFO_AUTHOR
};

/** Structures */

struct cplugin_function_s;

struct dl_plugin_driver {
    enum cplugin_type   type;
    bool                enabled;
    bool                (*plugin_test)(const cstring_t *);
    void                *(*library_init)(void);
    void                (*library_uninit)(void *);
    cplugin_info_t      *(*load_info)(void *, void *);
    int                 (*load_functions)(void *,
                                          struct cplugin_function_s *,
                                          void *);

    void                (*unload_functions)(void *,
                                            struct cplugin_function_s *,
                                            void *);

    void                *(*open)(void *, const char *);
    int                 (*close)(void *, void *);
    void                (*call)(void *, struct cplugin_function_s *,
                                uint32_t, cplugin_t *);

    int                 (*plugin_startup)(void *, void *,
                                          cplugin_info_t *);

    int                 (*plugin_shutdown)(void *, void *,
                                           cplugin_info_t *);

    /* Plugin driver custom data */
    void                *data;
};

struct cplugin_fdata_s {
    clist_entry_t       *prev;
    clist_entry_t       *next;
    struct cobject_hdr  hdr;
    char                *name;
    enum cl_type        type;
    cobject_t           *value;
    uint32_t            caller_id;
};

#define CPLUGIN_ARG_OBJECT_OFFSET   \
    (sizeof(clist_entry_t *) + sizeof(clist_entry_t *))

struct cplugin_function_s {
    clist_entry_t               *prev;
    clist_entry_t               *next;

    /* Function name */
    char                        *name;

    /* Function type and return value */
    enum cl_type                return_value;
    struct cplugin_fdata_s      *values;
    pthread_mutex_t             m_return_value;

    /* Function arguments */
    enum cplugin_arg            type_of_args;
    struct cplugin_fdata_s      *args;

    /* Pointer to the real function */
    void                        *symbol;
};

#define cplugin_members                                         \
    cl_struct_member(enum cplugin_type, type)                   \
    cl_struct_member(void *, handle)                            \
    cl_struct_member(struct cplugin_function_s *, functions)    \
    cl_struct_member(cplugin_info_t *, info)                    \
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
cjson_t *api_load(const char *api_data);
void api_unload(cjson_t *api);
struct cplugin_function_s *api_parse(cplugin_info_t *info);
cstring_t *api_to_cstring(cjson_t *api);
cstring_list_t *api_functions(const cplugin_info_t *info);
cstring_list_t *api_function_arguments(const cplugin_info_t *info,
                                       const char *function_name);

enum cl_type api_function_return_type(const cplugin_info_t *info,
                                      const char *function_name);

enum cplugin_arg api_function_arg_mode(const cplugin_info_t *info,
                                       const char *function_name);

enum cl_type api_function_arg_type(const cplugin_info_t *info,
                                   const char *function_name,
                                   const char *argument_name);

/* call.c */
int adjust_arguments(struct cplugin_function_s *foo, int argc, va_list ap);

/* dl.c */
void dl_enable_plugin_types(enum cplugin_type types);
struct dl_plugin_driver *dl_get_plugin_driver(const char *pathname);
void *dl_open(struct dl_plugin_driver *drv, const char *pathname);
int dl_close(struct dl_plugin_driver *drv, void *handle);
int dl_load_functions(struct dl_plugin_driver *drv,
                      struct cplugin_function_s *flist, void *handle);

void dl_unload_functions(cplugin_s *cpl);
cplugin_info_t *dl_load_info(struct dl_plugin_driver *drv, void *handle);
void dl_call(cplugin_s *cpl, struct cplugin_function_s *foo,
             uint32_t caller_id);

int dl_plugin_shutdown(cplugin_s *cpl);
int dl_plugin_startup(struct dl_plugin_driver *drv, void *handle,
                      cplugin_info_t *info);

/* dl_java.c */
cstring_t *type_to_jni_type(enum cl_type type);

/* info.c */
cplugin_info_t *info_ref(cplugin_info_t *info);
void info_unref(cplugin_info_t *info);
cjson_t *info_get_api(const cplugin_info_t *info);
char *info_get_name(const cplugin_info_t *info);
char *info_get_version(const cplugin_info_t *info);
char *info_get_description(const cplugin_info_t *info);
char *info_get_author(const cplugin_info_t *info);
cplugin_info_t *info_create_from_data(const char *name, const char *version,
                                      const char *author, const char *description,
                                      const char *api);

cplugin_info_t *info_create_from_entry(struct cplugin_entry_s *entry);
void info_set_custom_data(cplugin_info_t *info, void *ptr);
void *info_get_custom_data(cplugin_info_t *info);

/* plugin_misc.c */
struct cplugin_fdata_s *new_cplugin_fdata_s(const char *name, enum cl_type type,
                                            uint32_t caller_id);

void destroy_cplugin_fdata_s(void *a);
cplugin_s *new_cplugin_s(void);
int destroy_cplugin_s(cplugin_s *cpl);

struct cplugin_function_s *new_cplugin_function_s(const char *name,
                                                  enum cl_type return_value,
                                                  enum cplugin_arg arg_type,
                                                  struct cplugin_fdata_s *args);

void destroy_cplugin_function_s_list(struct cplugin_function_s *flist);
int search_cplugin_function_s(void *a, void *b);
int search_cplugin_fdata_s(void *a, void *b);
int search_cplugin_fdata_s_by_caller_id(void *a, void *b);
struct cplugin_list_s *new_cplugin_list_s(const char *name, enum cl_type type);

void destroy_cplugin_list_s(struct cplugin_list_s *l);
uint32_t random_caller_id(struct cplugin_function_s *foo);

struct cplugin_entry_s *new_cplugin_entry_s(void);
void destroy_cplugin_entry_s(struct cplugin_entry_s *e);

/* rv.c */
cobject_t *cplugin_get_return_value(cplugin_s *cpl,
                                    const char *function_name,
                                    uint32_t caller_id);

#endif

