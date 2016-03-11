
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
        { \"name\": \"foo_int\", \"return_type\": \"int\" }\
    ]\
}"

static cplugin_internal_data_t *plugin_init(CPLUGIN_STARTUP_ARGS)
{
    string *s = new string();

    cout << "Pass through " << __FUNCTION__ << endl;
    s->append(__FUNCTION__);

    return reinterpret_cast<cplugin_internal_data_t *>(s);
}

static int plugin_uninit(CPLUGIN_SHUTDOWN_ARGS)
{
    string *s;
    cplugin_internal_data_t *p = CPLUGIN_GET_SHUTDOWN_ARG();

    s = reinterpret_cast<string *>(p);

    if (s != NULL) {
        cout << __FUNCTION__ << " -> " << s->c_str() << endl;
        delete s;
    } else
        cout << __FUNCTION__ << ": NULL" << endl;

    return 0;
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

