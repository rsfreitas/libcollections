
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Feb  7 23:29:53 2016
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

#include <collections.h>

struct ptr_arg {
    int a;
    int b;
    int c;
};

CL_PLUGIN_INIT()
{
    printf("pass through %s\n", __FUNCTION__);
    return 0;
}

CL_PLUGIN_UNINIT()
{
    printf("pass through %s\n", __FUNCTION__);
}

CL_PLUGIN_SET_INFO(
    "teste",
    "0.1",
    "Rodrigo Freitas",
    "C plugin example"
)

CL_PLUGIN_FUNCTION(int, foo_int)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);

    return 42;
}

CL_PLUGIN_FUNCTION(unsigned int, foo_uint)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 420;
}

CL_PLUGIN_FUNCTION(short int, foo_sint)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 421;
}

CL_PLUGIN_FUNCTION(unsigned short int, foo_usint)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 4201;
}

CL_PLUGIN_FUNCTION(char, foo_char)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 'a';
}

CL_PLUGIN_FUNCTION(unsigned char, foo_uchar)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 230;
}

CL_PLUGIN_FUNCTION(float, foo_float)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 42.5f;
}

CL_PLUGIN_FUNCTION(double, foo_double)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 4.2;
}

CL_PLUGIN_FUNCTION(bool, foo_boolean)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return true;
}

CL_PLUGIN_FUNCTION(long, foo_long)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 42000;
}

CL_PLUGIN_FUNCTION(unsigned long, foo_ulong)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 420001;
}

CL_PLUGIN_FUNCTION(long long, foo_llong)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 420009;
}

CL_PLUGIN_FUNCTION(unsigned long long, foo_ullong)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
    return 4200019;
}

/*CL_PLUGIN_OBJECT_ARGS_ONLY(void, foo_args)
{
    char *s;

    CL_PLUGIN_LOAD_ARGUMENTS();

    printf("arg1 value %d\n", CL_PLUGIN_ARGUMENT_INT("arg1"));
    printf("arg2 value %d\n", CL_PLUGIN_ARGUMENT_UINT("arg2"));
    printf("arg3 value %d\n", CL_PLUGIN_ARGUMENT_SINT("arg3"));
    printf("arg4 value %d\n", CL_PLUGIN_ARGUMENT_USINT("arg4"));
    printf("arg5 value %c\n", CL_PLUGIN_ARGUMENT_CHAR("arg5"));
    printf("arg6 value %d\n", CL_PLUGIN_ARGUMENT_UCHAR("arg6"));
    printf("arg7 value %f\n", CL_PLUGIN_ARGUMENT_FLOAT("arg7"));
    printf("arg8 value %f\n", CL_PLUGIN_ARGUMENT_DOUBLE("arg8"));
    printf("arg9 value %ld\n", CL_PLUGIN_ARGUMENT_LONG("arg9"));
    printf("arg10 value %lu\n", CL_PLUGIN_ARGUMENT_ULONG("arg10"));
    printf("arg11 value %lld\n", CL_PLUGIN_ARGUMENT_LLONG("arg11"));
    printf("arg12 value %llu\n", CL_PLUGIN_ARGUMENT_ULLONG("arg12"));
    printf("arg13 value %d\n", CL_PLUGIN_ARGUMENT_BOOL("arg13"));
    printf("arg14 value %s\n", CL_PLUGIN_ARGUMENT_STRING("arg14"));

    CL_PLUGIN_UNLOAD_ARGUMENTS();
}*/

CL_PLUGIN_FUNCTION(void, outside_api)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    printf("%s\n", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(int, another_outside_api)
{
    int i = cl_plugin_argument_int(args, "arg1");

    printf("%s\n", __FUNCTION__);
    printf("Argument 'arg1': %d\n", i);

    return 42;
}

CL_PLUGIN_FUNCTION(void, foo_pointer)
{
    struct ptr_arg *ptr2 = NULL;

    cl_plugin_argument_pointer(args, "ptr", (void **)&ptr2);
    printf("%s: %d\n", __FUNCTION__, sizeof(void *));
    printf("%s: address in plugin %p\n", __FUNCTION__, ptr2);

    printf("%s: a=%d, b=%d, c=%d\n", __FUNCTION__,
            ptr2->a, ptr2->b, ptr2->c);
}

