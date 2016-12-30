
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

#ifndef _MAGIC
# include <magic.h>
#endif

#include <stddef.h>

#ifndef LIBEXPORT
# ifdef LINUX
#  define LIBEXPORT                     __attribute__((visibility("default")))
# else
#  define LIBEXPORT
# endif
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* _cplugin.c */
/* Keys to access PyObject encapsulated info */
#define PYARGS                          "pyargs"
#define PYCPLUGIN_T                     "pycplugin_t"
#define PYSHUTDOWN_ARGS                 "pyshutargs"

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
            if (validate_object(object, type) == false)\
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
            if (validate_object_with_offset(object, type, offset) == false)\
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
            if (validate_object(object, type) == false)\
                return;\
    } while (0);

/* error.c */
void cerrno_clear(void);
void cset_errno(enum cerror_code error_code);

/* ref.c */
struct ref_s {
    void    (*free)(const struct ref_s *);
    int     count;
};

inline void ref_inc(const struct ref_s *ref);
inline void ref_dec(const struct ref_s *ref);
inline bool ref_bool_compare(const struct ref_s *ref, int old, int new);

/* value.c */
bool validate_cl_type(enum cl_type type);
void cobject_set_char(cobject_t *value, char c);
void cobject_set_uchar(cobject_t *value, unsigned char uc);
void cobject_set_int(cobject_t *value, int i);
void cobject_set_uint(cobject_t *value, unsigned int ui);
void cobject_set_sint(cobject_t *value, short int si);
void cobject_set_usint(cobject_t *value, unsigned short int usi);
void cobject_set_float(cobject_t *value, float f);
void cobject_set_double(cobject_t *value, double d);
void cobject_set_long(cobject_t *value, long l);
void cobject_set_ulong(cobject_t *value, unsigned long ul);
void cobject_set_llong(cobject_t *value, long long ll);
void cobject_set_ullong(cobject_t *value, unsigned long long ull);
void cobject_set_boolean(cobject_t *value, bool b);
void cobject_set_string(cobject_t *value, char *s);
void cobject_set_cstring(cobject_t *value, cstring_t *s);

/* util.c */
char *value_to_hex(void *p, unsigned int size);
char *strip_filename(const char *pathname);
char *file_extension(const char *pathname);

/* plugin.c */
void dl_library_init(void);
bool dl_is_plugin_enabled(enum cplugin_type type);

/* init.c */
bool library_initialized(void);
magic_t *library_get_cookie(void);
struct random_data *library_random_data(void);

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
                 int (*foo)(void *, void *), void *data);

void *cglist_map_indexed(const void *list, enum cl_object object,
                         int (*foo)(unsigned int, void *, void *),
                         void *data);

void *cglist_map_reverse(const void *list, enum cl_object object,
                         int (*foo)(void *, void *), void *data);

void *cglist_map_reverse_indexed(const void *list, enum cl_object object,
                                 int (*foo)(unsigned int, void *, void *),
                                 void *data);

void *cglist_at(const void *list, enum cl_object object, unsigned int index);
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

/* random.c */
unsigned int cl_cseed(void);

#endif

