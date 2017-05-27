
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

#include <stdarg.h>

#include "collections.h"
#include "plugin.h"

int adjust_arguments(struct cplugin_function_s *foo,
    struct function_argument *args, int argc, va_list ap)
{
    struct cplugin_fdata_s *arg = NULL;
    char *arg_name;
    cl_string_t *p;
    cl_json_t *node = NULL, *jargs = NULL;
    void *ptr;
    int i = 0;
    bool b;
    enum cl_json_type type;

    jargs = cl_json_create_object();

    for (i = 0; i < argc; i += 2) {
        arg_name = va_arg(ap, char *);

        /*
         * Search for the structure that contains the value passed as
         * argument by its own argument name informed by the user at the
         * moment of function call.
         *
         * Example: call("arg_name", value, "arg_name", value);
         */
        arg = cl_dll_map(foo->args, search_cplugin_fdata_s, arg_name);

        if (NULL == arg)
            return -1;

        switch (arg->type) {
            case CL_CHAR:
                node = cl_json_create_node(CL_JSON_STRING, "%c",
                                           (char)va_arg(ap, int));

                break;

            case CL_UCHAR:
                node = cl_json_create_node(CL_JSON_STRING, "%c",
                                           (unsigned char)va_arg(ap, int));

                break;

            case CL_BOOLEAN:
                b = (bool)va_arg(ap, int);

                if (b == true)
                    type = CL_JSON_TRUE;
                else
                    type = CL_JSON_FALSE;

                node = cl_json_create_node(type, "%d", b);
                break;

            case CL_INT:
                node = cl_json_create_node(CL_JSON_NUMBER, "%d",
                                           va_arg(ap, int));

                break;

            case CL_UINT:
                node = cl_json_create_node(CL_JSON_NUMBER, "%u",
                                           (unsigned int)va_arg(ap, int));

                break;

            case CL_SINT:
                node = cl_json_create_node(CL_JSON_NUMBER, "%hd",
                                           (short int)va_arg(ap, int));

                break;

            case CL_USINT:
                node = cl_json_create_node(CL_JSON_NUMBER, "%hu",
                                           (unsigned int)va_arg(ap, int));

                break;

            case CL_FLOAT:
                node = cl_json_create_node(CL_JSON_NUMBER_FLOAT, "%f",
                                           (float)va_arg(ap, double));

                break;

            case CL_DOUBLE:
                node = cl_json_create_node(CL_JSON_NUMBER_FLOAT, "%f",
                                           va_arg(ap, double));

                break;

            case CL_LONG:
                node = cl_json_create_node(CL_JSON_NUMBER, "%ld",
                                           va_arg(ap, long));

                break;

            case CL_ULONG:
                node = cl_json_create_node(CL_JSON_NUMBER, "%lu",
                                           va_arg(ap, unsigned long));

                break;

            case CL_LLONG:
                node = cl_json_create_node(CL_JSON_NUMBER, "%lld",
                                           va_arg(ap, long long));

                break;

            case CL_ULLONG:
                node = cl_json_create_node(CL_JSON_NUMBER, "%llu",
                                           va_arg(ap, unsigned long long));

                break;

            case CL_POINTER:
                ptr = va_arg(ap, void *);
                args->ptr = ptr;
                break;

            case CL_STRING:
            case CL_CSTRING:
                if (arg->type == CL_STRING)
                    p = cl_string_create("%s", va_arg(ap, char *));
                else
                    p = (cl_string_t *)va_arg(ap, void *);

                node = cl_json_create_node(CL_JSON_STRING, "%s",
                                           cl_string_valueof(p));

                cl_string_unref(p);
                break;

            default:
                node = NULL;
                break;
        }

        if (node != NULL)
            cl_json_add_item_to_object(jargs, arg_name, node);
    }

    args->jargs = cl_json_to_string(jargs, false);
    cl_json_delete(jargs);

    return 0;
}

