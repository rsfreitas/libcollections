
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 18:34:01 BRST 2015
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

#include <stdarg.h>
#include <string.h>

#include "collections.h"
#include "plugin.h"

/**
 * @name cplugin_get_return_value
 * @brief Gets the return value from a function previously called.
 *
 * @param [in] cpl: The cplugin_t object.
 * @param [in] function_name: Called function name.
 * @param [in] caller_id: Identification function number.
 *
 * @return On success returns a cplugin_value_t object with the function
 *         return value or NULL otherwise. The user is responsible for release
 *         the cplugin_value_t object.
 */
cvalue_t *cplugin_get_return_value(cplugin_t *cpl,
    const char *function_name, uint32_t caller_id)
{
    struct cplugin_function_s *foo = NULL;
    struct cplugin_fdata_s *return_value = NULL;

    foo = cdll_map(cpl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
        return NULL;
    }

    /*
     * Search for a return value structure with the same @caller_id.
     */
    pthread_mutex_lock(&foo->m_return_value);
    return_value = cdll_filter(&foo->values, search_cplugin_fdata_s_by_caller_id,
                               &caller_id);

    pthread_mutex_unlock(&foo->m_return_value);

    if (NULL == return_value) {
        cset_errno(CL_VALUE_NOT_FOUND);
        return NULL;
    }

    return cvalue_ref(return_value->value);
}

