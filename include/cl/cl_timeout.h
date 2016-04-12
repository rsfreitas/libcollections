
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Apr 12 13:35:23 2016
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

#ifndef _COLLECTIONS_CL_TIMEOUT_H
#define _COLLECTIONS_CL_TIMEOUT_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_timeout.h> directly; include <collections.h> instead."
# endif
#endif

/** Types of timeout interval checks */
enum ctimeout {
    CL_TM_SECONDS,
    CL_TM_MSECONDS,
    CL_TM_USECONDS
};

/**
 * @name ctimeout_create
 * @brief Starts a timeout verification with a specific precision.
 *
 * @param [in] interval: Timeout expiration interval.
 * @param [in] precision: The accuracy of timeout expiration check.
 *
 * @return On success returns a ctimeout_t object with the timeout info or
 *         NULL otherwise.
 */
ctimeout_t *ctimeout_create(unsigned int interval, enum ctimeout precision);

/**
 * @name ctimeout_destroy
 * @brief Releases a ctimeout_t object from memory.
 *
 * @param [in] t: The ctimeout_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int ctimeout_destroy(ctimeout_t *t);

/**
 * @name ctimeout_reset
 * @brief Resets a ctimeout_t object info.
 *
 * @param [in,out] t: The ctimeout_t object.
 * @param [in] interval: The new timeout expiration interval.
 * @param [in] precision: The new accuracy of timeout expiration check.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int ctimeout_reset(ctimeout_t *t, unsigned int interval,
                   enum ctimeout precision);

/**
 * @name ctimeout_expired
 * @brief Checks whether a timeout has expired or not.
 *
 * @param [in] t: The ctimeout_t object.
 *
 * @return Returns true or false if the timeout had expired or not.
 */
bool ctimeout_expired(const ctimeout_t *t);

#endif

