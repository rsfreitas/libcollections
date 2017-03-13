
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

CPLUGIN_INIT()
{
    printf("pass through %s\n", __FUNCTION__);
    return 0;
}

CPLUGIN_UNINIT()
{
    printf("pass through %s\n", __FUNCTION__);
}

CPLUGIN_SET_INFO(
    teste,
    "0.1",
    "Rodrigo Freitas",
    "C plugin example",
    API
)

CPLUGIN_OBJECT_VOID(int, foo_int)
{
    printf("%s\n", __FUNCTION__);

    return 42;
}

CPLUGIN_OBJECT_VOID(unsigned int, foo_uint)
{
    printf("%s\n", __FUNCTION__);
    return 420;
}

CPLUGIN_OBJECT_VOID(short int, foo_sint)
{
    printf("%s\n", __FUNCTION__);
    return 421;
}

CPLUGIN_OBJECT_VOID(unsigned short int, foo_usint)
{
    printf("%s\n", __FUNCTION__);
    return 4201;
}

CPLUGIN_OBJECT_VOID(char, foo_char)
{
    printf("%s\n", __FUNCTION__);
    return 'a';
}

CPLUGIN_OBJECT_VOID(unsigned char, foo_uchar)
{
    printf("%s\n", __FUNCTION__);
    return 230;
}

CPLUGIN_OBJECT_VOID(float, foo_float)
{
    printf("%s\n", __FUNCTION__);
    return 42.5f;
}

CPLUGIN_OBJECT_VOID(double, foo_double)
{
    printf("%s\n", __FUNCTION__);
    return 4.2;
}

CPLUGIN_OBJECT_VOID(bool, foo_boolean)
{
    printf("%s\n", __FUNCTION__);
    return true;
}

CPLUGIN_OBJECT_VOID(long, foo_long)
{
    printf("%s\n", __FUNCTION__);
    return 42000;
}

CPLUGIN_OBJECT_VOID(unsigned long, foo_ulong)
{
    printf("%s\n", __FUNCTION__);
    return 420001;
}

CPLUGIN_OBJECT_VOID(long long, foo_llong)
{
    printf("%s\n", __FUNCTION__);
    return 420009;
}

CPLUGIN_OBJECT_VOID(unsigned long long, foo_ullong)
{
    printf("%s\n", __FUNCTION__);
    return 4200019;
}

CPLUGIN_OBJECT_ARGS_ONLY(void, foo_args)
{
    char *s;
//    cobject_t *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9,
//             *arg10, *arg11, *arg12, *arg13, *arg14;

    CPLUGIN_LOAD_ARGUMENTS();

    {
        cstring_t *tmp = cjson_to_cstring(___jargs, false);
        printf("'%s'\n", cstring_valueof(tmp));
        cstring_unref(tmp);
    }
    printf("arg1 value %d\n", CPLUGIN_ARGUMENT_INT("arg1"));
    printf("arg2 value %d\n", CPLUGIN_ARGUMENT_UINT("arg2"));
    printf("arg3 value %d\n", CPLUGIN_ARGUMENT_SINT("arg3"));
    printf("arg4 value %d\n", CPLUGIN_ARGUMENT_USINT("arg4"));
    printf("arg5 value %c\n", CPLUGIN_ARGUMENT_CHAR("arg5"));
    printf("arg6 value %d\n", CPLUGIN_ARGUMENT_UCHAR("arg6"));
    printf("arg7 value %f\n", CPLUGIN_ARGUMENT_FLOAT("arg7"));
    printf("arg8 value %f\n", CPLUGIN_ARGUMENT_DOUBLE("arg8"));
    printf("arg9 value %ld\n", CPLUGIN_ARGUMENT_LONG("arg9"));
    printf("arg10 value %lu\n", CPLUGIN_ARGUMENT_ULONG("arg10"));
    printf("arg11 value %lld\n", CPLUGIN_ARGUMENT_LLONG("arg11"));
    printf("arg12 value %llu\n", CPLUGIN_ARGUMENT_ULLONG("arg12"));
    printf("arg13 value %d\n", CPLUGIN_ARGUMENT_BOOL("arg13"));
    printf("arg14 value %s\n", CPLUGIN_ARGUMENT_STRING("arg14"));

/*    printf("Number of arguments: %d\n", CPLUGIN_ARG_COUNT());
    arg1 = CPLUGIN_ARGUMENT("arg1");
    arg2 = CPLUGIN_ARGUMENT("arg2");
    arg3 = CPLUGIN_ARGUMENT("arg3");
    arg4 = CPLUGIN_ARGUMENT("arg4");
    arg5 = CPLUGIN_ARGUMENT("arg5");
    arg6 = CPLUGIN_ARGUMENT("arg6");
    arg7 = CPLUGIN_ARGUMENT("arg7");
    arg8 = CPLUGIN_ARGUMENT("arg8");
    arg9 = CPLUGIN_ARGUMENT("arg9");
    arg10 = CPLUGIN_ARGUMENT("arg10");
    arg11 = CPLUGIN_ARGUMENT("arg11");
    arg12 = CPLUGIN_ARGUMENT("arg12");
    arg13 = CPLUGIN_ARGUMENT("arg13");
    arg14 = CPLUGIN_ARGUMENT("arg14");

    printf("arg1 value %d\n", COBJECT_AS_INT(arg1));
    printf("arg2 value %d\n", COBJECT_AS_UINT(arg2));
    printf("arg3 value %d\n", COBJECT_AS_SINT(arg3));
    printf("arg4 value %d\n", COBJECT_AS_USINT(arg4));
    printf("arg5 value %c\n", COBJECT_AS_CHAR(arg5));
    printf("arg6 value %d\n", COBJECT_AS_UCHAR(arg6));
    printf("arg7 value %f\n", COBJECT_AS_FLOAT(arg7));
    printf("arg8 value %f\n", COBJECT_AS_DOUBLE(arg8));
    printf("arg9 value %ld\n", COBJECT_AS_LONG(arg9));
    printf("arg10 value %lu\n", COBJECT_AS_ULONG(arg10));
    printf("arg11 value %lld\n", COBJECT_AS_LLONG(arg11));
    printf("arg12 value %llu\n", COBJECT_AS_ULLONG(arg12));
    printf("arg13 value %d\n", COBJECT_AS_BOOLEAN(arg13));

    s = COBJECT_AS_STRING(arg14);
    printf("arg14 value %s\n", s);

    cobject_unref(arg14);
    cobject_unref(arg13);
    cobject_unref(arg12);
    cobject_unref(arg11);
    cobject_unref(arg10);
    cobject_unref(arg9);
    cobject_unref(arg8);
    cobject_unref(arg7);
    cobject_unref(arg6);
    cobject_unref(arg5);
    cobject_unref(arg4);
    cobject_unref(arg3);
    cobject_unref(arg2);
    cobject_unref(arg1);*/

    CPLUGIN_UNLOAD_ARGUMENTS();
}

