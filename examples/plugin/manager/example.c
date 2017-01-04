
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Feb  7 23:06:50 2016
 * Project: examples
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <collections.h>

static void help(void)
{
    fprintf(stdout, "Usage: plugin-manager [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "plugin API for the plugin manager size.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -f [file name]\tIndicate the plugin to load.\n");
    fprintf(stdout, "  -I\t\t\tOnly shows the plugin informations.\n");

    fprintf(stdout, "\n");
}

static void plugin_details(cplugin_info_t *info)
{
    cstring_t *p, *q;
    cstring_list_t *l, *a;
    int i, t = 0, j, k = 0;
    char *name, *arg_name;

    fprintf(stdout, "Name: %s\n", cplugin_name(info));
    fprintf(stdout, "Author: %s\n", cplugin_author(info));
    fprintf(stdout, "Version: %s\n", cplugin_version(info));
    fprintf(stdout, "Description: %s\n", cplugin_description(info));

    p = cplugin_API(info);
    fprintf(stdout, "API: %s\n", cstring_valueof(p));
    cstring_unref(p);

    l = cplugin_functions(info);
    t = cstring_list_size(l);

    for (i = 0; i < t; i++) {
        p = cstring_list_get(l, i);
        name = (char *)cstring_valueof(p);

        fprintf(stdout, "Function name: %s\n", name);
        fprintf(stdout, "Return type: %d\n",
                cplugin_function_return_type(info, name));

        fprintf(stdout, "Argument mode: %d\n",
                cplugin_function_arg_mode(info, name));

        a = cplugin_function_arguments(info, name);

        if (NULL == a) {
            cstring_unref(p);
            continue;
        }

        k = cstring_list_size(a);
        fprintf(stdout, "Arguments\n");

        for (j = 0; j < k; j++) {
            q = cstring_list_get(a, j);
            arg_name = (char *)cstring_valueof(q);
            fprintf(stdout, "\tName: %s\n", arg_name);
            fprintf(stdout, "\tType: %d\n",
                    cplugin_function_arg_type(info, name, arg_name));

            cstring_unref(q);
        }

        cstring_list_destroy(a);
        cstring_unref(p);
    }

    cstring_list_destroy(l);
}

static void load_and_show_plugin_info(const char *filename)
{
    cplugin_info_t *info;

    info = cplugin_info_from_file(filename);

    if (NULL == info) {
        return ;
    }

    plugin_details(info);
    cplugin_info_unref(info);
}

static void show_plugin_info(cplugin_t *cpl)
{
    cplugin_info_t *info;

    info = cplugin_info(cpl);

    if (NULL == info) {
        return;
    }

    plugin_details(info);
    cplugin_info_unref(info);
}

static cstring_t *get_return_as_string(const char *name, cobject_t *v)
{
    cstring_t *s = cstring_create("%s return value: ", name);
    char *tmp;

    if (COBJECT_ischar(v))
        cstring_cat(s, "%c", COBJECT_AS_CHAR(v));
    else if (COBJECT_isuchar(v))
        cstring_cat(s, "%d", COBJECT_AS_UCHAR(v));
    else if (COBJECT_isint(v))
        cstring_cat(s, "%d", COBJECT_AS_INT(v));
    else if (COBJECT_isuint(v))
        cstring_cat(s, "%u", COBJECT_AS_UINT(v));
    else if (COBJECT_issint(v))
        cstring_cat(s, "%d", COBJECT_AS_SINT(v));
    else if (COBJECT_isusint(v))
        cstring_cat(s, "%d", COBJECT_AS_USINT(v));
    else if (COBJECT_isfloat(v))
        cstring_cat(s, "%f", COBJECT_AS_FLOAT(v));
    else if (COBJECT_isdouble(v))
        cstring_cat(s, "%f", COBJECT_AS_DOUBLE(v));
    else if (COBJECT_islong(v))
        cstring_cat(s, "%ld", COBJECT_AS_LONG(v));
    else if (COBJECT_isulong(v))
        cstring_cat(s, "%lu", COBJECT_AS_ULONG(v));
    else if (COBJECT_isllong(v))
        cstring_cat(s, "%lld", COBJECT_AS_LLONG(v));
    else if (COBJECT_isullong(v))
        cstring_cat(s, "%llu", COBJECT_AS_ULLONG(v));
    else if (COBJECT_isstring(v)) {
        tmp = COBJECT_AS_STRING(v);

        if (tmp != NULL)
            cstring_cat(s, "%s", tmp);
    } else if (COBJECT_isboolean(v))
        cstring_cat(s, "%d", COBJECT_AS_BOOLEAN(v));

    return s;
}

static void call_functions(cplugin_t *cpl)
{
    cplugin_info_t *info;
    cstring_list_t *l;
    int i, t;
    cstring_t *p, *s;
    cobject_t *ret;

    info = cplugin_info(cpl);
    l = cplugin_functions(info);
    t = cstring_list_size(l);

    for (i = 0; i < t; i++) {
        p = cstring_list_get(l, i);
        ret = cplugin_call(cpl, cstring_valueof(p), NULL);
        s = get_return_as_string(cstring_valueof(p), ret);
        printf("%s: %s\n", __FUNCTION__, cstring_valueof(s));
        cstring_unref(s);
        cobject_unref(ret);
        cstring_unref(p);
    }

    cstring_list_destroy(l);
    cplugin_info_unref(info);
}

int main(int argc, char **argv)
{
    const char *opt = "f:hI";
    int option;
    char *filename = NULL;
    cplugin_t *cpl;
    bool info = false;
    cobject_t *ret;
    int i;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'f':
                filename = strdup(optarg);
                break;

            case 'I':
                info = true;
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == filename) {
        fprintf(stderr, "There must be a valid plugin file name.\n");
        return -1;
    }

    collections_init();

    if (info == true) {
        load_and_show_plugin_info(filename);
        return 0;
    }

    cpl = cplugin_load(filename);

    if (NULL == cpl) {
        printf("Error 0: %s\n", cstrerror(cget_last_error()));
        return -1;
    }

    /* Show plugin informations */
    show_plugin_info(cpl);

    /* XXX: call test functions */
//    call_functions(cpl);

/*    cplugin_call(cpl, "foo_args",
                 "arg1", 20,
                 "arg2", 21,
                 "arg3", 22,
                 "arg4", 23,
                 "arg5", 'a',
                 "arg6", 231,
                 "arg7", 3.1415f,
                 "arg8", 2.27,
                 "arg9", 123,
                 "arg10", 1234,
                 "arg11", 12345LL,
                 "arg12", 123456LL,
                 "arg13", true,
                 "arg14", "Sample text",
                 NULL);*/

/*    ret = cplugin_call(cpl, "foo_class", "data", "Ola, sou uma classe", NULL);
    printf("Type of return: %d\n", cobject_type(ret));
    ret = cplugin_call(cpl, "foo_pointer", "data", COBJECT_AS_POINTER(ret), NULL);
    printf("Second return: %d\n", COBJECT_AS_INT(ret));
    printf("Last call error: %s\n", cstrerror(cget_last_error()));*/

    void *p;
    for (i = 0; i < 20; i++) {
        ret = cplugin_call(cpl, "tx_connect", "info",
                           "{\"recv_timeout\": 10, \"connect_attempts\": 2}",
                           "job", "{\"nome\": \"job\"}", NULL);
        p = COBJECT_AS_POINTER(ret);
        cobject_unref(ret);
        ret = cplugin_call(cpl, "tx_send", "data", p, NULL);
        cobject_unref(ret);
        cplugin_call(cpl, "tx_finish", "data", p, NULL);
    }

    cplugin_unload(cpl);

    if (filename != NULL)
        free(filename);

    collections_uninit();

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

