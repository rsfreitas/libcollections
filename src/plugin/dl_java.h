
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Apr 18 13:22:39 2016
 * Project: libcollections
 *
 * Cojniright (C) 2015 Rodrigo Freitas
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
 * You should have received a cojni of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef _COLLECTIONS_DL_JAVA_H
#define _COLLECTIONS_DL_JAVA_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <dl_java.h> directly; include <collections.h> instead."
# endif
#endif

void *jni_library_init(void);
void jni_library_uninit(void *data);
cplugin_info_t *jni_load_info(void *data, void *ptr);
int jni_load_functions(void *data, struct cplugin_function_s *flist,
                       void *handle);

void *jni_open(void *data, const char *pathname);
int jni_close(void *data, void *ptr);
cobject_t *jni_call(void *data, struct cplugin_function_s *foo,
                    cplugin_t *cpl, struct function_argument *args);

int jni_plugin_startup(void *data, void *handle, cplugin_info_t *info);
int jni_plugin_shutdown(void *data, void *handle, cplugin_info_t *info);
bool jni_plugin_test(const cstring_t *mime);

#endif

