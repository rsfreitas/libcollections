
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Jan  4 11:33:46 2017
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

#ifndef _COLLECTIONS_CL_INTL_H
#define _COLLECTIONS_CL_INTL_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_intl.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _LIBINTL_H
# include <libintl.h>
#endif

/**
 * @name tr
 * @brief Macro to shorten the gettext term inside a source file.
 *
 * This macro should be used in strings that will be translated to other
 * languages.
 *
 * @param [in] string: The string.
 */
#define tr(string)                  gettext(string)

/**
 * @name tr_noop
 * @brief Macro to be used in special cases of translatable strings.
 *
 * For more details consult:
 * https://www.gnu.org/software/gettext/manual/html_node/Special-cases.html
 *
 * @param [in] string: The string.
 */
#define tr_noop(string)             string

#endif

