
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

#ifndef _COLLECTIONS_DL_ELF_H
#define _COLLECTIONS_DL_ELF_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <dl_elf.h> directly; include <collections.h> instead."
# endif
#endif

void *elf_library_init(void);
void elf_library_uninit(void *data);
cl_plugin_info_t *elf_load_info(void *data, void *handle);
void *elf_open(void *data, const char *pathname);
int elf_close(void *data, void *handle);
cl_object_t* elf_call(void *data, struct cplugin_function_s *foo,
                      cl_plugin_t *cpl);

int elf_plugin_startup(void *data, void *handle, cl_plugin_info_t *info);
int elf_plugin_shutdown(void *data, void *handle, cl_plugin_info_t *info);
bool elf_plugin_test(const cl_string_t *mime);
int elf_load_function(void *data, void *handle, struct cplugin_function_s *foo);

#endif

