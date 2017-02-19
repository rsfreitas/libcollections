#!/usr/bin/python

#
# Description:
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
import json
import cl_plugin as cplugin

def module_init():
    print cplugin.CpluginFunctionName()
    print 'Inside module init'
    return 0



def module_uninit():
    print cplugin.CpluginFunctionName()
    print 'Inside module uninit'
    return 0



class CpluginMainEntry(cplugin.CpluginEntryAPI):
    def __init__(self):
        self.name = "py-plugin"
        self.version = "0.1"
        self.author = "Rodrigo Freitas"
        self.description = "Python plugin example"
        self.startup = "module_init"
        self.shutdown = "module_uninit"
        self.api = self._populate_api()


    def _populate_api(self):
        api = cplugin.CpluginAPI()

        api.add_function('foo_int', 'int')
        api.add_function('foo_uint', 'uint')
        api.add_function('foo_sint', 'sint')
        api.add_function('foo_usint', 'usint')
        api.add_function('foo_char', 'char')
        api.add_function('foo_uchar', 'uchar')
        api.add_function('foo_float', 'float')
        api.add_function('foo_double', 'double')
        api.add_function('foo_long', 'long')
        api.add_function('foo_ulong', 'ulong')
        api.add_function('foo_llong', 'llong')
        api.add_function('foo_ullong', 'ullong')
        api.add_function('foo_boolean', 'boolean')
        api.add_function('foo_string', 'string')
        api.add_function('foo_cstring', 'cstring')

        api.add_function('foo_class', 'pointer')
        api.add_argument('foo_class', 'data', 'string')

        api.add_function('foo_pointer', 'int')
        api.add_argument('foo_pointer', 'data', 'pointer')

        api.add_function('foo_args', 'void')
        api.add_argument('foo_args', 'arg1', 'int')
        api.add_argument('foo_args', 'arg2', 'uint')
        api.add_argument('foo_args', 'arg3', 'sint')
        api.add_argument('foo_args', 'arg4', 'usint')
        api.add_argument('foo_args', 'arg5', 'char')
        api.add_argument('foo_args', 'arg6', 'uchar')
        api.add_argument('foo_args', 'arg7', 'float')
        api.add_argument('foo_args', 'arg8', 'double')
        api.add_argument('foo_args', 'arg9', 'long')
        api.add_argument('foo_args', 'arg10', 'ulong')
        api.add_argument('foo_args', 'arg11', 'llong')
        api.add_argument('foo_args', 'arg12', 'ullong')
        api.add_argument('foo_args', 'arg13', 'boolean')
        api.add_argument('foo_args', 'arg14', 'string')

        return api


    def get_name(self):
        return self.name


    def get_version(self):
        return self.version


    def get_author(self):
        return self.author


    def get_description(self):
        return self.description


    def get_api(self):
        return self.api.export() #json.dumps({'API': self.populate_api()}) #self.api


    def get_startup(self):
        return self.startup


    def get_shutdown(self):
        return self.shutdown



# ########################################################## #
#                                                            #
#                       Plugin API                           #
#                                                            #
# ########################################################## #

def foo_int(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.INT.value, 42)



def foo_uint(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.UINT.value, 420)



def foo_sint(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.SINT.value, 421)



def foo_usint(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.USINT.value, 4201)



def foo_char(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.CHAR.value, 'a')



def foo_uchar(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.UCHAR.value, 230)



def foo_float(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.FLOAT.value, 42.5)



def foo_double(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.DOUBLE.value, 4.2)



def foo_boolean(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.BOOLEAN.value, True)



def foo_long(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.LONG.value, 42000)



def foo_ulong(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.ULONG.value, 420001)



def foo_llong(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.LLONG.value, 420009)



def foo_ullong(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.ULLONG.value, 4200019)



def foo_string(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.STRING.value,
                        "Just a String test!")



def foo_cstring(caller_id, cplugin_t):
    print cplugin.CpluginFunctionName()
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.STRING.value,
                        "Just a Cstring test!")



def foo_args(args):
    print cplugin.CpluginFunctionName()
    a = cplugin.CpluginFunctionArguments(args)
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



class Test(object):
    def __init__(self, data):
        self.data = data
        self._x = 42
        self.a = 1
        self.b = 2
        self.c = 3


    def __str__(self):
        print "#Test class:",self.data


    def get_value(self):
        return self._x


def foo_class(caller_id, cplugin_t, args):
    print '--',cplugin.CpluginFunctionName()
    a = cplugin.CpluginFunctionArguments(args)
    data = a.argument('data')
    print data
    t = Test(data)
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.POINTER.value, t)



def foo_pointer(caller_id, cplugin_t, args):
    print '--',cplugin.CpluginFunctionName()
    a = cplugin.CpluginFunctionArguments(args)
    data = a.argument('data')
    print data.a,data.b,data.c
    rv = cplugin.CpluginFunctionReturnValue(caller_id, cplugin_t,
                                            cplugin.CpluginFunctionName())

    rv.set_return_value(cplugin.CpluginValue.INT.value, data.get_value())



if __name__ == '__main__':
    c = CpluginMainEntry()
    print c.get_api()



