
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

int adjust_arguments(struct cplugin_function_s *foo, int argc, va_list ap)
{
    int i = 0, increment = 0, t = 0;
    struct cplugin_fdata_s *arg = NULL;
    char *tmp;
    cstring_t *p;

    if (foo->type_of_args == CPLUGIN_ARG_FIXED)
        increment = 2;
    else if (foo->type_of_args == CPLUGIN_ARG_VAR)
        increment = 3;
    else {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    printf("total de argumentos: %d, %d\n", argc, cdll_size(foo->args));
    for (i = 0; i < argc; i += increment) {
        /*
         * Search for the structure that contains the value passed as
         * argument by its own argument name informed by the user at the
         * moment of function call.
         *
         * Example: call("arg_name", value, "arg_name", value);
         */
        printf("%s %d\n", __FUNCTION__, 5);
        if (foo->type_of_args == CPLUGIN_ARG_FIXED) {
        printf("%s %d\n", __FUNCTION__, 6);
            tmp = va_arg(ap, char *);
            printf("%s: tmp =  %s\n", __FUNCTION__, tmp);
            arg = cdll_map(foo->args, search_cplugin_fdata_s, tmp);
//                           va_arg(ap, char *));
        printf("%s %d\n", __FUNCTION__, 7);
        } else if (foo->type_of_args == CPLUGIN_ARG_VAR) {
            tmp = va_arg(ap, char *);
            t = va_arg(ap, int);
            arg = new_cplugin_fdata_s(tmp, t, 0);
        }

        if (NULL == arg)
            return -1;

                printf("%s %d\n", __FUNCTION__, arg->type);
        switch (arg->type) {
            case CL_CHAR:
                arg->value = cvalue_create(CL_CHAR, va_arg(ap, int), NULL);
                break;

            case CL_UCHAR:
                arg->value = cvalue_create(CL_UCHAR, va_arg(ap, int), NULL);
                break;

            case CL_INT:
                arg->value = cvalue_create(CL_INT, va_arg(ap, int), NULL);
                break;

            case CL_UINT:
                arg->value = cvalue_create(CL_UINT, va_arg(ap, int), NULL);
                break;

            case CL_SINT:
                arg->value = cvalue_create(CL_SINT, va_arg(ap, int), NULL);
                break;

            case CL_USINT:
                arg->value = cvalue_create(CL_USINT, va_arg(ap, int), NULL);
                break;

            case CL_FLOAT:
                arg->value = cvalue_create(CL_FLOAT,
                                           va_arg(ap, double), NULL);

                break;

            case CL_DOUBLE:
                arg->value = cvalue_create(CL_DOUBLE,
                                           va_arg(ap, double), NULL);

                break;

            case CL_LONG:
                arg->value = cvalue_create(CL_LONG,
                                           va_arg(ap, long), NULL);

                break;

            case CL_ULONG:
                arg->value = cvalue_create(CL_ULONG,
                                           va_arg(ap, long), NULL);

                break;

/*            case CL_LLONG:
                printf("%s 1 -- %d\n", __FUNCTION__, arg->type);
                arg->value = cvalue_create(CL_LLONG,
                                           va_arg(ap, long long), NULL);

                printf("%s 2 -- %d, %d\n", __FUNCTION__, arg->type, CVALUE_LLONG(arg->value));
                break;*/

            case CL_ULLONG:
                arg->value = cvalue_create(CL_ULLONG,
                                           va_arg(ap, long long), NULL);

                break;

            case CL_POINTER:
                arg->value = cvalue_create(CL_POINTER, false,
                                           va_arg(ap, void *), 0, NULL);

                break;

            case CL_BOOLEAN:
                arg->value = cvalue_create(CL_BOOLEAN, va_arg(ap, int), NULL);
                break;

            case CL_STRING:
                printf("%s %d\n", __FUNCTION__, arg->type);
                p = cstring_create("%s", va_arg(ap, char *));
                printf("%s %d\n", __FUNCTION__, arg->type);
                arg->value = cvalue_create(CL_STRING, p, NULL);
                printf("%s %d\n", __FUNCTION__, arg->type);
                break;

            default:
                va_arg(ap, int);
                break;
//                cset_errno(CL_UNSUPPORTED_TYPE);
//                return -1;
        }

        printf("%s %d\n", __FUNCTION__, 3);
        if (foo->type_of_args == CPLUGIN_ARG_VAR)
            foo->args = cdll_unshift(foo->args, arg);
        printf("%s %d\n", __FUNCTION__, 4);
    }

    printf("%s %d\n", __FUNCTION__, 100);

    return 0;
}

