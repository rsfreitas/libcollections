
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:34:01 BRST 2015
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "collections.h"
#include "plugin.h"

/*
 * With our custom classpath we need to able to load the 'cplugin.jar'
 * file. So plugins may correctly load their dependencies.
 */
#define CL_PLUGIN_CLASSPATH           "/usr/local/lib/cplugin.jar:/usr/local/lib"
#define MAX_JVM_OPTIONS             1

struct jdriver {
    JavaVM      *jvm;
    JNIEnv      *env;
};

struct jinfo {
    jobject     obj;
};

struct jtmp_data {
    struct jdriver  *jdriver;
    jclass          cls;
};

/*
 *
 * Internal functions
 *
 */

static void set_custom_plugin_info(cl_plugin_info_t *info, jobject obj)
{
    struct jinfo *p = NULL;

    p = calloc(1, sizeof(struct jinfo));

    if (NULL == p)
        return;

    p->obj = obj;

    info_set_custom_data(info, p);
}

static void release_cutom_plugin_info(struct jinfo *info)
{
    if (NULL == info)
        return;

    free(info);
}

static void add_arguments_signature(cl_string_t *jargs,
    struct cplugin_function_s *foo)
{
    cl_string_t *arg = NULL;

    if (foo->arg_mode & CL_PLUGIN_ARGS_COMMON) {
        arg = type_to_jni_type(CL_STRING);
        cl_string_cat(jargs, "%s", cl_string_valueof(arg));
        cl_string_unref(arg);
    }

    if (foo->arg_mode & CL_PLUGIN_ARGS_POINTER) {
        arg = type_to_jni_type(CL_POINTER);
        cl_string_cat(jargs, "%s", cl_string_valueof(arg));
        cl_string_unref(arg);
    }
}

static void add_return_value_signature(cl_string_t *jargs,
    struct cplugin_function_s *foo)
{
    cl_string_t *foo_return = NULL;

    foo_return = type_to_jni_type(foo->return_value);
    cl_string_cat(jargs, ")%s", cl_string_valueof(foo_return));
    cl_string_unref(foo_return);
}

static cl_string_t *create_method_signature(struct cplugin_function_s *foo)
{
    cl_string_t *jargs = cl_string_create("(");

    if (foo->arg_mode != CL_PLUGIN_ARGS_VOID)
        add_arguments_signature(jargs, foo);

    if (foo->return_value == CL_VOID)
        cl_string_cat(jargs, ")V");
    else
        add_return_value_signature(jargs, foo);

    return jargs;
}

static int jni_load_function(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    struct jtmp_data *jtmp = (struct jtmp_data *)b;
    struct jdriver *j = jtmp->jdriver;
    jmethodID m;
    cl_string_t *signature = NULL;

    signature = create_method_signature(foo);
    m = (*j->env)->GetMethodID(j->env, jtmp->cls, foo->name,
                               cl_string_valueof(signature));

    cl_string_unref(signature);
    foo->symbol = m;

    return 0;
}

static void jni_call_v(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        (*j->env)->CallVoidMethod(j->env, jinfo->obj, foo->symbol,
                                  jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        (*j->env)->CallVoidMethod(j->env, jinfo->obj, foo->symbol,
                                  jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        (*j->env)->CallVoidMethod(j->env, jinfo->obj, foo->symbol,
                                  ptr_arg);
    } else {
        (*j->env)->CallVoidMethod(j->env, jinfo->obj, foo->symbol);
    }
}

static char jni_call_c(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    char c;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        c = (char)(*j->env)->CallByteMethod(j->env, jinfo->obj, foo->symbol,
                                            jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        c = (char)(*j->env)->CallByteMethod(j->env, jinfo->obj, foo->symbol,
                                            jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        c = (char)(*j->env)->CallByteMethod(j->env, jinfo->obj, foo->symbol,
                                            ptr_arg);
    } else {
        c = (char)(*j->env)->CallByteMethod(j->env, jinfo->obj, foo->symbol);
    }

    return c;
}

static int jni_call_i(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    int i;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (int)(*j->env)->CallIntMethod(j->env, jinfo->obj, foo->symbol,
                                          jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (int)(*j->env)->CallIntMethod(j->env, jinfo->obj, foo->symbol,
                                          jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (int)(*j->env)->CallIntMethod(j->env, jinfo->obj, foo->symbol,
                                          ptr_arg);
    } else {
        i = (int)(*j->env)->CallIntMethod(j->env, jinfo->obj, foo->symbol);
    }

    return i;
}

static short int jni_call_si(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    short int i;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (short int)(*j->env)->CallShortMethod(j->env, jinfo->obj,
                                                  foo->symbol, jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (short int)(*j->env)->CallShortMethod(j->env, jinfo->obj,
                                                  foo->symbol, jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        i = (short int)(*j->env)->CallShortMethod(j->env, jinfo->obj,
                                                  foo->symbol, ptr_arg);
    } else {
        i = (short int)(*j->env)->CallShortMethod(j->env, jinfo->obj,
                                                  foo->symbol);
    }

    return i;
}

static long long jni_call_ll(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    long long l;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        l = (long long)(*j->env)->CallLongMethod(j->env, jinfo->obj, foo->symbol,
                                                 jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        l = (long long)(*j->env)->CallLongMethod(j->env, jinfo->obj, foo->symbol,
                                                 jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        l = (long long)(*j->env)->CallLongMethod(j->env, jinfo->obj, foo->symbol,
                                                 ptr_arg);
    } else {
        l = (long long)(*j->env)->CallLongMethod(j->env, jinfo->obj,
                                                 foo->symbol);
    }

    return l;
}

static float jni_call_f(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    float f;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        f = (float)(*j->env)->CallFloatMethod(j->env, jinfo->obj, foo->symbol,
                                              jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        f = (float)(*j->env)->CallFloatMethod(j->env, jinfo->obj, foo->symbol,
                                              jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        f = (float)(*j->env)->CallFloatMethod(j->env, jinfo->obj, foo->symbol,
                                              ptr_arg);
    } else {
        f = (float)(*j->env)->CallFloatMethod(j->env, jinfo->obj, foo->symbol);
    }

    return f;
}

static double jni_call_d(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    double d;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        d = (double)(*j->env)->CallDoubleMethod(j->env, jinfo->obj, foo->symbol,
                                                jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        d = (double)(*j->env)->CallDoubleMethod(j->env, jinfo->obj, foo->symbol,
                                                jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        d = (double)(*j->env)->CallDoubleMethod(j->env, jinfo->obj, foo->symbol,
                                                ptr_arg);
    } else {
        d = (double)(*j->env)->CallDoubleMethod(j->env, jinfo->obj, foo->symbol);
    }

    return d;
}

static bool jni_call_b(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    bool b;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        b = (bool)(*j->env)->CallBooleanMethod(j->env, jinfo->obj, foo->symbol,
                                               jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        b = (bool)(*j->env)->CallBooleanMethod(j->env, jinfo->obj, foo->symbol,
                                               jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        b = (bool)(*j->env)->CallBooleanMethod(j->env, jinfo->obj, foo->symbol,
                                               ptr_arg);
    } else {
        b = (bool)(*j->env)->CallBooleanMethod(j->env, jinfo->obj, foo->symbol);
    }

    return b;
}

static char *jni_call_cp(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    char *s = NULL;
    jstring ret = NULL;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        ret = (jstring)(*j->env)->CallObjectMethod(j->env, jinfo->obj,
                                                   foo->symbol, jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        ret = (jstring)(*j->env)->CallObjectMethod(j->env, jinfo->obj,
                                                   foo->symbol, jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        ret = (jstring)(*j->env)->CallObjectMethod(j->env, jinfo->obj,
                                                   foo->symbol, ptr_arg);
    } else {
        ret = (jstring)(*j->env)->CallObjectMethod(j->env, jinfo->obj,
                                                   foo->symbol);
    }

    if (ret != NULL)
        s = strdup((char *)(*j->env)->GetStringUTFChars((j->env), ret, NULL));

    return s;
}

static void *jni_call_p(struct jdriver *j, struct jinfo *jinfo,
    struct cplugin_function_s *foo, jstring jargs, void *ptr_arg)
{
    jobject jret;

    if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
        (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol,
                                           jargs, ptr_arg);
    } else if ((foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               !(foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol,
                                           jargs);
    } else if (!(foo->arg_mode & CL_PLUGIN_ARGS_COMMON) &&
               (foo->arg_mode & CL_PLUGIN_ARGS_POINTER))
    {
        jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol,
                                           ptr_arg);
    } else {
        jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol);
    }

    return jret;
}

/*
 *
 * Internal API
 *
 */

/*
 *
 * Plugin Driver API
 *
 */

CL_INTERNAL_API
void *jni_library_init(void)
{
    struct jdriver *j = NULL;
    JavaVMInitArgs vm_args;
    JavaVMOption options;

    j = calloc(1, sizeof(struct jdriver));

    if (NULL == j)
        return NULL;

    options.optionString = "-Djava.class.path=" CL_PLUGIN_CLASSPATH;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.options = &options;
    vm_args.nOptions = MAX_JVM_OPTIONS;
    vm_args.ignoreUnrecognized = JNI_TRUE;

    /* Create the Java VM */
    if (JNI_CreateJavaVM(&j->jvm, (void **)&j->env, &vm_args) < 0) {
        /* Error */
        return NULL;
    }

    return j;
}

CL_INTERNAL_API
void jni_library_uninit(void *data)
{
    struct jdriver *j = (struct jdriver *)data;

    if (j != NULL)
        (*j->jvm)->DestroyJavaVM(j->jvm);
}

CL_INTERNAL_API
cl_plugin_info_t *jni_load_info(void *data, void *handle)
{
    struct cplugin_entry_api_methods {
        char *name;
        char *signature;
        char *data;
    };

    struct jdriver *j = (struct jdriver *)data;
    cl_plugin_info_t *info = NULL;
    jclass cls;
    jmethodID constructor, m;
    jobject obj;
    jstring s;
    int i, t;
    struct cplugin_entry_api_methods emethods[] = {
        { "getName",        "()Ljava/lang/String;",  NULL },
        { "getVersion",     "()Ljava/lang/String;",  NULL },
        { "getAuthor",      "()Ljava/lang/String;",  NULL },
        { "getDescription", "()Ljava/lang/String;",  NULL }
    };

    t = sizeof(emethods) / sizeof(emethods[0]);

    /* call class constructor */
    cls = (jclass)handle;
    constructor = (*j->env)->GetMethodID(j->env, cls, "<init>", "()V");
    obj = (*j->env)->NewObject(j->env, cls, constructor);

    /* TODO: validate if we can cantinue... */

    /* call CpluginEntryAPI methods */
    for (i = 0; i < t; i++) {
        m = (*j->env)->GetMethodID(j->env, cls, emethods[i].name,
                                   emethods[i].signature);

        s = (jstring)(*j->env)->CallObjectMethod(j->env, obj, m);
        emethods[i].data = (char *)(*j->env)->GetStringUTFChars(j->env,
                                                                s, 0);
    }

    info = info_create_from_data(emethods[0].data, emethods[1].data,
                                 emethods[2].data, emethods[3].data);

    if (info != NULL)
        set_custom_plugin_info(info, obj);

    return info;
}

CL_INTERNAL_API
cl_string_t *type_to_jni_type(enum cl_type type)
{
    cl_string_t *r = NULL;

    switch (type) {
    case CL_VOID:
        r = cl_string_create("V");
        break;

    case CL_CHAR:
    case CL_UCHAR:
        r = cl_string_create("B"); /* byte */
        break;

    case CL_INT:
    case CL_UINT:
    case CL_LONG:
    case CL_ULONG:
        r = cl_string_create("I");
        break;

    case CL_SINT:
    case CL_USINT:
        r = cl_string_create("S");
        break;

    case CL_FLOAT:
        r = cl_string_create("F");
        break;

    case CL_DOUBLE:
        r = cl_string_create("D");
        break;

    case CL_LLONG:
    case CL_ULLONG:
        r = cl_string_create("J");
        break;

    case CL_POINTER:
    case CL_CSTRING:
        r = cl_string_create("Ljava/lang/Object;");
        break;

    case CL_STRING:
        r = cl_string_create("Ljava/lang/String;");
        break;

    case CL_BOOLEAN:
        r = cl_string_create("Z");
        break;
    }

    return r;
}

CL_INTERNAL_API
int jni_load_functions(void *data, struct cplugin_function_s *flist,
    void *handle)
{
    struct jdriver *j = (struct jdriver *)data;
    jclass cls = (jclass)handle;
    struct jtmp_data jtmp;

    jtmp.jdriver = j;
    jtmp.cls = cls;

    if (cl_dll_map(flist, jni_load_function, &jtmp) != NULL)
        return -1;

    return 0;
}

CL_INTERNAL_API
void *jni_open(void *data, const char *pathname)
{
    struct jdriver *j = (struct jdriver *)data;
    jclass cls;
    char *classname;

    classname = strip_filename(pathname);
    cls = (*j->env)->FindClass(j->env, classname);
    free(classname);

    if (NULL == cls) {
        /* Class not found */
        return NULL;
    }

    return cls;
}

CL_INTERNAL_API
int jni_close(void *data, void *handle __attribute__((unused)))
{
    struct jdriver *j = (struct jdriver *)data;

    (*j->jvm)->DestroyJavaVM(j->jvm);

    return 0;
}

CL_INTERNAL_API
cl_object_t *jni_call(void *data, struct cplugin_function_s *foo,
    cl_plugin_t *cpl, struct function_argument *args)
{
    struct jdriver *j = (struct jdriver *)data;
    cplugin_s *c = (cplugin_s *)cpl;
    struct jinfo *jinfo = NULL;
    cl_object_t *ret = NULL;
    char *tmp;
    void *ptr;
    jstring jargs;

    jinfo = (struct jinfo *)info_get_custom_data(c->info);

    if (NULL == jinfo)
        return NULL;

    ret = cl_object_create_empty(foo->return_value);

    if (foo->arg_mode & CL_PLUGIN_ARGS_COMMON) {
        jargs = (*j->env)->NewStringUTF(j->env, args->jargs);

        /*
         * XXX: Don't we need to release this?
         */
    }

    switch (foo->return_value) {
    case CL_VOID:
        jni_call_v(j, jinfo, foo, jargs, args->ptr);
        break;

    case CL_CHAR:
        cl_object_set_char(ret,
                           jni_call_c(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_UCHAR:
        cl_object_set_uchar(ret,
                            (unsigned char)jni_call_c(j, jinfo, foo,
                                                      jargs, args->ptr));

        break;

    case CL_INT:
        cl_object_set_int(ret,
                          jni_call_i(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_UINT:
        cl_object_set_uint(ret,
                           (unsigned int)jni_call_i(j, jinfo, foo,
                                                    jargs, args->ptr));

        break;

    case CL_SINT:
        cl_object_set_sint(ret,
                           jni_call_si(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_USINT:
        cl_object_set_usint(ret,
                            (unsigned short int)jni_call_si(j, jinfo, foo,
                                                            jargs,
                                                            args->ptr));

        break;

    case CL_FLOAT:
        cl_object_set_float(ret,
                            jni_call_f(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_DOUBLE:
        cl_object_set_double(ret,
                             jni_call_d(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_LONG:
        cl_object_set_long(ret,
                           (long)jni_call_i(j, jinfo, foo, jargs,
                                            args->ptr));

        break;

    case CL_ULONG:
        cl_object_set_ulong(ret,
                            (unsigned long)jni_call_i(j, jinfo, foo,
                                                      jargs, args->ptr));

        break;

    case CL_LLONG:
        cl_object_set_llong(ret,
                            jni_call_ll(j, jinfo, foo, jargs, args->ptr));

        break;

    case CL_ULLONG:
        cl_object_set_ullong(ret,
                             (unsigned long long)jni_call_ll(j, jinfo, foo,
                                                             jargs,
                                                             args->ptr));

        break;

    case CL_POINTER:
        ptr = jni_call_p(j, jinfo, foo, jargs, args->ptr);
        cl_object_set(ret, false, ptr, -1);
        break;

    case CL_STRING:
        tmp = jni_call_cp(j, jinfo, foo, jargs, args->ptr);

        if (tmp != NULL) {
            cl_object_set_string(ret, tmp);
            free(tmp);
        } else {
            /* We must destroy the object and return NULL to the caller */
            cl_object_destroy(ret);
            ret = NULL;
        }

        break;

    case CL_BOOLEAN:
        cl_object_set_boolean(ret,
                              jni_call_b(j, jinfo, foo, jargs, args->ptr));

        break;

    default:
        break;
    }

    return ret;
}

CL_INTERNAL_API
int jni_plugin_startup(void *data, void *handle, cl_plugin_info_t *info)
{
    struct jdriver *j = (struct jdriver *)data;
    jclass cls = (jclass)handle;
    struct jinfo *jinfo = NULL;
    jmethodID m;
    jint ret;

    jinfo = (struct jinfo *)info_get_custom_data(info);

    if (NULL == jinfo)
        return -1;

    /* call module init function */
    m = (*j->env)->GetMethodID(j->env, cls, "module_init", "()I");
    ret = (jint)(*j->env)->CallIntMethod(j->env, jinfo->obj, m);

    return ret;
}

CL_INTERNAL_API
int jni_plugin_shutdown(void *data, void *handle, cl_plugin_info_t *info)
{
    struct jdriver *j = (struct jdriver *)data;
    jclass cls = (jclass)handle;
    struct jinfo *jinfo = NULL;
    jmethodID m;

    jinfo = (struct jinfo *)info_get_custom_data(info);

    if (NULL == jinfo)
        return -1;

    /* call module init function */
    m = (*j->env)->GetMethodID(j->env, cls, "module_uninit", "()V");
    (*j->env)->CallVoidMethod(j->env, jinfo->obj, m);
    release_cutom_plugin_info(jinfo);

    return 0;
}

CL_INTERNAL_API
bool jni_plugin_test(const cl_string_t *mime)
{
    cl_string_t *p = NULL;
    bool ret = false;

    p = cl_string_create("application/x-java-applet");

    if (cl_string_cmp(mime, p) == 0)
        ret = true;

    cl_string_unref(p);

    return ret;
}

CL_INTERNAL_API
int jni_load_foreign_function(void *data, void *handle,
    struct cplugin_function_s *foo)
{
}

