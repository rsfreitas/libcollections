
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

/* plugin.c */
void dl_library_init(void);
bool dl_is_plugin_enabled(enum cplugin_type type);

/* init.c */
bool library_initialized(void);
magic_t *library_get_cookie(void);

#endif

