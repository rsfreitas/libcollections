
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Jun  8 13:47:03 2016
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

#ifndef _COLLECTIONS_JNI_CPLUGIN_H
#define _COLLECTIONS_JNI_CPLUGIN_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <jni_cplugin.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _JAVASOFT_JNI_H_
# include <jni.h>
#endif

#ifndef _COLLECTIONS_PLUGIN_H
# include "../plugin.h"
#endif

/* jni_CpluginArguments.c */
jobject newCpluginArguments(JNIEnv *env, struct cplugin_function_s *foo);

/* jni_CpluginObject.c */
int set_return_value_from_CpluginObject(JNIEnv *env, cplugin_t *cpl,
                                        const char *function_name,
                                        uint32_t caller_id,
                                        jobject *object);

/* jni_CpluginType.c */
enum cl_type CpluginType_to_cl_type(jobject *object, JNIEnv *env);

#endif

