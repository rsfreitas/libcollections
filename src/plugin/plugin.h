
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

/* Supported plugin languages */
enum cplugin_plugin_type {
    CPLUGIN_C               /* C or C++ */
};

enum cplugin_info {
    CPLUGIN_INFO_NAME,
    CPLUGIN_INFO_VERSION,
    CPLUGIN_INFO_DESCRIPTION,
    CPLUGIN_INFO_FUNCTION_LIST,
    CPLUGIN_INFO_FUNCTION_ARG_LIST,
    CPLUGIN_INFO_CREATOR
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

    /* Pointers to the real function */
    void                        (*p)(uint32_t, cplugin_t *, cplugin_arg_t *);
    void                        *p_ex;
};

struct cplugin_s {
    /* Plugin */
    enum cplugin_plugin_type        type;
    void                            *handle;

    /* Plugin functions list */
    struct cplugin_function_s       *functions;

    /* Plugin information */
    struct cplugin_info_s           *info;

    /* Plugin custom internal data */
    cplugin_internal_data_t         *idata;
};

/* api_parser.c */
cjson_t *api_load(const char *api_data);
void api_unload(cjson_t *api);
struct cplugin_function_s *api_parse(cjson_t *api);

/* call.c */
int adjust_arguments(struct cplugin_function_s *foo, int argc, va_list ap);

/* dl_c.c */
void c_library_init(void);
void c_library_uninit(void);
struct cplugin_info_s *c_dl_load_info(void *handle);
int c_load_functions(struct cplugin_function_s *flist, void *handle);
void *c_dl_open(const char *pathname);
int c_dl_close(void *handle);
void c_dl_call(struct cplugin_function_s *foo, uint32_t caller_id,
               struct cplugin_s *cpl);

cplugin_internal_data_t *c_plugin_startup(struct cplugin_info_s *info);
int c_plugin_shutdown(cplugin_internal_data_t *plugin_idata,
                      struct cplugin_info_s *info);

/* dl.c */
void *dl_open(const char *pathname, enum cplugin_plugin_type plugin_type);
int dl_close(void *handle, enum cplugin_plugin_type plugin_type);
int dl_load_functions(struct cplugin_function_s *flist, void *handle,
                      enum cplugin_plugin_type plugin_type);

struct cplugin_info_s *dl_load_info(void *handle,
                                    enum cplugin_plugin_type plugin_type);

void dl_call(struct cplugin_function_s *foo, uint32_t caller_id,
             struct cplugin_s *cpl);

cplugin_internal_data_t *dl_plugin_startup(void *handle,
                                           enum cplugin_plugin_type plugin_type,
                                           struct cplugin_info_s *info);

int dl_plugin_shutdown(struct cplugin_s *cpl);

/* info.c */
struct cplugin_info_s *info_create_from_data(const char *name,
                                             const char *version,
                                             const char *creator,
                                             const char *description,
                                             const char *api);

struct cplugin_info_s *info_create_from_entry(struct cplugin_entry_s *entry);
char *get_info(enum cplugin_info info, struct cplugin_s *cpl,
               const char *function_name, char token);

cjson_t *get_function_object(cjson_t *api, const char *function_name);

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
struct cplugin_list_s *new_cplugin_list_s(const char *name,
                                          enum cl_type type);

void destroy_cplugin_list_s(struct cplugin_list_s *l);
struct cplugin_info_s *new_cplugin_info_s(const char *name, const char *version,
                                          const char *description,
                                          const char *creator);

void destroy_cplugin_info_s(struct cplugin_info_s *info);
uint32_t random_caller_id(struct cplugin_function_s *foo);
enum cplugin_plugin_type guess_plugin_type(const char *pathname);

struct cplugin_entry_s *new_cplugin_entry_s(void);
void destroy_cplugin_entry_s(struct cplugin_entry_s *e);
enum cl_type cvt_str_to_cv(const char *rv);
cvalue_t *get_arg_value(const cplugin_arg_t *arg, const char *arg_name);

/* rv.c */
cplugin_value_t *cplugin_get_return_value(cplugin_t *cpl,
                                          const char *function_name,
                                          uint32_t caller_id);
#endif

