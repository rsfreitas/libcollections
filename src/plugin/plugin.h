
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

/* Plugin languages */
enum cplugin_plugin_type {
    CPLUGIN_UNKNOWN,
    CPLUGIN_C,              /* C or C++ */
    CPLUGIN_PYTHON          /* Python */
};

enum cplugin_info {
    CPLUGIN_INFO_NAME,
    CPLUGIN_INFO_VERSION,
    CPLUGIN_INFO_DESCRIPTION,
    CPLUGIN_INFO_FUNCTION_LIST,
    CPLUGIN_INFO_FUNCTION_ARG_LIST,
    CPLUGIN_INFO_AUTHOR
};

/** Structures */

struct cplugin_fdata_s {
    clist_t         *prev;
    clist_t         *next;

    char            *name;
    enum cl_type    type;
    cvalue_t        *value;
    uint32_t        caller_id;
};

struct cplugin_function_s {
    clist_t                     *prev;
    clist_t                     *next;

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

struct cplugin_s {
    /* Plugin */
    enum cplugin_plugin_type        type;
    void                            *handle;

    /* Plugin functions list */
    struct cplugin_function_s       *functions;

    /* Plugin information */
    cplugin_info_t                  *info;

    /* Optional startup and shutdown plugin functions */
    int                             (*startup)(void);
    void                            (*shutdown)(void);
};

/* api_parser.c */
cjson_t *api_load(const char *api_data);
void api_unload(cjson_t *api);
struct cplugin_function_s *api_parse(cplugin_info_t *info);
cstring_t *api_to_string(cjson_t *api);
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

/* dl_c.c */
void c_library_init(void);
void c_library_uninit(void);
cplugin_info_t *c_load_info(void *handle);
int c_load_functions(struct cplugin_function_s *flist, void *handle);
void *c_open(const char *pathname);
int c_close(void *handle);
void c_call(struct cplugin_function_s *foo, uint32_t caller_id,
            struct cplugin_s *cpl);

int c_plugin_startup(void *handle);
int c_plugin_shutdown(cplugin_info_t *info);

/* dl_python.c */
void py_library_init(void);
void py_library_uninit(void);
cplugin_info_t *py_load_info(void *ptr);
int py_load_functions(struct cplugin_function_s *flist, void *handle);
void *py_open(const char *pathname);
int py_close(void *ptr);
void py_call(struct cplugin_function_s *foo, uint32_t caller_id,
             struct cplugin_s *cpl);

int py_plugin_startup(void *handle, cplugin_info_t *info);
int py_plugin_shutdown(void *handle, cplugin_info_t *info);

/* dl.c */
enum cplugin_plugin_type dl_get_plugin_type(const char *pathname);
void *dl_open(const char *pathname, enum cplugin_plugin_type plugin_type);
int dl_close(void *handle, enum cplugin_plugin_type plugin_type);
int dl_load_functions(struct cplugin_function_s *flist, void *handle,
                      enum cplugin_plugin_type plugin_type);

cplugin_info_t *dl_load_info(void *handle, enum cplugin_plugin_type plugin_type);
void dl_call(struct cplugin_function_s *foo, uint32_t caller_id,
             struct cplugin_s *cpl);

int dl_plugin_startup(void *handle, enum cplugin_plugin_type plugin_type,
                      cplugin_info_t *info);

int dl_plugin_shutdown(struct cplugin_s *cpl);

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
struct cplugin_s *new_cplugin_s(void);
int destroy_cplugin_s(struct cplugin_s *cpl);

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
cvalue_t *cplugin_get_return_value(struct cplugin_s *cpl,
                                   const char *function_name,
                                   uint32_t caller_id);

#endif

