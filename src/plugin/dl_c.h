
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Apr 12 15:14:33 2016
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

#ifndef _COLLECTIONS_DL_C_H
#define _COLLECTIONS_DL_C_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <dl_c.h> directly; include <collections.h> instead."
# endif
#endif

void c_library_init(void);
void c_library_uninit(void);
cplugin_info_t *c_load_info(void *handle);
int c_load_functions(struct cplugin_function_s *flist, void *handle);
void *c_open(const char *pathname);
int c_close(void *handle);
void c_call(struct cplugin_function_s *foo, uint32_t caller_id,
            struct cplugin_s *cpl);

int c_plugin_startup(void *handle, cplugin_info_t *info);
int c_plugin_shutdown(void *handle, cplugin_info_t *info);

#endif

