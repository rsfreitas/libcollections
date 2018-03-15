
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Mar  9 15:10:02 2018
 * Project: libcollections
 *
 * Copyright (C) 2018 Rodrigo Freitas All rights reserved.
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

static int add_argument_type(struct cplugin_function_s *foo, enum cl_type type)
{
    struct cl_arg_type *arg = NULL;

    arg = new_arg_type(type);

    if (NULL == arg)
        return -1;

    cl_list_unshift(foo->arg_types, arg, -1);

    return 0;
}

static int add_argument_value(struct cplugin_function_s *foo,
    const char *argument_name, cl_object_t *argument)
{
    if ((cl_hashtable_put(foo->arguments, argument_name,
                          cl_object_ref(argument)) == NULL) &&
        (cl_get_last_error() != CL_NO_ERROR))
    {
        return -1;
    }

    return 0;
}

static int add_argument(struct cplugin_function_s *foo, cl_object_t *argument,
    const char *argument_name, enum cl_type argument_type)
{
    /* Insert a new argument type */
    if (add_argument_type(foo, argument_type) < 0)
        return -1;

    /* Add a new argument and its value */
    if (add_argument_value(foo, argument_name, argument) < 0)
        return -1;

    return 0;
}

int arguments_parse(struct cplugin_function_s *foo, int argc, va_list ap)
{
    int i = 0, arg_type;
    char *arg_name = NULL;
    cl_object_t *argument = NULL;

    while (i < argc) {
        arg_name = va_arg(ap, char *);
        i += 1;

        arg_type = (enum cl_type)va_arg(ap, int);
        i += 1;

        /*
         * It's important to notice here that if @arg_type is of CL_POINTER,
         * the user must inform if data will be internally copied or not, its
         * size (in bytes) and a function to release it (optionally). These
         * follow the same rules apllied to the cl_object_create function.
         */
        argument = cl_object_vcreate(arg_type, ap);

        if (argument != NULL)
            if (add_argument(foo, argument, arg_name, arg_type) < 0)
                return -1;

        if (cl_object_type(argument) == CL_POINTER)
            i += 4;
        else
            i += 1;

        cl_object_unref(argument);
    }

    return 0;
}

