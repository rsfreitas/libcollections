
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
#define _COLLECTIONS_INTERNAL_H     1

#include <stddef.h>

#ifndef LIBEXPORT
# ifdef LINUX
#  define LIBEXPORT                 __attribute__((visibility("default")))
# else
#  define LIBEXPORT
# endif
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

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
void cvalue_set_char(cvalue_t *value, char c);
void cvalue_set_uchar(cvalue_t *value, unsigned char uc);
void cvalue_set_int(cvalue_t *value, int i);
void cvalue_set_uint(cvalue_t *value, unsigned int ui);
void cvalue_set_sint(cvalue_t *value, short int si);
void cvalue_set_usint(cvalue_t *value, unsigned short int usi);
void cvalue_set_float(cvalue_t *value, float f);
void cvalue_set_double(cvalue_t *value, double d);
void cvalue_set_long(cvalue_t *value, long l);
void cvalue_set_ulong(cvalue_t *value, unsigned long ul);
void cvalue_set_llong(cvalue_t *value, long long ll);
void cvalue_set_ullong(cvalue_t *value, unsigned long long ull);
void cvalue_set_boolean(cvalue_t *value, bool b);
void cvalue_set_string(cvalue_t *value, cstring_t *s);

#endif

