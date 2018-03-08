
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

#define API     "{\
    \"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
        { \"name\": \"foo_uint\", \"return_type\": \"uint\" },\
        { \"name\": \"foo_char\", \"return_type\": \"char\" },\
        { \"name\": \"foo_uchar\", \"return_type\": \"uchar\" },\
        { \"name\": \"foo_sint\", \"return_type\": \"sint\" },\
        { \"name\": \"foo_usint\", \"return_type\": \"usint\" },\
        { \"name\": \"foo_float\", \"return_type\": \"float\" },\
        { \"name\": \"foo_double\", \"return_type\": \"double\" },\
        { \"name\": \"foo_long\", \"return_type\": \"long\" },\
        { \"name\": \"foo_ulong\", \"return_type\": \"ulong\" },\
        { \"name\": \"foo_llong\", \"return_type\": \"llong\" },\
        { \"name\": \"foo_ullong\", \"return_type\": \"ullong\" },\
        { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" },\
        { \"name\": \"foo_args\", \"return_type\": \"void\", \"arguments\": [\
            { \"name\": \"arg1\", \"type\": \"int\" },\
            { \"name\": \"arg2\", \"type\": \"uint\" },\
            { \"name\": \"arg3\", \"type\": \"sint\" },\
            { \"name\": \"arg4\", \"type\": \"usint\" },\
            { \"name\": \"arg5\", \"type\": \"char\" },\
            { \"name\": \"arg6\", \"type\": \"uchar\" },\
            { \"name\": \"arg7\", \"type\": \"float\" },\
            { \"name\": \"arg8\", \"type\": \"double\" },\
            { \"name\": \"arg9\", \"type\": \"long\" },\
            { \"name\": \"arg10\", \"type\": \"ulong\" },\
            { \"name\": \"arg11\", \"type\": \"llong\" },\
            { \"name\": \"arg12\", \"type\": \"ullong\" },\
            { \"name\": \"arg13\", \"type\": \"boolean\" },\
            { \"name\": \"arg14\", \"type\": \"string\" }\
            ] }\
    ]\
}"

#define API2     "{\
    \"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
        { \"name\": \"foo_uint\", \"return_type\": \"uint\" },\
        { \"name\": \"foo_char\", \"return_type\": \"char\" },\
        { \"name\": \"foo_uchar\", \"return_type\": \"uchar\" },\
        { \"name\": \"foo_sint\", \"return_type\": \"sint\" },\
        { \"name\": \"foo_usint\", \"return_type\": \"usint\" },\
        { \"name\": \"foo_float\", \"return_type\": \"float\" },\
        { \"name\": \"foo_double\", \"return_type\": \"double\" },\
        { \"name\": \"foo_long\", \"return_type\": \"long\" },\
        { \"name\": \"foo_ulong\", \"return_type\": \"ulong\" },\
        { \"name\": \"foo_llong\", \"return_type\": \"llong\" },\
        { \"name\": \"foo_ullong\", \"return_type\": \"ullong\" },\
        { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" }\
    ]\
}"

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
    teste,
    "0.1",
    "Rodrigo Freitas",
    "C plugin example",
    API
)

CL_PLUGIN_OBJECT_VOID(int, foo_int)
{
    printf("%s\n", __FUNCTION__);

    return 42;
}

CL_PLUGIN_OBJECT_VOID(unsigned int, foo_uint)
{
    printf("%s\n", __FUNCTION__);
    return 420;
}

CL_PLUGIN_OBJECT_VOID(short int, foo_sint)
{
    printf("%s\n", __FUNCTION__);
    return 421;
}

CL_PLUGIN_OBJECT_VOID(unsigned short int, foo_usint)
{
    printf("%s\n", __FUNCTION__);
    return 4201;
}

CL_PLUGIN_OBJECT_VOID(char, foo_char)
{
    printf("%s\n", __FUNCTION__);
    return 'a';
}

CL_PLUGIN_OBJECT_VOID(unsigned char, foo_uchar)
{
    printf("%s\n", __FUNCTION__);
    return 230;
}

CL_PLUGIN_OBJECT_VOID(float, foo_float)
{
    printf("%s\n", __FUNCTION__);
    return 42.5f;
}

CL_PLUGIN_OBJECT_VOID(double, foo_double)
{
    printf("%s\n", __FUNCTION__);
    return 4.2;
}

CL_PLUGIN_OBJECT_VOID(bool, foo_boolean)
{
    printf("%s\n", __FUNCTION__);
    return true;
}

CL_PLUGIN_OBJECT_VOID(long, foo_long)
{
    printf("%s\n", __FUNCTION__);
    return 42000;
}

CL_PLUGIN_OBJECT_VOID(unsigned long, foo_ulong)
{
    printf("%s\n", __FUNCTION__);
    return 420001;
}

CL_PLUGIN_OBJECT_VOID(long long, foo_llong)
{
    printf("%s\n", __FUNCTION__);
    return 420009;
}

CL_PLUGIN_OBJECT_VOID(unsigned long long, foo_ullong)
{
    printf("%s\n", __FUNCTION__);
    return 4200019;
}

CL_PLUGIN_OBJECT_ARGS_ONLY(void, foo_args)
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
}

CL_PLUGIN_OBJECT_VOID(void, outside_api)
{
    printf("%s\n", __FUNCTION__);
}

CL_PLUGIN_OBJECT_ARGS_ONLY(int, another_outside_api)
{
    CL_PLUGIN_LOAD_ARGUMENTS();

    printf("%s\n", __FUNCTION__);
    printf("Argument 'arg1': %d\n", CL_PLUGIN_ARGUMENT_INT("arg1"));

    CL_PLUGIN_UNLOAD_ARGUMENTS();

    return 42;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, foo_pointer)
{
    struct ptr_arg *ptr2 = CL_PLUGIN_PTR_ARGUMENT();

    printf("%s: a=%d, b=%d, c=%d\n", __FUNCTION__,
            ptr2->a, ptr2->b, ptr2->c);
}

