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

from abc import ABCMeta, abstractmethod

VERSION = "0.1"
__FUNCTION__ = lambda n=0: sys._getframe(n + 1).f_code.co_name

def version():
    """
    Gets the module version.
    """
    return VERSION



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



