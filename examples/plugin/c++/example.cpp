
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

#include <iostream>
#include <cstring>
#include <cstdlib>

#include <collections.h>

using namespace std;

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
    cout << "Pass through " << __FUNCTION__ << endl;

    return 0;
}

static void plugin_uninit(void)
{
    cout << "Pass through " << __FUNCTION__ << endl;
}

struct cplugin_entry_s plugin_example_plugin_entry = {
    "cpp-plugin",
    "0.1",
    "Rodrigo Freitas",
    "C++ plugin example",
    API,
    plugin_init,
    plugin_uninit
};

CPLUGIN_SET_PLUGIN_ENTRY_NAME(plugin_example);

CPLUGIN_OBJECT_EXPORT(foo_int)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_INT, 42);
}

CPLUGIN_OBJECT_EXPORT(foo_uint)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_UINT, 420);
}

CPLUGIN_OBJECT_EXPORT(foo_sint)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_SINT, 421);
}

CPLUGIN_OBJECT_EXPORT(foo_usint)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_USINT, 4201);
}

CPLUGIN_OBJECT_EXPORT(foo_char)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_CHAR, 'a');
}

CPLUGIN_OBJECT_EXPORT(foo_uchar)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_UCHAR, 230);
}

CPLUGIN_OBJECT_EXPORT(foo_float)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_FLOAT, 42.5f);
}

CPLUGIN_OBJECT_EXPORT(foo_double)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_DOUBLE, 4.2);
}

CPLUGIN_OBJECT_EXPORT(foo_boolean)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_BOOLEAN, true);
}

CPLUGIN_OBJECT_EXPORT(foo_long)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_LONG, 42000);
}

CPLUGIN_OBJECT_EXPORT(foo_ulong)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_ULONG, 420001);
}

CPLUGIN_OBJECT_EXPORT(foo_llong)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_LLONG, 420009);
}

CPLUGIN_OBJECT_EXPORT(foo_ullong)
{
    CPLUGIN_SET_VOID_ARGS();

    cout << __FUNCTION__ << endl;
    CPLUGIN_SET_RETURN_VALUE(CL_ULLONG, 4200019);
}

CPLUGIN_OBJECT_EXPORT(foo_args)
{
    char *s;
    cobject_t *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9,
             *arg10, *arg11, *arg12, *arg13, *arg14;

    cout << "Number of arguments: " << CPLUGIN_ARG_COUNT() << endl;

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

    cout << "arg1 value: " << CVALUE_AS_INT(arg1) << endl;
    cout << "arg2 value: " << CVALUE_AS_UINT(arg2) << endl;
    cout << "arg3 value: " << CVALUE_AS_SINT(arg3) << endl;
    cout << "arg4 value: " << CVALUE_AS_USINT(arg4) << endl;
    cout << "arg5 value: " << CVALUE_AS_CHAR(arg5) << endl;
    cout << "arg6 value: " << CVALUE_AS_UCHAR(arg6) << endl;
    cout << "arg7 value: " << CVALUE_AS_FLOAT(arg7) << endl;
    cout << "arg8 value: " << CVALUE_AS_DOUBLE(arg8) << endl;
    cout << "arg9 value: " << CVALUE_AS_LONG(arg9) << endl;
    cout << "arg10 value: " << CVALUE_AS_ULONG(arg10) << endl;
    cout << "arg11 value: " << CVALUE_AS_LLONG(arg11) << endl;
    cout << "arg12 value: " << CVALUE_AS_ULLONG(arg12) << endl;
    cout << "arg13 value: " << CVALUE_AS_BOOLEAN(arg13) << endl;

    s = CVALUE_AS_STRING(arg14);
    cout << "arg14 value: " << s << endl;

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
    cobject_unref(arg1);

    CPLUGIN_SET_RETURN_VALUE_AS_VOID();
}

