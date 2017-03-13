
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
#include "java/jni_cplugin.h"

/*
 * With our custom classpath we need to able to load the 'cplugin.jar'
 * file. So plugins may correctly load their dependencies.
 */
#define CPLUGIN_CLASSPATH           "/usr/local/lib/cplugin.jar:/usr/local/lib"
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

static void set_custom_plugin_info(cplugin_info_t *info, jobject obj)
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

/*
 *
 * Plugin Driver API
 *
 */

void *jni_library_init(void)
{
    struct jdriver *j = NULL;
    JavaVMInitArgs vm_args;
    JavaVMOption options;

    j = calloc(1, sizeof(struct jdriver));

    if (NULL == j)
        return NULL;

    options.optionString = "-Djava.class.path=" CPLUGIN_CLASSPATH;
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

void jni_library_uninit(void *data)
{
    struct jdriver *j = (struct jdriver *)data;

    if (j != NULL)
        (*j->jvm)->DestroyJavaVM(j->jvm);
}

cplugin_info_t *jni_load_info(void *data, void *handle)
{
    struct cplugin_entry_api_methods {
        char *name;
        char *signature;
        char *data;
    };

    struct jdriver *j = (struct jdriver *)data;
    cplugin_info_t *info = NULL;
    jclass cls;
    jmethodID constructor, m;
    jobject obj;
    jstring s;
    int i, t;
    struct cplugin_entry_api_methods emethods[] = {
        { "getName",        "()Ljava/lang/String;",  NULL },
        { "getVersion",     "()Ljava/lang/String;",  NULL },
        { "getCreator",     "()Ljava/lang/String;",  NULL },
        { "getDescription", "()Ljava/lang/String;",  NULL },
        { "getAPI",         "()Ljava/lang/String;",  NULL }
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
                                 emethods[2].data, emethods[3].data,
                                 emethods[4].data);

    if (info != NULL)
        set_custom_plugin_info(info, obj);

    return info;
}

cstring_t *type_to_jni_type(enum cl_type type)
{
    cstring_t *r = NULL;

    switch (type) {
        case CL_VOID:
            r = cstring_create("V");
            break;

        case CL_CHAR:
        case CL_UCHAR:
            r = cstring_create("B"); /* byte */
            break;

        case CL_INT:
        case CL_UINT:
        case CL_LONG:
        case CL_ULONG:
            r = cstring_create("I");
            break;

        case CL_SINT:
        case CL_USINT:
            r = cstring_create("S");
            break;

        case CL_FLOAT:
            r = cstring_create("F");
            break;

        case CL_DOUBLE:
            r = cstring_create("D");
            break;

        case CL_LLONG:
        case CL_ULLONG:
            r = cstring_create("J");
            break;

        case CL_POINTER:
        case CL_CSTRING:
            r = cstring_create("Ljava/lang/Object;");
            break;

        case CL_STRING:
            r = cstring_create("Ljava/lang/String;");
            break;

        case CL_BOOLEAN:
            r = cstring_create("Z");
            break;
    }

    return r;
}

static void add_arguments_signature(cstring_t *jargs,
    struct cplugin_function_s *foo)
{
}

static void add_return_value_signature(cstring_t *jargs,
    struct cplugin_function_s *foo)
{
    cstring_t *foo_return = NULL;

    foo_return = type_to_jni_type(foo->return_value);
    cstring_cat(jargs, ")%s", cstring_valueof(foo_return));
    cstring_unref(foo_return);
}

static cstring_t *create_method_signature(struct cplugin_function_s *foo)
{
    cstring_t *jargs = cstring_create("(");

    if (foo->arg_mode != CPLUGIN_ARGS_VOID)
        add_arguments_signature(jargs, foo);

    if (foo->return_value == CL_VOID)
        cstring_cat(jargs, ")V");
    else
        add_return_value_signature(jargs, foo);

/*    if (foo->return_value == CL_VOID) {
        if (foo->type_of_args != CPLUGIN_NO_ARGS)
            cstring_cat(jargs, "Lcplugin/CpluginArguments;");

        cstring_cat(jargs, ")V");
    } else {
        if (foo->type_of_args != CPLUGIN_NO_ARGS)
            cstring_cat(jargs, "Lcplugin/CpluginArguments;");

        cstring_cat(jargs, ")Lcplugin/CpluginObject;");
    }*/

    return jargs;
}

static int jni_load_function(void *a, void *b)
{
    struct cplugin_function_s *foo = (struct cplugin_function_s *)a;
    struct jtmp_data *jtmp = (struct jtmp_data *)b;
    struct jdriver *j = jtmp->jdriver;
    jmethodID m;
    cstring_t *signature = NULL;

    signature = create_method_signature(foo);
    m = (*j->env)->GetMethodID(j->env, jtmp->cls, foo->name,
                               cstring_valueof(signature));

    cstring_unref(signature);
    foo->symbol = m;

    return 0;
}

int jni_load_functions(void *data, struct cplugin_function_s *flist,
    void *handle)
{
    struct jdriver *j = (struct jdriver *)data;
    jclass cls = (jclass)handle;
    struct jtmp_data jtmp;

    jtmp.jdriver = j;
    jtmp.cls = cls;

    if (cdll_map(flist, jni_load_function, &jtmp) != NULL)
        return -1;

    return 0;
}

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

int jni_close(void *data, void *handle __attribute__((unused)))
{
    struct jdriver *j = (struct jdriver *)data;

    (*j->jvm)->DestroyJavaVM(j->jvm);

    return 0;
}

cobject_t *jni_call(void *data, struct cplugin_function_s *foo,
    uint32_t caller_id, cplugin_t *cpl,
    struct function_argument *args __attribute__((unused)))
{
    struct jdriver *j = (struct jdriver *)data;
    cplugin_s *c = (cplugin_s *)cpl;
    struct jinfo *jinfo = NULL;
    jobject jret = NULL, args;
    cobject_t *ret;

    jinfo = (struct jinfo *)info_get_custom_data(c->info);

    if (NULL == jinfo)
        return NULL;

    ret = cobject_create_empty(foo->return_value);

    if (foo->return_value == CL_VOID) {
        if (foo->type_of_args != CPLUGIN_NO_ARGS) {
            args = newCpluginArguments(j->env, foo);
            (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol, args);
        } else
            (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol);
    } else {
        if (foo->type_of_args != CPLUGIN_NO_ARGS) {
            args = newCpluginArguments(j->env, foo);
            jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol,
                                              args);
        } else
            jret = (*j->env)->CallObjectMethod(j->env, jinfo->obj, foo->symbol);

        if (jret != NULL) {
            /* Sets the return value from the function */
            set_return_value_from_CpluginObject(j->env, cpl, foo->name,
                                                caller_id, &jret);
        }
    }

    return ret;
}

int jni_plugin_startup(void *data, void *handle, cplugin_info_t *info)
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
    ret = (jint)(*j->env)->CallObjectMethod(j->env, jinfo->obj, m);

    return ret;
}

int jni_plugin_shutdown(void *data, void *handle, cplugin_info_t *info)
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
    (*j->env)->CallObjectMethod(j->env, jinfo->obj, m);
    release_cutom_plugin_info(jinfo);

    return 0;
}

bool jni_plugin_test(const cstring_t *mime)
{
    cstring_t *p = NULL;
    bool ret = false;

    p = cstring_create("application/x-java-applet");

    if (cstring_cmp(mime, p) == 0)
        ret = true;

    cstring_unref(p);

    return ret;
}

