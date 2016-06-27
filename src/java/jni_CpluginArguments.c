
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
#include <string.h>
#include <stdlib.h>

#include "collections.h"
#include "../plugin/plugin.h"

struct tmp_s {
    JNIEnv      *env;
    jobject     o;
    jclass      c;
};

static void fill_method_argument_value(JNIEnv *env, struct cplugin_fdata_s *arg,
    jvalue *jarg)
{
    switch (arg->type) {
        case CL_VOID:
            break;

        case CL_CHAR:
            jarg->b = CVALUE_AS_CHAR(arg->value);
            break;

        case CL_UCHAR:
            jarg->b = CVALUE_AS_UCHAR(arg->value);
            break;

        case CL_INT:
            jarg->i = CVALUE_AS_INT(arg->value);
            break;

        case CL_UINT:
            jarg->i = CVALUE_AS_UINT(arg->value);
            break;

        case CL_LONG:
            jarg->i = CVALUE_AS_LONG(arg->value);
            break;

        case CL_ULONG:
            jarg->i = CVALUE_AS_ULONG(arg->value);
            break;

        case CL_SINT:
            jarg->s = CVALUE_AS_SINT(arg->value);
            break;

        case CL_USINT:
            jarg->s = CVALUE_AS_USINT(arg->value);
            break;

        case CL_FLOAT:
            jarg->f = CVALUE_AS_FLOAT(arg->value);
            break;

        case CL_DOUBLE:
            jarg->d = CVALUE_AS_DOUBLE(arg->value);
            break;

        case CL_LLONG:
            jarg->j = CVALUE_AS_LLONG(arg->value);
            break;

        case CL_ULLONG:
            jarg->j = CVALUE_AS_ULLONG(arg->value);
            break;

        case CL_POINTER:
        case CL_CSTRING:
            /* TODO */
            break;

        case CL_STRING:
            jarg->l = (*env)->NewStringUTF(env, CVALUE_AS_STRING(arg->value));
            break;

        case CL_BOOLEAN:
            jarg->z = CVALUE_AS_BOOLEAN(arg->value);
            break;
    }
}

static jmethodID addArgument_methodID(JNIEnv *env, jclass class,
    struct cplugin_fdata_s *arg)
{
    jmethodID m;
    cstring_t *signature = cstring_create("(Ljava/lang/String;"), *arg_s;

    arg_s = type_to_jni_type(arg->type);
    cstring_cat(signature, "%s)V", cstring_valueof(arg_s));
    cstring_unref(arg_s);

    /* get addArgument method reference */
    m = (*env)->GetMethodID(env, class, "addArgument",
                            cstring_valueof(signature));

    cstring_unref(signature);

    return m;
}

static int add_argument(void *a, void *b)
{
    struct cplugin_fdata_s *arg = (struct cplugin_fdata_s *)a;
    struct tmp_s *tmp = (struct tmp_s *)b;
    jvalue args[2];
    jmethodID m;

    m = addArgument_methodID(tmp->env, tmp->c, arg);

    if (NULL == m)
        return -1;

    /* call addArgument(name, value) */
    args[0].l = (*tmp->env)->NewStringUTF(tmp->env, arg->name);
    fill_method_argument_value(tmp->env, arg, &args[1]);
    (*tmp->env)->CallObjectMethodA(tmp->env, tmp->o, m, args);

    return 0;
}

jobject newCpluginArguments(JNIEnv *env, struct cplugin_function_s *foo)
{
    jclass c;
    jmethodID constructor;
    jobject o;
    struct tmp_s arg;

    c = (*env)->FindClass(env, "cplugin/CpluginArguments");

    if (NULL == c)
        return NULL;

    constructor = (*env)->GetMethodID(env, c, "<init>", "()V");

    if (NULL == constructor)
        return NULL;

    /* Call CpluginArguments constructor */
    o = (*env)->NewObject(env, c, constructor);

    /* Fill arguments and values */
    arg.env = env;
    arg.o = o;
    arg.c = c;
    cdll_map(foo->args, add_argument, &arg);

    return o;
}

