#!/usr/bin/python

#
# Description: libcollections python extension to be used inside python plugins.
#
# Author: Rodrigo Freitas
# Date: Wed Aug 12 12:56:01 BRT 2015
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

"""
Libcollections python extension to be used inside python plugins.
"""

import sys
import json

from enum import Enum
from abc import ABCMeta, abstractmethod

import _cplugin

CpluginFunctionName = lambda n=0: sys._getframe(n + 1).f_code.co_name

class AutoNumber(Enum):
    def __new__(cls):
        value = len(cls.__members__)
        obj = object.__new__(cls)
        obj._value_ = value
        return obj



class CpluginValue(AutoNumber):
    """ Function return types """
    __order__ = 'VOID CHAR UCHAR INT UINT SINT USINT FLOAT DOUBLE \
LONG ULONG LLONG ULLONG POINTER STRING BOOLEAN'
    VOID = ()
    CHAR = ()
    UCHAR = ()
    INT = ()
    UINT = ()
    SINT = ()
    USINT = ()
    FLOAT = ()
    DOUBLE = ()
    LONG = ()
    ULONG = ()
    LLONG = ()
    ULLONG = ()
    POINTER = ()
    STRING = ()
    BOOLEAN = ()
    CSTRING = ()    # unsupported



class CpluginArg(AutoNumber):
    """ Function argument types """
    __order__ = 'ARG_FIXED ARG_VAR NO_ARGS'
    ARG_FIXED = ()
    ARG_VAR = ()
    NO_ARGS = ()



class CpluginEntryAPI(object):
    """
        A class to be inherited by the mandatory plugin class named as
        'CpluginMainEntry', so that a plugin manager may get all plugin
        informations and its API.
    """

    __metaclass__ = ABCMeta

    @abstractmethod
    def get_name(self):
        """
            Function to return the module name.
        """
        pass


    @abstractmethod
    def get_version(self):
        """
            Function to return the module version.
        """
        pass


    # TODO: Rename to author
    @abstractmethod
    def get_author(self):
        """
            Function to return the module author name.
        """
        pass


    @abstractmethod
    def get_description(self):
        """
            Function to return the module description.
        """
        pass


    @abstractmethod
    def get_api(self):
        """
            Function to return the module description.
        """
        pass


    # TODO: Put these into another abstract class
    @abstractmethod
    def get_startup(self):
        """
            Function to return the name of the function called when the
            plugin is loaded.
        """
        pass


    @abstractmethod
    def get_shutdown(self):
        """
            Function to return the name of the function called when the
            plugin is unloaded.
        """
        pass



class CpluginFunctionReturnValue(object):
    """
        This class is used to set the return value from an exported function
        from a plugin. It requires the 'caller_id' and the 'cplugin_t'
        arguments.
    """
    def __init__(self, caller_id, cplugin_t, function_name):
        self.caller_id = caller_id
        self.cplugin_t = cplugin_t
        self.function_name = function_name


    def set_return_value(self, type_of_return, value):
        """
        Sets the return value from a plugin function, so the caller may get it.
        If we're returning an object (such a instance from a python class) we
        send it as itsef otherwise it goas as a string.
        """
        if type_of_return in (CpluginValue.POINTER.value, \
                CpluginValue.CSTRING.value):
            return_value = value
        else:
            return_value = str(value)

        return _cplugin.set_return_value(self.caller_id, self.cplugin_t,
                                         self.function_name, type_of_return,
                                         return_value)



class CpluginFunctionArguments(object):
    """
        This class is used to extract arguments received by functions called
        from a plugin manager.

        In a python plugin, a function may receive none or up to 3 arguments.
        If its return value is void, it may receive none (void arguments) or
        1 argument, called 'args'. Otherwise, it may receive 2 if it does not
        have any argument (caller_id, cplugin_t) or 3 if it does (caller_id,
        cplugin_t, args).
    """
    def __init__(self, args):
        self.args = args


    def argument(self, argument_name):
        return _cplugin.argument(self.args, argument_name)


    def arg_count(self):
        return _cplugin.arg_count(self.args)



class CpluginAPI(object):
    """
    A class to easy the creation of an API from within a plugin.
    """
    def __init__(self):
        self._functions = list()


    def add_function(self, name, return_type):
        """
        Adds a function inside the API with a name and a return type.

        :param name: The function name.
        :param return_type: The type of the function returned value.
        """
        self._functions.append({
            'name': name,
            'return_type': return_type
        })


    def add_argument(self, function_name, arg_name, arg_type):
        """
        Adds an argument to a function inside the API. This argument must have
        a name and a type.

        Raises an Exception if the function does not exist inside the API.

        :param function_name: The function name which will have a new argument.
        :param arg_name: The argument name.
        :param arg_type: The argument type.
        """
        foo = next(
            (item for item in self._functions if item['name'] == function_name),
            None
        )

        if foo is None:
            raise Exception("Function '%s' not created yet" % function_name)

        if not foo.has_key('arguments'):
            foo['arguments'] = list()

        foo['arguments'].append({
            'name': arg_name,
            'type': arg_type
        })


    def export(self):
        """
        Exports the API to the libcollection's api format.
        """
        return json.dumps({'API': self._functions})



