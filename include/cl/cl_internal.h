
/*
 * Description: Internal declarations for libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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

#ifndef _COLLECTIONS_INTERNAL_H
#define _COLLECTIONS_INTERNAL_H         1

/*
 * An internal representation of a public function. It does not affect the code
 * or the function visibility. Its objective is only to let clear what is and
 * what is not been exported from library by looking at the code.
 *
 * Every exported function must have this at the beginning of its declaration.
 * Example:
 *
 * __PUB_API__ const char *function(int arg)
 * {
 *      // Body
 * }
 */
#define __PUB_API__

#include <cl/cl_typeof.h>

/*
 * This macro is responsible to do internal library validations, such as
 * clearing the internal error code, checking if the library environment
 * was initiated and validating a specific object.
 *
 * Every exported function must call this macro at the begining.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 * @return_value: Since this macro may be called from within any exported
 *                function, its type of return value may vary. Here we
 *                tell what will be this value.
 */
#define __clib_function_init__(obj_validation, object, type, return_value)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return return_value;\
\
        if (obj_validation == true)\
            if (typeof_validate_object(object, type) == false)\
                return return_value;\
    } while (0);

/*
 * This a copy of the previous macro with a slightly modification.
 *
 * Every exported function that needs to validate an object passed as
 * arguments with an offset in its header must call this macro at the begining.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 * @offset: The object's offset to access its type.
 * @return_value: Since this macro may be called from within any exported
 *                function, its type of return value may vary. Here we
 *                tell what will be this value.
 */
#define __clib_function_init_ex__(obj_validation, object, type, offset,\
        return_value)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return return_value;\
\
        if (obj_validation == true)\
            if (typeof_validate_object_with_offset(object, type, offset) == false)\
                return return_value;\
    } while (0);

/*
 * Another variation of the previous macro. Here we don't have a return value.
 * This should be used from within void functions.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 */
#define __clib_function_init_ex2__(obj_validation, object, type)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return;\
\
        if (obj_validation == true)\
            if (typeof_validate_object(object, type) == false)\
                return;\
    } while (0);

/* error.c */
void cerrno_clear(void);
void cset_errno(enum cl_error_code error_code);

/* value.c */
bool cl_object_is_valid(enum cl_type type);
void cl_object_set_char(cl_object_t *value, char c);
void cl_object_set_uchar(cl_object_t *value, unsigned char uc);
void cl_object_set_int(cl_object_t *value, int i);
void cl_object_set_uint(cl_object_t *value, unsigned int ui);
void cl_object_set_sint(cl_object_t *value, short int si);
void cl_object_set_usint(cl_object_t *value, unsigned short int usi);
void cl_object_set_float(cl_object_t *value, float f);
void cl_object_set_double(cl_object_t *value, double d);
void cl_object_set_long(cl_object_t *value, long l);
void cl_object_set_ulong(cl_object_t *value, unsigned long ul);
void cl_object_set_llong(cl_object_t *value, long long ll);
void cl_object_set_ullong(cl_object_t *value, unsigned long long ull);
void cl_object_set_boolean(cl_object_t *value, bool b);
void cl_object_set_string(cl_object_t *value, char *s);
void cl_object_set_cstring(cl_object_t *value, cl_string_t *s);
void cl_object_set_pointer(cl_object_t *object, bool dup_data, void *data,
                           unsigned int size, void (*free_object)(void *));

/* utils.c */
char *value_to_hex(void *p, unsigned int size);
char *strip_filename(const char *pathname);
char *file_extension(const char *pathname);

/* plugin.c */
void dl_library_init(void);
void dl_library_uninit(void);
bool dl_is_plugin_enabled(enum cl_plugin_type type);

/* init.c */
bool library_initialized(void);
struct random_data *library_random_data(void);
cl_json_t *library_configuration(void);
const char *library_package_name(void);
char *library_file_mime_type(const char *filename);
char *library_buffer_mime_type(const unsigned char *buffer, unsigned int size);

/* glist.c */
void *cglist_node_ref(void *node, enum cl_object object);
int cglist_node_unref(void *node, enum cl_object object);
void *cglist_node_content(const void *node, enum cl_object object);
void *cglist_ref(void *list, enum cl_object object);
int cglist_unref(void *list, enum cl_object object);
void *cglist_create(enum cl_object object, void (*free_data)(void *),
                    int (*compare_to)(void *, void *),
                    int (*filter)(void *, void *),
                    int (*equals)(void *, void *));

int cglist_destroy(void *list, enum cl_object object);
int cglist_size(const void *list, enum cl_object object);
int cglist_push(void *list, enum cl_object object, const void *node_content,
                unsigned int size, enum cl_object node_object);

void *cglist_pop(void *list, enum cl_object object);
void *cglist_shift(void *list, enum cl_object object);
int cglist_unshift(void *list, enum cl_object object, const void *node_content,
                   unsigned int size, enum cl_object node_object);

void *cglist_map(const void *list, enum cl_object object,
                 enum cl_object node_object, int (*foo)(void *, void *),
                 void *data);

void *cglist_map_indexed(const void *list, enum cl_object object,
                         enum cl_object node_object,
                         int (*foo)(unsigned int, void *, void *),
                         void *data);

void *cglist_map_reverse(const void *list, enum cl_object object,
                         enum cl_object node_object,
                         int (*foo)(void *, void *), void *data);

void *cglist_map_reverse_indexed(const void *list, enum cl_object object,
                                 enum cl_object node_object,
                                 int (*foo)(unsigned int, void *, void *),
                                 void *data);

void *cglist_at(const void *list, enum cl_object object,
                enum cl_object node_object, unsigned int index);

int cglist_delete(void *list, enum cl_object object, void *data);
int cglist_delete_indexed(void *list, enum cl_object object, unsigned int index);
void *cglist_move(void *list, enum cl_object object);
void *cglist_filter(void *list, enum cl_object object, void *data);
int cglist_sort(void *list, enum cl_object object);
int cglist_indexof(const void *list, enum cl_object object, void *content,
                   unsigned int size, enum cl_object node_object);

int cglist_last_indexof(const void *list, enum cl_object object, void *content,
                        unsigned int size, enum cl_object node_object);

bool cglist_contains(const void *list, enum cl_object object, void *content,
                     unsigned int size, enum cl_object node_object);

void *cglist_peek(const void *list, enum cl_object object,
                  enum cl_object node_object);

bool cglist_is_empty(const void *list, enum cl_object object);
int cglist_set_compare_to(const void *list, enum cl_object object,
                          int (*compare_to)(void *, void *));

int cglist_set_filter(const void *list, enum cl_object object,
                      int (*filter)(void *, void *));

int cglist_set_equals(const void *list, enum cl_object object,
                      int (*equals)(void *, void *));

void *cglist_middle(const void *list, enum cl_object object);
int cglist_rotate(void *list, enum cl_object object, unsigned int n);

/* random.c */
unsigned int cl_cseed(void);

/* intl.c */
int intl_start(const char *package, const char *locale_dir);

#endif

