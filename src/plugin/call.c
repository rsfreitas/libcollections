
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
    cstring_t *p;
    cjson_t *node, *jargs;
    void *ptr;
    int i = 0;

    jargs = cjson_create_object();

    for (i = 0; i < argc; i += 2) {
        arg_name = va_arg(ap, char *);

        /*
         * Search for the structure that contains the value passed as
         * argument by its own argument name informed by the user at the
         * moment of function call.
         *
         * Example: call("arg_name", value, "arg_name", value);
         */
        arg = cdll_map(foo->args, search_cplugin_fdata_s, arg_name);

        if (NULL == arg)
            return -1;

        switch (arg->type) {
            case CL_CHAR:
                node = cjson_create_stringv("%c", (char)va_arg(ap, int));
                break;

            case CL_UCHAR:
                node = cjson_create_stringv("%c", (unsigned char)va_arg(ap, int));
                break;

            case CL_INT:
            case CL_BOOLEAN:
                node = cjson_create_stringv("%d", va_arg(ap, int));
                break;

            case CL_UINT:
                node = cjson_create_stringv("%u", (unsigned int)va_arg(ap, int));
                break;

            case CL_SINT:
                node = cjson_create_stringv("%hd", (short int)va_arg(ap, int));
                break;

            case CL_USINT:
                node = cjson_create_stringv("%hu", (unsigned int)va_arg(ap, int));
                break;

            case CL_FLOAT:
                node = cjson_create_stringv("%f", (float)va_arg(ap, double));
                break;

            case CL_DOUBLE:
                node = cjson_create_stringv("%f", va_arg(ap, double));
                break;

            case CL_LONG:
                node = cjson_create_stringv("%ld", va_arg(ap, long));
                break;

            case CL_ULONG:
                node = cjson_create_stringv("%lu", va_arg(ap, unsigned long));
                break;

            case CL_LLONG:
                node = cjson_create_stringv("%lld", va_arg(ap, long long));
                break;

            case CL_ULLONG:
                node = cjson_create_stringv("%llu",
                                            va_arg(ap, unsigned long long));

                break;

            case CL_POINTER:
                ptr = va_arg(ap, void *);
                args->ptr = ptr;
                break;

            case CL_STRING:
                /*
                 * We support receiving JSON strings also. And to do this we need
                 * to replace every " for a \".
                 */
                p = cstring_create("%s", va_arg(ap, char *));
                cstring_rplsubstr(p, "\"", "\\\"");
                node = cjson_create_stringv("%s", cstring_valueof(p));
                cstring_unref(p);
                break;

            default:
                node = NULL;
                break;
        }

        if (node != NULL)
            cjson_add_item_to_object(jargs, arg_name, node);
    }

    args->jargs = cjson_to_string(jargs, false);
    cjson_delete(jargs);

    return 0;
}

