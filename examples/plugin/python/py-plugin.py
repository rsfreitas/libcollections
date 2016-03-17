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
    print 'Inside module_init'



def module_uninit():
    print CpluginFunctionName()
    print 'Inside module_uninit'

    return 0



class cplugin_entry_s(CpluginEntryAPI):
    def __init__(self):
        self.name = "py-plugin"
        self.version = "0.1"
        self.creator = "Rodrigo Freitas"
        self.description = "Python plugin example"
        self.api = "{\
    \"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
        { \"name\": \"foo_args\", \"return_type\": \"void\", \
            \"arguments\": [\
                { \"name\": \"arg1\", \"type\": \"int\" },\
                { \"name\": \"arg2\", \"type\": \"int\" }\
            ]\
        }\
    ]\
}"



# ########################################################## #
#                                                            #
#                       Plugin API                           #
#                                                            #
# ########################################################## #

def foo_int(caller_id, cplugin_t):
    print CpluginFunctionName()
    print caller_id
    rv = CpluginFunctionReturnValue(caller_id, cplugin_t, CpluginFunctionName())
    rv.set_return_value(CpluginValue.INT.value, 42)



def foo_args(args):
    print CpluginFunctionName()
    a = CpluginFunctionArgs(args)
    arg1 = a.argument('arg1')
    arg2 = a.argument('arg2')

    print "Arguments (arg1=%d, arg2=%d)" % (int(arg1), int(arg2))



