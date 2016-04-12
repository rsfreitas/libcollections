#!/usr/bin/python

#
# Descricao:
#
# Author: Rodrigo Freitas
# Date: Thu Oct 15 12:52:15 BRT 2015
#
# Copyright (C) 2015 Rodrigo Freitas
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA
#

import sys
from cplugin import *

def module_init():
    print CpluginFunctionName()
    print 'Inside module init'
    return 0



def module_uninit():
    print CpluginFunctionName()
    print 'Inside module uninit'



class cplugin_entry_s(CpluginEntryAPI):
    def __init__(self):
        self.name = "py-plugin"
        self.version = "0.1"
        self.creator = "Rodrigo Freitas"
        self.description = "Python plugin example"
        self.startup = "module_init"
        self.shutdown = "module_uninit"
        self.api = "{\
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
        { \"name\": \"foo_string\", \"return_type\": \"string\" },\
        { \"name\": \"foo_cstring\", \"return_type\": \"cstring\" },\
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



# ########################################################## #
#                                                            #
#                       Plugin API                           #
#                                                            #
# ########################################################## #

def foo_int(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.INT.value, 42)



def foo_uint(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.UINT.value, 420)



def foo_sint(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.SINT.value, 421)



def foo_usint(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.USINT.value, 4201)



def foo_char(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.CHAR.value, 'a')



def foo_uchar(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.UCHAR.value, 230)



def foo_float(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.FLOAT.value, 42.5)



def foo_double(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.DOUBLE.value, 4.2)



def foo_boolean(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.BOOLEAN.value, True)



def foo_long(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.LONG.value, 42000)



def foo_ulong(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.ULONG.value, 420001)



def foo_llong(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.LLONG.value, 420009)



def foo_ullong(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.ULLONG.value, 4200019)



def foo_string(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.STRING.value, "Just a String test!")



def foo_cstring(caller_id, cplugin_t):
    print CpluginFunctionName()
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.STRING.value, "Just a Cstring test!")



def foo_args(args):
    print CpluginFunctionName()
    a = CpluginFunctionArgs(args)
    arg1 = a.argument('arg1')
    arg2 = a.argument('arg2')
    arg3 = a.argument('arg3')
    arg4 = a.argument('arg4')
    arg5 = a.argument('arg5')
    arg6 = a.argument('arg6')
    arg7 = a.argument('arg7')
    arg8 = a.argument('arg8')
    arg9 = a.argument('arg9')
    arg10 = a.argument('arg10')
    arg11 = a.argument('arg11')
    arg12 = a.argument('arg12')
    arg13 = a.argument('arg13')
    arg14 = a.argument('arg14')

    print "Arguments (arg1=%d, arg2=%d, arg3=%d, arg4=%d, arg5=%c, arg6=%d, \
arg7=%f, arg8=%f, arg9=%d, arg10=%d, arg11=%d, arg12=%d, arg13=%d, arg14=%s)" % \
            (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11,\
            arg12, arg13, arg14)



