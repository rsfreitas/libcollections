
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

CL_PLUGIN_INIT()
{
    cout << "Pass through " << __FUNCTION__ << endl;
    return 0;
}

CL_PLUGIN_UNINIT()
{
    cout << "Pass through " << __FUNCTION__ << endl;
}

CL_PLUGIN_SET_INFO(
    cpp_plugin,
    "0.1",
    "Rodrigo Freitas",
    "C++ plugin example",
    API
)

CL_PLUGIN_OBJECT_VOID(int, foo_int)
{
    cout << __FUNCTION__ << endl;
    return 42;
}

CL_PLUGIN_OBJECT_VOID(unsigned int, foo_uint)
{
    cout << __FUNCTION__ << endl;
    return 420;
}

CL_PLUGIN_OBJECT_VOID(short int, foo_sint)
{
    cout << __FUNCTION__ << endl;
    return 421;
}

CL_PLUGIN_OBJECT_VOID(unsigned short int, foo_usint)
{
    cout << __FUNCTION__ << endl;
    return 4201;
}

CL_PLUGIN_OBJECT_VOID(char, foo_char)
{
    cout << __FUNCTION__ << endl;
    return 'a';
}

CL_PLUGIN_OBJECT_VOID(unsigned char, foo_uchar)
{
    cout << __FUNCTION__ << endl;
    return 230;
}

CL_PLUGIN_OBJECT_VOID(float, foo_float)
{
    cout << __FUNCTION__ << endl;
    return 42.5f;
}

CL_PLUGIN_OBJECT_VOID(double, foo_double)
{
    cout << __FUNCTION__ << endl;
    return 4.2;
}

CL_PLUGIN_OBJECT_VOID(bool, foo_boolean)
{
    cout << __FUNCTION__ << endl;
    return true;
}

CL_PLUGIN_OBJECT_VOID(long, foo_long)
{
    cout << __FUNCTION__ << endl;
    return 42000;
}

CL_PLUGIN_OBJECT_VOID(unsigned long, foo_ulong)
{
    cout << __FUNCTION__ << endl;
    return 420001;
}

CL_PLUGIN_OBJECT_VOID(long long, foo_llong)
{
    cout << __FUNCTION__ << endl;
    return 420009;
}

CL_PLUGIN_OBJECT_VOID(unsigned long long, foo_ullong)
{
    cout << __FUNCTION__ << endl;
    return 4200019;
}

CL_PLUGIN_OBJECT_ARGS_ONLY(void, foo_args)
{

    CL_PLUGIN_LOAD_ARGUMENTS();

    cout << "arg1 value: " << CL_PLUGIN_ARGUMENT_INT("arg1") << endl;
    cout << "arg2 value: " << CL_PLUGIN_ARGUMENT_UINT("arg2") << endl;
    cout << "arg3 value: " << CL_PLUGIN_ARGUMENT_SINT("arg3") << endl;
    cout << "arg4 value: " << CL_PLUGIN_ARGUMENT_USINT("arg4") << endl;
    cout << "arg5 value: " << CL_PLUGIN_ARGUMENT_CHAR("arg5") << endl;
    cout << "arg6 value: " << CL_PLUGIN_ARGUMENT_UCHAR("arg6") << endl;
    cout << "arg7 value: " << CL_PLUGIN_ARGUMENT_FLOAT("arg7") << endl;
    cout << "arg8 value: " << CL_PLUGIN_ARGUMENT_DOUBLE("arg8") << endl;
    cout << "arg9 value: " << CL_PLUGIN_ARGUMENT_LONG("arg9") << endl;
    cout << "arg10 value: " << CL_PLUGIN_ARGUMENT_ULONG("arg10") << endl;
    cout << "arg11 value: " << CL_PLUGIN_ARGUMENT_LLONG("arg11") << endl;
    cout << "arg12 value: " << CL_PLUGIN_ARGUMENT_ULLONG("arg12") << endl;
    cout << "arg13 value: " << CL_PLUGIN_ARGUMENT_BOOL("arg13") << endl;
    cout << "arg14 value: " << CL_PLUGIN_ARGUMENT_STRING("arg14") << endl;

    CL_PLUGIN_UNLOAD_ARGUMENTS();
}

