
/*
 * Description: A generic reference count for internal objects.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Jan  2 11:36:44 2016
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "collections.h"

inline void ref_inc(const struct ref_s *ref)
{
    if (NULL == ref)
        return;

    __sync_add_and_fetch((int *)&ref->count, 1);
}

inline void ref_dec(const struct ref_s *ref)
{
    if (__sync_sub_and_fetch((int *)&ref->count, 1) == 0)
        (ref->free)(ref);
}

