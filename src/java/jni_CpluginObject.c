
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

/*
 * TODO: Needs to get the Java CpluginObject values and insert them into
 *       the return value.
 */
static enum cl_type get_type_from_CpluginObject(JNIEnv *env, jclass class,
    jobject *object)
{
    jmethodID m;
    jobject jtype;

    m = (*env)->GetMethodID(env, class, "getType", "()Lcplugin/CpluginType");

    if (NULL == m)
        return -1;

    jtype = (*env)->CallObjectMethod(env, object, m);

    if (NULL == jtype)
        return -1;

    /* Convert jni type to type */

    return CL_VOID;
}

static int __set_return_value(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class, enum cl_type type)
{
    int ret = -1;

    /* Get return value */

    switch (type) {
        case CL_VOID:
            break;

        case CL_CHAR:
            /* get method signature */
            /* call method */
            /* set return value */
            break;

        case CL_UCHAR:
            break;

        case CL_INT:
            break;

        case CL_UINT:
            break;

        case CL_LONG:
            break;

        case CL_ULONG:
            break;

        case CL_SINT:
            break;

        case CL_USINT:
            break;

        case CL_FLOAT:
            break;

        case CL_DOUBLE:
            break;

        case CL_LLONG:
            break;

        case CL_ULLONG:
            break;

        case CL_POINTER:
        case CL_CSTRING:
            /* TODO */
            break;

        case CL_STRING:
            break;

        case CL_BOOLEAN:
            break;
    }

    return ret;
}

int set_return_value_from_CpluginObject(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object)
{
    jclass c;
    enum cl_type type;

    c = (*env)->FindClass(env, "cplugin/CpluginObject");

    if (NULL == c)
        return -1;

    /* Get return type */
    type = get_type_from_CpluginObject(env, c, object);

    return __set_return_value(env, cpl, function_name, caller_id, object,
                              c, type);
}

