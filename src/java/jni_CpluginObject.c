
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
#include "../java/jni_cplugin.h"

/*
 * Here we convert the internal CpluginType field to an enum cl_type.
 *
 * XXX: Attention to the semicolon at the end of the method signature.
 */
static enum cl_type get_type_from_CpluginObject(JNIEnv *env, jclass class,
    jobject *object)
{
    jmethodID m;
    jobject jtype;

    m = (*env)->GetMethodID(env, class, "getType", "()Lcplugin/CpluginType;");

    if (NULL == m)
        return -1;

    jtype = (*env)->CallObjectMethod(env, *object, m);

    if (NULL == jtype)
        return -1;

    /* Convert jni type to type */
    return CpluginType_to_cl_type(&jtype, env);
}

static int __set_return_value_byte(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    char ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_byte", "()B");

    if (NULL == m)
        return -1;

    ret = (char)(*env)->CallByteMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_CHAR, ret);
}

static int __set_return_value_short(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    short int ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_short", "()S");

    if (NULL == m)
        return -1;

    ret = (short int)(*env)->CallShortMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_SINT, ret);
}

static int __set_return_value_int(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    int ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_int", "()I");

    if (NULL == m)
        return -1;

    ret = (int)(*env)->CallIntMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_INT, ret);
}

static int __set_return_value_long(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    long long ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_long", "()J");

    if (NULL == m)
        return -1;

    ret = (long long)(*env)->CallLongMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_LLONG, ret);
}

static int __set_return_value_float(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    float ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_float", "()F");

    if (NULL == m)
        return -1;

    ret = (float)(*env)->CallFloatMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_FLOAT, ret);
}

static int __set_return_value_double(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    double ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_double", "()D");

    if (NULL == m)
        return -1;

    ret = (double)(*env)->CallDoubleMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_DOUBLE, ret);
}

static int __set_return_value_boolean(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class)
{
    jmethodID m;
    bool ret;

    m = (*env)->GetMethodID(env, class, "getValue_as_boolean", "()Z");

    if (NULL == m)
        return -1;

    ret = (bool)(*env)->CallBooleanMethod(env, *object, m);

    return cplugin_set_return_value(cpl, function_name, caller_id,
                                    CL_BOOLEAN, ret);
}

static int __set_return_value(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, unsigned int caller_id, jobject *object,
    jclass class, enum cl_type type)
{
    int ret = -1;

    switch (type) {
        case CL_VOID:
        case CL_UCHAR:
        case CL_UINT:
        case CL_LONG:
        case CL_ULONG:
        case CL_USINT:
        case CL_ULLONG:
        case CL_POINTER:
        case CL_CSTRING:
        case CL_STRING:
            break;

        case CL_BOOLEAN:
            ret = __set_return_value_boolean(env, cpl, function_name, caller_id,
                                             object, class);

            break;

        case CL_CHAR:
            ret = __set_return_value_byte(env, cpl, function_name, caller_id,
                                          object, class);

            break;

        case CL_INT:
            ret = __set_return_value_int(env, cpl, function_name, caller_id,
                                         object, class);

            break;

        case CL_SINT:
            ret = __set_return_value_short(env, cpl, function_name, caller_id,
                                           object, class);

            break;

        case CL_FLOAT:
            ret = __set_return_value_float(env, cpl, function_name, caller_id,
                                           object, class);

            break;

        case CL_DOUBLE:
            ret = __set_return_value_double(env, cpl, function_name, caller_id,
                                            object, class);

            break;

        case CL_LLONG:
            ret = __set_return_value_long(env, cpl, function_name, caller_id,
                                          object, class);

            break;
    }

    return ret;
}

int set_return_value_from_CpluginObject(JNIEnv *env, cplugin_t *cpl,
    const char *function_name, uint32_t caller_id, jobject *object)
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

