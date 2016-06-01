
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
enum cweekday {
    CL_SUNDAY,
    CL_MONDAY,
    CL_TUESDAY,
    CL_WEDNESDAY,
    CL_THURSDAY,
    CL_FRIDAY,
    CL_SATURDAY
};

/** Months */
enum cmonth {
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
 * @name cdt_localtime
 * @brief Gets the current date and time in localtime format.
 *
 * @return On success returns a cdatetime_t object with the localtime or NULL
 *         otherwise.
 */
cdatetime_t *cdt_localtime(void);

/**
 * @name cdt_gmtime
 * @brief Gets the current date and time in GMT format.
 *
 * @return On success returns a cdatetime_t object with the GMT or NULL
 *         otherwise.
 */
cdatetime_t *cdt_gmtime(void);

/**
 * @name cdt_destroy
 * @brief Releases a cdatetime_t object from memory.
 *
 * @param [in,out] dt: The cdatetime_t object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cdt_destroy(cdatetime_t *dt);

/**
 * @name cdt_day
 * @brief Gets the current day from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the day value or -1 otherwise.
 */
int cdt_day(const cdatetime_t *dt);

/**
 * @name cdt_month
 * @brief Gets the current month from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the month value or -1 otherwise.
 */
int cdt_month(const cdatetime_t *dt);

/**
 * @name cdt_year
 * @brief Gets the current year from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the year value or -1 otherwise.
 */
int cdt_year(const cdatetime_t *dt);

/**
 * @name cdt_hour
 * @brief Gets the current hour from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the hour value or -1 otherwise.
 */
int cdt_hour(const cdatetime_t *dt);

/**
 * @name cdt_minute
 * @brief Gets the current minute from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the minute value or -1 otherwise.
 */
int cdt_minute(const cdatetime_t *dt);

/**
 * @name cdt_second
 * @brief Gets the current second from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns the second value or -1 otherwise.
 */
int cdt_second(const cdatetime_t *dt);

/**
 * @name cdt_isdst
 * @brief Gets the current DST information from whithin a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return Returns true or false if DST is set or not inside \a dt.
 */
bool cdt_isdst(const cdatetime_t *dt);

/**
 * @name cdt_leap_year
 * @brief Gets info if a cdatetime_t object corresponds to a leap year.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return Returns true or false is \a dt corresponds to a leap year.
 */
bool cdt_leap_year(const cdatetime_t *dt);

/**
 * @name cdt_weekday
 * @brief Gets the current weekday information from within a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return Returns the current weekday information.
 */
enum cweekday cdt_weekday(const cdatetime_t *dt);

/**
 * @name cdt_get_seconds
 * @brief Gets date and time information in seconds format.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns date and time converted to seconds or 0 otherwise.
 */
unsigned int cdt_get_seconds(const cdatetime_t *dt);

/**
 * @name cdt_get_mseconds
 * @brief Gets date and time information in milliseconds format.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns date and time converted to milliseconds or 0
 *         otherwise.
 */
unsigned long long cdt_get_mseconds(const cdatetime_t *dt);

/**
 * @name cdt_get_useconds
 * @brief Gets date and time information in microseconds format.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns date and time converted to microseconds or 0
 *         otherwise.
 */
unsigned long long cdt_get_useconds(const cdatetime_t *dt);

/**
 * @name cdt_month_of_year
 * @brief Gets the string of a month inside a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 * @param [in] full: Boolean flag to indicate if the month description will be
 *                   abbreviated or not.
 *
 * @return On success returns a cstring_t object with the month description or
 *         NULL otherwise.
 */
cstring_t *cdt_month_of_year(const cdatetime_t *dt, bool full);

/**
 * @name cdt_day_of_week
 * @brief Gets the string of a weekday inside a cdatetime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 * @param [in] full: Boolean flag to indicate if the weekday description will
 *                   be abbreviated or not.
 *
 * @return On success returns a cstring_t object with the weekday description
 *         or NULL otherwise.
 */
cstring_t *cdt_day_of_week(const cdatetime_t *dt, bool full);

/**
 * @name cdt_to_cstring
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
 * @param [in] dt: The cdatetime_t object.
 * @param [in] fmt: The output format.
 *
 * @return On success returns a cstring_t object containing date and time with
 *         the requested format.
 */
cstring_t *cdt_to_cstring(const cdatetime_t *dt, const char *fmt);

/**
 * @name cdt_dup
 * @brief Duplicates a cdatime_t object.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns a copy of \a dt or NULL otherwise.
 */
cdatetime_t *cdt_dup(const cdatetime_t *dt);

/**
 * @name cdt_to_localtime
 * @brief Converts a cdatetime_t to localtime format.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns a new cdatetime_t object in localtime or NULL
 *         otherwise.
 */
cdatetime_t *cdt_to_localtime(const cdatetime_t *dt);

/**
 * @name cdt_to_gmtime
 * @brief Converts a cdatetime_t to GMT format.
 *
 * @param [in] dt: The cdatetime_t object.
 *
 * @return On success returns a new cdatetime_t object in GMT or NULL
 *         otherwise.
 */
cdatetime_t *cdt_to_gmtime(const cdatetime_t *dt);

/**
 * @name cdt_cmp
 * @brief Compares two cdatetime_t objects.
 *
 * @param [in] t1: The first cdatetime_t object.
 * @param [in] t2: The second cdatetime_t object.
 *
 * @return Returns a integer less than, equal to, or greater than zero if \a t1
 *         is found to be less than, to match, or be greater than \a t2.
 */
int cdt_cmp(const cdatetime_t *t1, const cdatetime_t *t2);

/**
 * @name cdt_isafter
 * @brief Checks if a cdatetime_t object \a dt is after the specified
 *        cdatetime_t object \a other.
 *
 * @param [in] dt: The cdatetime_t object.
 * @param [in] other: The other cdatetime_t object.
 *
 * @return Returns true or false if \a dt is after or not of \a other.
 */
bool cdt_isafter(const cdatetime_t *dt, const cdatetime_t *other);

/**
 * @name cdt_isbefore
 * @brief Checks if a cdatetime_t object \a dt is before the specified
 *        cdatetime_t object \a other.
 *
 * @param [in] dt: The cdatetime_t object.
 * @param [in] other: The other cdatetime_t object.
 *
 * @return Returns true or false if \a dt is before or not of \a other.
 */
bool cdt_isbefore(const cdatetime_t *dt, const cdatetime_t *other);

/**
 * @name cdt_isequal
 * @brief Checks if a cdatetime_t object \a dt is equal the specified
 *        cdatetime_t object \a other.
 *
 * @param [in] dt: The cdatetime_t object.
 * @param [in] other: The other cdatetime_t object.
 *
 * @return Returns true or false if \a dt is equal or not of \a other.
 */
bool cdt_isequal(const cdatetime_t *dt, const cdatetime_t *other);

/**
 * @name cdt_mktime
 * @brief Creates a cdatetime_t object with a specific date and time.
 *
 * @param [in] year: The year.
 * @param [in] month: The month.
 * @param [in] day: The day.
 * @param [in] hour: The hour.
 * @param [in] minute: The minutes.
 * @param [in] second: The seconds.
 *
 * @return On success a new cdatetime_t object will be returned or NULL
 *         otherwise.
 */
cdatetime_t *cdt_mktime(unsigned int year, unsigned int month, unsigned int day,
                        unsigned int hour, unsigned int minute,
                        unsigned int second);

/**
 * @name cdt_mktime_from_cstring
 * @brief Creates a cdatetime_t object with a specific date and time.
 *
 * In this case the date and time information comes from a cstring_t object,
 * with the following supported formats:
 *
 * YYYY-MM-DD HH:MM:SS
 * YYYY/MM/DD HH:MM:SS
 * DD-MM-YYYY HH:MM:SS
 * DD/MM/YYYY HH:MM:SS
 *
 * @param [in] datetime: The cstring_t object.
 *
 * @return On success a new cdatetime_t object will be returned or NULL
 *         otherwise.
 */
cdatetime_t *cdt_mktime_from_cstring(const cstring_t *datetime);

/**
 * @name cdt_minus_years
 * @brief Subtracts a specific number of years from a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] years: The years to subtract.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of years subtracted or NULL otherwise.
 */
cdatetime_t *cdt_minus_years(const cdatetime_t *dt, unsigned int years);

/**
 * @name cdt_minus_days
 * @brief Subtracts a specific number of days from a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] days: The days to subtract.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of days subtracted or NULL otherwise.
 */
cdatetime_t *cdt_minus_days(const cdatetime_t *dt, unsigned int days);

/**
 * @name cdt_minus_seconds
 * @brief Subtracts a specific number of secondss from a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] seconds: The seconds to subtract.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of secondss subtracted or NULL otherwise.
 */
cdatetime_t *cdt_minus_seconds(const cdatetime_t *dt, unsigned int seconds);

/**
 * @name cdt_minus_minutes
 * @brief Subtracts a specific number of minutes from a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] minutes: The minutes to subtract.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of minutes subtracted or NULL otherwise.
 */
cdatetime_t *cdt_minus_minutes(const cdatetime_t *dt, unsigned int minutes);

/**
 * @name cdt_minus_hours
 * @brief Subtracts a specific number of hours from a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] hours: The hours to subtract.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of hours subtracted or NULL otherwise.
 */
cdatetime_t *cdt_minus_hours(const cdatetime_t *dt, unsigned int hours);

/**
 * @name cdt_plus_years
 * @brief Adds a specific number of years to a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] years: The years to add.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of years added or NULL otherwise.
 */
cdatetime_t *cdt_plus_years(const cdatetime_t *dt, unsigned int years);

/**
 * @name cdt_plus_days
 * @brief Adds a specific number of days to a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] days: The days to add.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of days added or NULL otherwise.
 */
cdatetime_t *cdt_plus_days(const cdatetime_t *dt, unsigned int days);

/**
 * @name cdt_plus_seconds
 * @brief Adds a specific number of seconds to a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] seconds: The seconds to add.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of seconds added or NULL otherwise.
 */
cdatetime_t *cdt_plus_seconds(const cdatetime_t *dt, unsigned int seconds);

/**
 * @name cdt_plus_minutes
 * @brief Adds a specific number of minutes to a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] minutes : The minutes to add.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of minutes added or NULL otherwise.
 */
cdatetime_t *cdt_plus_minutes(const cdatetime_t *dt, unsigned int minutes);

/**
 * @name cdt_plus_hours
 * @brief Adds a specific number of hours to a cdatetime_t object.
 *
 * @param [in] dt: The original cdatetime_t object.
 * @param [in] hours: The hours to add.
 *
 * @return On success returns a new cdatetime_t object with the specified number
 *         of hours added or NULL otherwise.
 */
cdatetime_t *cdt_plus_hours(const cdatetime_t *dt, unsigned int hours);

/**
 * @name cdt_current_weekday
 * @brief Gets current weekday information.
 *
 * @return Returns the current weekday information.
 */
enum cweekday cdt_current_weekday(void);

/**
 * @name cdt_current_month
 * @brief Gets current month information.
 *
 * @return Returns the current month information.
 */
enum cmonth cdt_current_month(void);

/**
 * @name cdt_is_local_dst
 * @brief Returns true or false if DST is active or not.
 *
 * @return Returns true or false if DST is active or not.
 */
bool cdt_is_local_dst(void);

/**
 * @name cdt_is_leap_year
 * @brief Returns true or false if the current year is a leap year or not.
 *
 * @return Returns true or false if the current year is a leap year or
 *         not.
 */
bool cdt_is_leap_year(void);

#endif

