
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Mar  8 08:06:47 2018
 * Project: libcollections
 *
 * Copyright (C) 2018 Rodrigo Freitas All rights reserved.
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

#ifndef _COLLECTIONS_INTERNAL_VALUE_H
#define _COLLECTIONS_INTERNAL_VALUE_H

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <value.h> directly; include <collections.h> instead."
# endif
#endif

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

#endif
