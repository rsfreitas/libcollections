
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Jan 13 11:08:37 2016
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

#ifndef _COLLECTIONS_API_COUNTER_H
#define _COLLECTIONS_API_COUNTER_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <counter.h> directly; include <collections.h> instead."
# endif
#endif

/** Counter precisions */
enum cl_counter_precision {
    CL_8BIT_COUNTER,
    CL_16BIT_COUNTER,
    CL_32BIT_COUNTER,
    CL_64BIT_COUNTER
};

/**
 * @name cl_counter_create
 * @brief Creates a new counter.
 *
 * The counter created is for simple statistics counter. If an application
 * needs to known how many times something occours by counting it.
 *
 * The counter's value goes from \a min to \a max, and if \a circular is
 * false it'll stop in its limits when reached it.
 *
 * @param [in] precision: The counter bits precision.
 * @param [in] min: The minimum value of the counter.
 * @param [in] max: The maximum value of the counter.
 * @param [in] start_value: The counter initial value.
 * @param [in] circular: Boolean flag to indicate an automatic reset when
 *                       the limits are reached it.
 *
 * @return On success returns a cl_counter_t object or NULL otherwise.
 */
cl_counter_t *cl_counter_create(enum cl_counter_precision precision,
                                long long min, long long max,
                                long long start_value, bool circular);

/**
 * @name cl_counter_destroy
 * @brief Destroy a counter.
 *
 * @param [in] c: The counter which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_destroy(cl_counter_t *c);

/**
 * @name cl_counter_ref
 * @brief Increases the reference count for a cl_counter_t item.
 *
 * @param [in,out] c: The cl_counter_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_counter_t *cl_counter_ref(cl_counter_t *c);

/**
 * @name cl_counter_unref
 * @brief Decreases the reference count for a cl_counter_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] c: The cl_counter_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_unref(cl_counter_t *c);

/**
 * @name cl_counter_increase
 * @brief Increases the counter value by 1.
 *
 * @param [in,out] c: The counter.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_increase(cl_counter_t *c);

/**
 * @name cl_counter_increase_by
 * @brief Increases the counter value by the specified value.
 *
 * @param [in,out] c: The counter.
 * @param [in] gap: The added value to the counter.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_increase_by(cl_counter_t *c, long long gap);

/**
 * @name cl_counter_decrease
 * @brief Decreases the counter value by 1.
 *
 * @param [in,out] c: The counter.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_decrease(cl_counter_t *c);

/**
 * @name cl_counter_decrease_by
 * @brief Decreases the counter value by the specified value.
 *
 * @param [in,out] c: The counter.
 * @param [in] gap: The subtracted value to the counter.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_decrease_by(cl_counter_t *c, long long gap);

/**
 * @name cl_counter_reset
 * @brief Returns the counter value to its the start value.
 *
 * @param [in,out] c: The counter.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_reset(cl_counter_t *c);

/**
 * @name cl_counter_get
 * @brief Gets the actual value from the counter.
 *
 * @param [in,out] c: The counter.
 *
 * @return On success returns the current counter value or -1 otherwise.
 */
long long cl_counter_get(cl_counter_t *c);

/**
 * @name cl_counter_set_min
 * @brief Updates the counter minimum value.
 *
 * @param [in,out] c: The counter.
 * @param [in] min: The new minimum value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_set_min(cl_counter_t *c, long long min);

/**
 * @name cl_counter_set_max
 * @brief Updates the counter maximum value.
 *
 * @param [in,out] c: The counter.
 * @param [in] max: The new maximum value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_set_max(cl_counter_t *c, long long max);

/**
 * @name cl_counter_set_range
 * @brief Updates the counter minimum and maximum values.
 *
 * @param [in,out] c: The counter.
 * @param [in] min: The new minimum value.
 * @param [in] max: The new maximum value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_set_range(cl_counter_t *c, long long min, long long max);

/**
 * @name cl_counter_lt
 * @brief Compares if a counter value is less than other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is less than value returns true otherwise
 *         returns false.
 */
bool cl_counter_lt(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_le
 * @brief Compares if a counter value is less than or equal other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is less than or equal the value returns true
 *         otherwise returns false.
 */
bool cl_counter_le(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_lt
 * @brief Compares if a counter value is greater than other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is greater than value returns true otherwise
 *         returns false.
 */
bool cl_counter_gt(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_le
 * @brief Compares if a counter value is greater than or equal other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is greater than or equal the value returns true
 *         otherwise returns false.
 */
bool cl_counter_ge(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_eq
 * @brief Compares if a counter value is equal other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is equal value returns true otherwise
 *         returns false.
 */
bool cl_counter_eq(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_ne
 * @brief Compares if a counter value is not equal other value.
 *
 * @param [in] c: The counter.
 * @param [in] value: The value.
 *
 * @return If the counter value is not equal value returns true otherwise
 *         returns false.
 */
bool cl_counter_ne(const cl_counter_t *c, long long value);

/**
 * @name cl_counter_get_and_set
 * @brief Gets the current value of a counter and update its value.
 *
 * @param [in,out] c: The counter.
 * @param [in] new_value: The counter new value.
 *
 * @return On success returns the counter value before updating or -1 otherwise.
 */
long long cl_counter_get_and_set(cl_counter_t *c, long long new_value);

/**
 * @name cl_counter_set
 * @brief Updates the counter value to a specified value.
 *
 * @param [in,out] c: The counter.
 * @param [in] new_value: The counter new value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_counter_set(cl_counter_t *c, long long new_value);

#endif

