
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

int main(int argc, char **argv)
{
    const char *opt = "f:hI";
    int option;
    char *filename = NULL;
    cplugin_t *cpl;
    bool info = false;
    cvalue_t *ret;

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

    /* XXX: call test function */
    ret = cplugin_call(cpl, "foo_int", NULL);

    if (ret != NULL) {
        printf("foo_int return value: %d\n", CVALUE_INT(ret));
        cvalue_unref(ret);
    }

    //cplugin_call(cpl, "foo_args", "arg1", 20, "arg2", 30, NULL);
    cplugin_call(cpl, "foo_args",
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
                 "arg11", 12345,
                 "arg12", 123456,
                 "arg13", true,
                 "arg14", "texto",
                 NULL);

    printf("Error 1: %s\n", cstrerror(cget_last_error()));

    cplugin_unload(cpl);

    if (filename != NULL)
        free(filename);

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

