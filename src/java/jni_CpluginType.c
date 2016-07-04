
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Jun  8 11:01:39 2016
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

#include <jni.h>

#include "collections.h"

enum cl_type CpluginType_to_cl_type(jobject *object, JNIEnv *env)
{
    jmethodID m;
    jclass c;
    int ret;

    c = (*env)->FindClass(env, "cplugin/CpluginType");

    if (NULL == c)
        return -1;

    m = (*env)->GetMethodID(env, c, "getValue", "()I");

    if (NULL == m)
        return -1;

    ret = (int)(*env)->CallIntMethod(env, *object, m);

    return ret;
}

