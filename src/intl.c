
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Jan  3 10:00:26 2017
 * Project: libcollections
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

#include <locale.h>
#include <libintl.h>

#include "collections.h"

int intl_start(const char *package, const char *locale_dir)
{
    if (setlocale(LC_ALL, "") == NULL)
        return -1;

    if (setlocale(LC_NUMERIC, "en_US.UTF-8") == NULL)
        return -2;

    if (bindtextdomain(package, locale_dir) == NULL)
        return -3;

    if (textdomain(package) == NULL)
        return -4;

    return 0;
}

__PUB_API__ int cl_intl(const char *package, const char *locale_dir)
{
    __clib_function_init__(false, NULL, -1, -1);

    return intl_start(package, locale_dir);
}

