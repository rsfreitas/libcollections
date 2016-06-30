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
import _cplugin

from enum import Enum
from abc import ABCMeta, abstractmethod

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
    def get_creator(self):
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
        return _cplugin.set_return_value(self.caller_id, self.cplugin_t,
                                         self.function_name, type_of_return,
                                         str(value))



class CpluginFunctionArgs(object):
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



