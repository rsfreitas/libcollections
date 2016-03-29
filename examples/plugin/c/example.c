
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

static int plugin_init(void)
{
    printf("pass through %s\n", __FUNCTION__);

    return 0;
}

static void plugin_uninit(void)
{
    printf("pass through %s\n", __FUNCTION__);
}

struct cplugin_entry_s plugin_example_plugin_entry = {
    "cplugin",
    "0.1",
    "Rodrigo Freitas",
    "C plugin example",
    API,
    plugin_init,
    plugin_uninit
};

CPLUGIN_SET_PLUGIN_ENTRY_NAME(plugin_example);

CPLUGIN_OBJECT_EXPORT(foo_int)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_INT, 42);
}

CPLUGIN_OBJECT_EXPORT(foo_uint)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_UINT, 420);
}

CPLUGIN_OBJECT_EXPORT(foo_sint)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_SINT, 421);
}

CPLUGIN_OBJECT_EXPORT(foo_usint)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_USINT, 4201);
}

CPLUGIN_OBJECT_EXPORT(foo_char)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_CHAR, 'a');
}

CPLUGIN_OBJECT_EXPORT(foo_uchar)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_UCHAR, 230);
}

CPLUGIN_OBJECT_EXPORT(foo_float)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_FLOAT, 42.5f);
}

CPLUGIN_OBJECT_EXPORT(foo_double)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_DOUBLE, 4.2);
}

CPLUGIN_OBJECT_EXPORT(foo_boolean)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_BOOLEAN, true);
}

CPLUGIN_OBJECT_EXPORT(foo_long)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_LONG, 42000);
}

CPLUGIN_OBJECT_EXPORT(foo_ulong)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_ULONG, 420001);
}

CPLUGIN_OBJECT_EXPORT(foo_llong)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_LLONG, 420009);
}

CPLUGIN_OBJECT_EXPORT(foo_ullong)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_ULLONG, 4200019);
}

CPLUGIN_OBJECT_EXPORT(foo_args)
{
    cvalue_t *arg1, *arg2;

    printf("Number of arguments: %d\n", CPLUGIN_ARG_COUNT());
    arg1 = CPLUGIN_ARGUMENT("arg1");
    arg2 = CPLUGIN_ARGUMENT("arg2");

    printf("arg1 value %d\n", CVALUE_INT(arg1));
    printf("arg2 value %d\n", CVALUE_INT(arg2));

    cvalue_unref(arg2);
    cvalue_unref(arg1);

    CPLUGIN_SET_RETURN_VALUE_AS_VOID();
}

