
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

#ifndef _COLLECTIONS_INTERNAL_GLIST_H
#define _COLLECTIONS_INTERNAL_GLIST_H

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <glist.h> directly; include <collections.h> instead."
# endif
#endif

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

#endif
