
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

#include <stdlib.h>
#include <string.h>

#include <collections.h>

#define API     "{\
    \"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" }\
    ]\
}"

static cplugin_internal_data_t *plugin_init(CPLUGIN_STARTUP_ARGS)
{
    char *s=NULL;

    printf("pass through %s\n", __FUNCTION__);
    s = strdup(__FUNCTION__);

    return s;
}

static int plugin_uninit(CPLUGIN_SHUTDOWN_ARGS)
{
    char *s;

    s = CPLUGIN_GET_SHUTDOWN_ARG();

    if (s != NULL) {
        printf("%s -> %s\n", __FUNCTION__, s);
        free(s);
    } else
        printf("%s: NULL\n", __FUNCTION__);

    return 0;
}

struct cplugin_entry_s plugin_example_plugin_entry = {
    "cplugin",
    "0.1",
    "Rodrigo Freitas",
    "C plugin example",
    API,
    plugin_init,
    plugin_uninit
};

CPLUGIN_SET_PLUGIN_ENTRY_NAME(plugin_example);

CPLUGIN_OBJECT_EXPORT(foo_int)
{
    CPLUGIN_SET_VOID_ARGS();

    printf("%s\n", __FUNCTION__);
    CPLUGIN_SET_RETURN_VALUE(CL_INT, 42);
}

