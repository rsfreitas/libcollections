
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

    if (foo->type_of_args == CPLUGIN_ARG_FIXED)
        increment = 2;
    else if (foo->type_of_args == CPLUGIN_ARG_VAR)
        increment = 3;
    else {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    for (i = 0; i < argc; i += increment) {
        /*
         * Search for the structure that contains the value passed as
         * argument by its own argument name informed by the user at the
         * moment of function call.
         *
         * Example: call("arg_name", value, "arg_name", value);
         */
        if (foo->type_of_args == CPLUGIN_ARG_FIXED)
            arg = cdll_map(foo->args, search_cplugin_fdata_s, va_arg(ap, char *));
        else if (foo->type_of_args == CPLUGIN_ARG_VAR) {
            tmp = va_arg(ap, char *);
            t = va_arg(ap, int);
            arg = new_cplugin_fdata_s(tmp, t, 0);
        }

        if (NULL == arg)
            return -1;

        switch (arg->type) {
            case CL_INT:
                arg->value = cvalue_new(CL_INT, va_arg(ap, int), NULL);
                break;

            case CL_POINTER:
                arg->value = cvalue_new(CL_POINTER, false,
                                         va_arg(ap, void *), 0, NULL);

                break;

            case CL_CHAR:
                arg->value = cvalue_new(CL_CHAR, va_arg(ap, int), NULL);
                break;

            case CL_FLOAT:
                arg->value = cvalue_new(CL_FLOAT,
                                         va_arg(ap, double), NULL);

                break;

            default:
                cset_errno(CL_UNSUPPORTED_TYPE);
                return -1;
        }

        if (foo->type_of_args == CPLUGIN_ARG_VAR)
            foo->args = cdll_unshift(foo->args, arg);
    }

    return 0;
}

