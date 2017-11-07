
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
    fprintf(stdout, "  -r [num. of runs]\tTell how many times the plugin "
                    "functions will run.\n");

    fprintf(stdout, "\n");
}

static void plugin_details(cl_plugin_info_t *info)
{
    cl_string_t *p, *q;
    cl_stringlist_t *l, *a;
    int i, t = 0, j, k = 0;
    char *name, *arg_name;

    fprintf(stdout, "Name: %s\n", cl_plugin_name(info));
    fprintf(stdout, "Author: %s\n", cl_plugin_author(info));
    fprintf(stdout, "Version: %s\n", cl_plugin_version(info));
    fprintf(stdout, "Description: %s\n", cl_plugin_description(info));

    p = cl_plugin_API(info);
    fprintf(stdout, "API: %s\n", cl_string_valueof(p));
    cl_string_unref(p);

    l = cl_plugin_functions(info);
    t = cl_stringlist_size(l);

    for (i = 0; i < t; i++) {
        p = cl_stringlist_get(l, i);
        name = (char *)cl_string_valueof(p);

        fprintf(stdout, "Function name: %s\n", name);
        fprintf(stdout, "Return type: %d\n",
                cl_plugin_function_return_type(info, name));

        fprintf(stdout, "Argument mode: %d\n",
                cl_plugin_function_arg_mode(info, name));

        a = cl_plugin_function_arguments(info, name);

        if (NULL == a) {
            cl_string_unref(p);
            continue;
        }

        k = cl_stringlist_size(a);
        fprintf(stdout, "Arguments\n");

        for (j = 0; j < k; j++) {
            q = cl_stringlist_get(a, j);
            arg_name = (char *)cl_string_valueof(q);
            fprintf(stdout, "\tName: %s\n", arg_name);
            fprintf(stdout, "\tType: %d\n",
                    cl_plugin_function_arg_type(info, name, arg_name));

            cl_string_unref(q);
        }

        cl_stringlist_destroy(a);
        cl_string_unref(p);
    }

    cl_stringlist_destroy(l);
}

static void load_and_show_plugin_info(const char *filename)
{
    cl_plugin_info_t *info;

    info = cl_plugin_info_from_file(filename);

    if (NULL == info) {
        return ;
    }

    plugin_details(info);
    cl_plugin_info_unref(info);
}

static void show_plugin_info(cl_plugin_t *cpl)
{
    cl_plugin_info_t *info;

    info = cl_plugin_info(cpl);

    if (NULL == info) {
        return;
    }

    plugin_details(info);
    cl_plugin_info_unref(info);
}

static cl_string_t *get_return_as_string(const char *name, cl_object_t *v)
{
    cl_string_t *s = cl_string_create("%s return value: ", name);
    char *tmp;

    if (CL_OBJECT_ischar(v))
        cl_string_cat(s, "%c", CL_OBJECT_AS_CHAR(v));
    else if (CL_OBJECT_isuchar(v))
        cl_string_cat(s, "%d", CL_OBJECT_AS_UCHAR(v));
    else if (CL_OBJECT_isint(v))
        cl_string_cat(s, "%d", CL_OBJECT_AS_INT(v));
    else if (CL_OBJECT_isuint(v))
        cl_string_cat(s, "%u", CL_OBJECT_AS_UINT(v));
    else if (CL_OBJECT_issint(v))
        cl_string_cat(s, "%d", CL_OBJECT_AS_SINT(v));
    else if (CL_OBJECT_isusint(v))
        cl_string_cat(s, "%d", CL_OBJECT_AS_USINT(v));
    else if (CL_OBJECT_isfloat(v))
        cl_string_cat(s, "%f", CL_OBJECT_AS_FLOAT(v));
    else if (CL_OBJECT_isdouble(v))
        cl_string_cat(s, "%f", CL_OBJECT_AS_DOUBLE(v));
    else if (CL_OBJECT_islong(v))
        cl_string_cat(s, "%ld", CL_OBJECT_AS_LONG(v));
    else if (CL_OBJECT_isulong(v))
        cl_string_cat(s, "%lu", CL_OBJECT_AS_ULONG(v));
    else if (CL_OBJECT_isllong(v))
        cl_string_cat(s, "%lld", CL_OBJECT_AS_LLONG(v));
    else if (CL_OBJECT_isullong(v))
        cl_string_cat(s, "%llu", CL_OBJECT_AS_ULLONG(v));
    else if (CL_OBJECT_isstring(v)) {
        tmp = CL_OBJECT_AS_STRING(v);

        if (tmp != NULL)
            cl_string_cat(s, "%s", tmp);
    } else if (CL_OBJECT_isboolean(v))
        cl_string_cat(s, "%d", CL_OBJECT_AS_BOOLEAN(v));

    return s;
}

static void call_functions(cl_plugin_t *cpl)
{
    cl_plugin_info_t *info;
    cl_stringlist_t *l;
    int i, t;
    cl_string_t *p, *s;
    cl_object_t *ret;

    info = cl_plugin_info(cpl);
    l = cl_plugin_functions(info);
    t = cl_stringlist_size(l);

    for (i = 0; i < t; i++) {
        p = cl_stringlist_get(l, i);
        ret = cl_plugin_call(cpl, cl_string_valueof(p), NULL);
        s = get_return_as_string(cl_string_valueof(p), ret);
        printf("%s: %s\n", __FUNCTION__, cl_string_valueof(s));
        cl_string_unref(s);
        cl_object_unref(ret);
        cl_string_unref(p);
    }

    cl_stringlist_destroy(l);
    cl_plugin_info_unref(info);
}

int main(int argc, char **argv)
{
    const char *opt = "f:hIr:";
    int option;
    char *filename = NULL;
    cl_plugin_t *cpl;
    bool info = false;
    cl_object_t *ret;
    int i, run = 1;
    void *p;

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

            case 'r':
                run = atoi(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == filename) {
        fprintf(stderr, "There must be a valid plugin file name.\n");
        return -1;
    }

    cl_init(NULL);

    if (info == true) {
        load_and_show_plugin_info(filename);
        return 0;
    }

    cpl = cl_plugin_load(filename);

    if (NULL == cpl) {
        printf("Error 0: %s\n", cl_strerror(cl_get_last_error()));
        return -1;
    }

    /* Show plugin informations */
    show_plugin_info(cpl);

    /* XXX: call test functions */
    for (i = 0; i < run; i++) {
        call_functions(cpl);

        cl_plugin_call(cpl, "foo_args",
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
                     NULL);

/*        ret = cl_plugin_call(cpl, "foo_class", "data", "Hi, I'm a class data...",
                           NULL);

        printf("Type of return: %d\n", cl_object_type(ret));
        p = CL_OBJECT_AS_POINTER(ret);
        cl_object_unref(ret);
        ret = cl_plugin_call(cpl, "foo_pointer", "data", p, NULL);
        printf("Second return: %d\n", CL_OBJECT_AS_INT(ret));
        cl_object_unref(ret);*/
    }

    ret = cl_plugin_foreign_call(cpl, "outside_api", CL_VOID, CL_PLUGIN_ARGS_VOID, NULL);

    if (NULL == ret)
        printf("outside call: %s\n", cl_strerror(cl_get_last_error()));

    ret = cl_plugin_foreign_call(cpl, "another_outside_api", CL_INT,
                                 CL_PLUGIN_ARGS_COMMON, "arg1", CL_INT, 9981, NULL);

    if (NULL == ret)
        printf("another outside call: %s\n", cl_strerror(cl_get_last_error()));
    else {
        printf("return: %d\n", CL_OBJECT_AS_INT(ret));
        cl_object_unref(ret);
    }

    cl_plugin_unload(cpl);

    if (filename != NULL)
        free(filename);

    cl_uninit();

    /* This makes valgrind report no memory leaks. */
    cl_exit();

    return 0;
}

