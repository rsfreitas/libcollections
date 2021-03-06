
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 12 12:34:39 2015
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

#ifndef _COLLECTIONS_API_RANDOM_H
#define _COLLECTIONS_API_RANDOM_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <random.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_seed
 * @brief Creates a random seed to use in a pseudo-random generated number.
 *
 * This function uses the Linux random kernel file to create this seed.
 *
 * @return Returns a random seed on success or 0 otherwise.
 */
unsigned int cl_seed(void);

/**
 * @name cl_rand
 * @brief Generates a random number between 0 and \a random_max.
 *
 * One may generate numbers in a range using MIN + cl_rand(MAX - MIN).
 *
 * @param [in] random_max: The maximum value of the random number.
 *
 * @return Returns the random number.
 */
unsigned int cl_rand(unsigned int random_max);

#endif

