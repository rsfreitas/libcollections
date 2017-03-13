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
    print cplugin.__FUNCTION__()
    print 'Inside module init'
    return 0



def module_uninit():
    print cplugin.__FUNCTION__()
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


    def get_name(self):
        return self.name


    def get_version(self):
        return self.version


    def get_author(self):
        return self.author


    def get_description(self):
        return self.description


    def get_api(self):
        return json.dumps({
            'API': [
                { 'name': 'foo_char', 'return_type': 'char' },
                { 'name': 'foo_uchar', 'return_type': 'uchar' },
                { 'name': 'foo_int', 'return_type': 'int' },
                { 'name': 'foo_uint', 'return_type': 'uint' },
                { 'name': 'foo_sint', 'return_type': 'sint' },
                { 'name': 'foo_usint', 'return_type': 'usint' },
                { 'name': 'foo_float', 'return_type': 'float' },
                { 'name': 'foo_double', 'return_type': 'double' },
                { 'name': 'foo_long', 'return_type': 'long' },
                { 'name': 'foo_ulong', 'return_type': 'ulong' },
                { 'name': 'foo_llong', 'return_type': 'llong' },
                { 'name': 'foo_ullong', 'return_type': 'ullong' },
                { 'name': 'foo_boolean', 'return_type': 'boolean' },
                { 'name': 'foo_string', 'return_type': 'string' },
                { 'name': 'foo_cstring', 'return_type': 'string' },
                { 'name': 'foo_args', 'return_type': 'void',
                    'arguments': [
                        { 'name': 'arg1', 'type': 'int' },
                        { 'name': 'arg2', 'type': 'uint' },
                        { 'name': 'arg3', 'type': 'sint' },
                        { 'name': 'arg4', 'type': 'usint' },
                        { 'name': 'arg5', 'type': 'char' },
                        { 'name': 'arg6', 'type': 'uchar' },
                        { 'name': 'arg7', 'type': 'float' },
                        { 'name': 'arg8', 'type': 'double' },
                        { 'name': 'arg9', 'type': 'long' },
                        { 'name': 'arg10', 'type': 'ulong' },
                        { 'name': 'arg11', 'type': 'llong' },
                        { 'name': 'arg12', 'type': 'ullong' },
                        { 'name': 'arg13', 'type': 'boolean' },
                        { 'name': 'arg14', 'type': 'string' },
                    ]
                },
                { 'name': 'foo_class', 'return_type': 'pointer',
                    'arguments': [
                        { 'name': 'data', 'type': 'string' }
                    ]
                },
                { 'name': 'foo_pointer', 'return_type': 'int',
                    'arguments': [
                        { 'name': 'data', 'type': 'pointer' }
                    ]
                }
            ]
        })


    def get_startup(self):
        return self.startup


    def get_shutdown(self):
        return self.shutdown



# ########################################################## #
#                                                            #
#                       Plugin API                           #
#                                                            #
# ########################################################## #

def foo_int():
    return 42



def foo_uint():
    return 420



def foo_sint():
    return 421



def foo_usint():
    return 4201



def foo_char():
    return 'a'



def foo_uchar():
    return 230



def foo_float():
    return 42.5



def foo_double():
    return 4.2



def foo_long():
    return 42000



def foo_ulong():
    return 420001



def foo_llong():
    return 420009



def foo_ullong():
    return 4200019



def foo_boolean():
    return False



def foo_string():
    return "Just a String test!"



def foo_cstring():
    return "Just a Cstring test!"



def foo_args(args):
    print args



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



def foo_class(args):
    print '--',cplugin.__FUNCTION__()
    t = Test(args)
    print t.get_value()
    return t



def foo_pointer(ptr):
    print '--',cplugin.__FUNCTION__()
    data = ptr
    print data.data
    return data.get_value()



