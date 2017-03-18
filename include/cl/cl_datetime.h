
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:19 2015
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

#ifndef _COLLECTIONS_CL_DATETIME_H
#define _COLLECTIONS_CL_DATETIME_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_datetime.h> directly; include <collections.h> instead."
# endif
#endif

/** Weekdays */
enum cl_weekday {
    CL_SUNDAY,
    CL_MONDAY,
    CL_TUESDAY,
    CL_WEDNESDAY,
    CL_THURSDAY,
    CL_FRIDAY,
    CL_SATURDAY
};

/** Months */
enum cl_month {
    CL_JANUARY,
    CL_FEBRUARY,
    CL_MARCH,
    CL_APRIL,
    CL_MAY,
    CL_JUNE,
    CL_JULY,
    CL_AUGUST,
    CL_SEPTEMBER,
    CL_OCTOBER,
    CL_NOVEMBER,
    CL_DECEMBR
};

/**
 * @name cl_dt_localtime
 * @brief Gets the current date and time in localtime format.
 *
 * @return On success returns a cl_datetime_t object with the localtime or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_localtime(void);

/**
 * @name cl_dt_gmtime
 * @brief Gets the current date and time in GMT format.
 *
 * @return On success returns a cl_datetime_t object with the GMT or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_gmtime(void);

/**
 * @name cl_dt_destroy
 * @brief Releases a cl_datetime_t object from memory.
 *
 * @param [in,out] dt: The cl_datetime_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_dt_destroy(cl_datetime_t *dt);

/**
 * @name cl_dt_day
 * @brief Gets the current day from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the day value or -1 otherwise.
 */
int cl_dt_day(const cl_datetime_t *dt);

/**
 * @name cl_dt_month
 * @brief Gets the current month from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the month value or -1 otherwise.
 */
int cl_dt_month(const cl_datetime_t *dt);

/**
 * @name cl_dt_year
 * @brief Gets the current year from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the year value or -1 otherwise.
 */
int cl_dt_year(const cl_datetime_t *dt);

/**
 * @name cl_dt_hour
 * @brief Gets the current hour from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the hour value or -1 otherwise.
 */
int cl_dt_hour(const cl_datetime_t *dt);

/**
 * @name cl_dt_minute
 * @brief Gets the current minute from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the minute value or -1 otherwise.
 */
int cl_dt_minute(const cl_datetime_t *dt);

/**
 * @name cl_dt_second
 * @brief Gets the current second from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns the second value or -1 otherwise.
 */
int cl_dt_second(const cl_datetime_t *dt);

/**
 * @name cl_dt_isdst
 * @brief Gets the current DST information from whithin a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return Returns true or false if DST is set or not inside \a dt.
 */
bool cl_dt_isdst(const cl_datetime_t *dt);

/**
 * @name cl_dt_leap_year
 * @brief Gets info if a cl_datetime_t object corresponds to a leap year.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return Returns true or false is \a dt corresponds to a leap year.
 */
bool cl_dt_leap_year(const cl_datetime_t *dt);

/**
 * @name cl_dt_weekday
 * @brief Gets the current weekday information from within a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return Returns the current weekday information.
 */
enum cl_weekday cl_dt_weekday(const cl_datetime_t *dt);

/**
 * @name cl_dt_get_seconds
 * @brief Gets date and time information in seconds format.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns date and time converted to seconds or 0 otherwise.
 */
unsigned int cl_dt_get_seconds(const cl_datetime_t *dt);

/**
 * @name cl_dt_get_mseconds
 * @brief Gets date and time information in milliseconds format.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns date and time converted to milliseconds or 0
 *         otherwise.
 */
unsigned long long cl_dt_get_mseconds(const cl_datetime_t *dt);

/**
 * @name cl_dt_get_useconds
 * @brief Gets date and time information in microseconds format.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns date and time converted to microseconds or 0
 *         otherwise.
 */
unsigned long long cl_dt_get_useconds(const cl_datetime_t *dt);

/**
 * @name cl_dt_month_of_year
 * @brief Gets the string of a month inside a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] full: Boolean flag to indicate if the month description will be
 *                   abbreviated or not.
 *
 * @return On success returns a cl_string_t object with the month description or
 *         NULL otherwise.
 */
cl_string_t *cl_dt_month_of_year(const cl_datetime_t *dt, bool full);

/**
 * @name cl_dt_day_of_week
 * @brief Gets the string of a weekday inside a cl_datetime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] full: Boolean flag to indicate if the weekday description will
 *                   be abbreviated or not.
 *
 * @return On success returns a cl_string_t object with the weekday description
 *         or NULL otherwise.
 */
cl_string_t *cl_dt_day_of_week(const cl_datetime_t *dt, bool full);

/**
 * @name cl_dt_to_cstring
 * @brief Gets the date and time in a specific string format.
 *
 * Supported formats:
 *
 * %a - abbreviated weekday
 * %A - full weekday
 * %b - abbreviated month
 * %B - full month
 *
 * %d - day of month (1 to 31)
 * %m - month (01 to 12)
 * %y - year (00 to 99)
 * %Y - full year
 * %D - equivalent to %d/%m/%y
 * %F - ISO 8601 format, equivalent to %Y-%m-%d
 *
 * %H - hour (00 to 23)
 * %I - hour (01 to 12)
 * %M - minute (00 to 59)
 * %S - second (00 to 60)
 * %p - AM or PM
 * %P - am or pm
 * %T - equivalent to %H:%M:%S
 * %r - equivalent to %I:%M:%S %p
 * %u - decimal day of week (1 to 7) (1 = Monday)
 * %w - decimal day of week (0 to 6) (0 = Sunday)
 * %Z - Timezone name or abbreviation
 *
 * NON ANSI C:
 *
 * %1 - milliseconds
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] fmt: The output format.
 *
 * @return On success returns a cl_string_t object containing date and time with
 *         the requested format.
 */
cl_string_t *cl_dt_to_cstring(const cl_datetime_t *dt, const char *fmt);

/**
 * @name cl_dt_dup
 * @brief Duplicates a cdatime_t object.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns a copy of \a dt or NULL otherwise.
 */
cl_datetime_t *cl_dt_dup(const cl_datetime_t *dt);

/**
 * @name cl_dt_to_localtime
 * @brief Converts a cl_datetime_t to localtime format.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns a new cl_datetime_t object in localtime or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_to_localtime(const cl_datetime_t *dt);

/**
 * @name cl_dt_to_gmtime
 * @brief Converts a cl_datetime_t to GMT format.
 *
 * @param [in] dt: The cl_datetime_t object.
 *
 * @return On success returns a new cl_datetime_t object in GMT or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_to_gmtime(const cl_datetime_t *dt);

/**
 * @name cl_dt_cmp
 * @brief Compares two cl_datetime_t objects.
 *
 * @param [in] t1: The first cl_datetime_t object.
 * @param [in] t2: The second cl_datetime_t object.
 *
 * @return Returns a integer less than, equal to, or greater than zero if \a t1
 *         is found to be less than, to match, or be greater than \a t2.
 */
int cl_dt_cmp(const cl_datetime_t *t1, const cl_datetime_t *t2);

/**
 * @name cl_dt_isafter
 * @brief Checks if a cl_datetime_t object \a dt is after the specified
 *        cl_datetime_t object \a other.
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] other: The other cl_datetime_t object.
 *
 * @return Returns true or false if \a dt is after or not of \a other.
 */
bool cl_dt_isafter(const cl_datetime_t *dt, const cl_datetime_t *other);

/**
 * @name cl_dt_isbefore
 * @brief Checks if a cl_datetime_t object \a dt is before the specified
 *        cl_datetime_t object \a other.
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] other: The other cl_datetime_t object.
 *
 * @return Returns true or false if \a dt is before or not of \a other.
 */
bool cl_dt_isbefore(const cl_datetime_t *dt, const cl_datetime_t *other);

/**
 * @name cl_dt_isequal
 * @brief Checks if a cl_datetime_t object \a dt is equal the specified
 *        cl_datetime_t object \a other.
 *
 * @param [in] dt: The cl_datetime_t object.
 * @param [in] other: The other cl_datetime_t object.
 *
 * @return Returns true or false if \a dt is equal or not of \a other.
 */
bool cl_dt_isequal(const cl_datetime_t *dt, const cl_datetime_t *other);

/**
 * @name cl_dt_mktime
 * @brief Creates a cl_datetime_t object with a specific date and time.
 *
 * @param [in] year: The year.
 * @param [in] month: The month.
 * @param [in] day: The day.
 * @param [in] hour: The hour.
 * @param [in] minute: The minutes.
 * @param [in] second: The seconds.
 *
 * @return On success a new cl_datetime_t object will be returned or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_mktime(unsigned int year, unsigned int month,
                            unsigned int day, unsigned int hour,
                            unsigned int minute, unsigned int second);

/**
 * @name cl_dt_mktime_from_cstring
 * @brief Creates a cl_datetime_t object with a specific date and time.
 *
 * In this case the date and time information comes from a cl_string_t object,
 * with the following supported formats:
 *
 * YYYY-MM-DD HH:MM:SS
 * YYYY/MM/DD HH:MM:SS
 * DD-MM-YYYY HH:MM:SS
 * DD/MM/YYYY HH:MM:SS
 *
 * @param [in] datetime: The cl_string_t object.
 *
 * @return On success a new cl_datetime_t object will be returned or NULL
 *         otherwise.
 */
cl_datetime_t *cl_dt_mktime_from_cstring(const cl_string_t *datetime);

/**
 * @name cl_dt_minus_years
 * @brief Subtracts a specific number of years from a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] years: The years to subtract.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of years subtracted or NULL otherwise.
 */
cl_datetime_t *cl_dt_minus_years(const cl_datetime_t *dt, unsigned int years);

/**
 * @name cl_dt_minus_days
 * @brief Subtracts a specific number of days from a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] days: The days to subtract.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of days subtracted or NULL otherwise.
 */
cl_datetime_t *cl_dt_minus_days(const cl_datetime_t *dt, unsigned int days);

/**
 * @name cl_dt_minus_seconds
 * @brief Subtracts a specific number of secondss from a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] seconds: The seconds to subtract.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of secondss subtracted or NULL otherwise.
 */
cl_datetime_t *cl_dt_minus_seconds(const cl_datetime_t *dt, unsigned int seconds);

/**
 * @name cl_dt_minus_minutes
 * @brief Subtracts a specific number of minutes from a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] minutes: The minutes to subtract.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of minutes subtracted or NULL otherwise.
 */
cl_datetime_t *cl_dt_minus_minutes(const cl_datetime_t *dt, unsigned int minutes);

/**
 * @name cl_dt_minus_hours
 * @brief Subtracts a specific number of hours from a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] hours: The hours to subtract.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of hours subtracted or NULL otherwise.
 */
cl_datetime_t *cl_dt_minus_hours(const cl_datetime_t *dt, unsigned int hours);

/**
 * @name cl_dt_plus_years
 * @brief Adds a specific number of years to a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] years: The years to add.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of years added or NULL otherwise.
 */
cl_datetime_t *cl_dt_plus_years(const cl_datetime_t *dt, unsigned int years);

/**
 * @name cl_dt_plus_days
 * @brief Adds a specific number of days to a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] days: The days to add.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of days added or NULL otherwise.
 */
cl_datetime_t *cl_dt_plus_days(const cl_datetime_t *dt, unsigned int days);

/**
 * @name cl_dt_plus_seconds
 * @brief Adds a specific number of seconds to a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] seconds: The seconds to add.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of seconds added or NULL otherwise.
 */
cl_datetime_t *cl_dt_plus_seconds(const cl_datetime_t *dt, unsigned int seconds);

/**
 * @name cl_dt_plus_minutes
 * @brief Adds a specific number of minutes to a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] minutes : The minutes to add.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of minutes added or NULL otherwise.
 */
cl_datetime_t *cl_dt_plus_minutes(const cl_datetime_t *dt, unsigned int minutes);

/**
 * @name cl_dt_plus_hours
 * @brief Adds a specific number of hours to a cl_datetime_t object.
 *
 * @param [in] dt: The original cl_datetime_t object.
 * @param [in] hours: The hours to add.
 *
 * @return On success returns a new cl_datetime_t object with the specified number
 *         of hours added or NULL otherwise.
 */
cl_datetime_t *cl_dt_plus_hours(const cl_datetime_t *dt, unsigned int hours);

/**
 * @name cl_dt_current_weekday
 * @brief Gets current weekday information.
 *
 * @return Returns the current weekday information.
 */
enum cl_weekday cl_dt_current_weekday(void);

/**
 * @name cl_dt_current_month
 * @brief Gets current month information.
 *
 * @return Returns the current month information.
 */
enum cl_month cl_dt_current_month(void);

/**
 * @name cl_dt_is_local_dst
 * @brief Returns true or false if DST is active or not.
 *
 * @return Returns true or false if DST is active or not.
 */
bool cl_dt_is_local_dst(void);

/**
 * @name cl_dt_is_leap_year
 * @brief Returns true or false if the current year is a leap year or not.
 *
 * @return Returns true or false if the current year is a leap year or
 *         not.
 */
bool cl_dt_is_leap_year(void);

#endif

