
/*
 * Description: API to handle a date/time object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
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

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>

#include "collections.h"

#define YEAR_IN_SECS            31536000
#define LEAP_YEAR_IN_SECS       31622400
#define DAY_IN_SECS             86400
#define HOUR_IN_SECS            3600

#define cl_datetime_members                     \
    cl_struct_member(unsigned int, day)         \
    cl_struct_member(unsigned int, month)       \
    cl_struct_member(unsigned int, year)        \
    cl_struct_member(unsigned int, hour)        \
    cl_struct_member(unsigned int, minute)      \
    cl_struct_member(unsigned int, second)      \
    cl_struct_member(enum cl_weekday, weekday)  \
    cl_struct_member(bool, isdst)               \
    cl_struct_member(struct timeval, tv)        \
    cl_struct_member(cl_string_t *, tzone)

cl_struct_declare(cl_datetime_s, cl_datetime_members);

#define cl_datetime_s         cl_struct(cl_datetime_s)

static const char *__dow_abbrv[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *__dow_full[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static const char *__moy_abbrv[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"
};

static const char *__moy_full[] = {
    "January", "February", "March", "April", "May", "June", "July", "August",
    "September", "October", "November", "December"
};

static bool is_leap_year(unsigned int year);

static cl_datetime_s *new_cdatetime_s(void)
{
    cl_datetime_s *d = NULL;

    d = calloc(1, sizeof(cl_datetime_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    d->tzone = NULL;
    set_typeof(CL_OBJ_DATETIME, d);

    return d;
}

static void destroy_cdatetime_s(cl_datetime_s *dt)
{
    if (NULL == dt)
        return;

    if (dt->tzone != NULL)
        cl_string_destroy(dt->tzone);

    free(dt);
}

static bool is_GMT(cl_datetime_s *dt)
{
    cl_string_t *s;
    bool ret = false;

    s = cl_string_create("GMT");

    if (cl_string_cmp(dt->tzone, s) == 0)
        ret = true;

    cl_string_destroy(s);

    return ret;
}

static void cvt_time(cl_datetime_s *dt, bool UTC)
{
    struct tm tm;

    if (UTC == true)
        gmtime_r(&dt->tv.tv_sec, &tm);
    else
        localtime_r(&dt->tv.tv_sec, &tm);

    dt->day = tm.tm_mday;
    dt->month = tm.tm_mon;
    dt->year = tm.tm_year + 1900;

    dt->hour = tm.tm_hour;
    dt->minute = tm.tm_min;
    dt->second = tm.tm_sec;

    dt->isdst = (tm.tm_isdst == 0) ? false : true;
    dt->weekday = tm.tm_wday;

    if (dt->tzone != NULL)
        cl_string_destroy(dt->tzone);

    dt->tzone = cl_string_create("%s", tm.tm_zone);
}

__PUB_API__ int cl_dt_destroy(cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);
    destroy_cdatetime_s(t);

    return 0;
}

__PUB_API__ cl_datetime_t *cl_dt_localtime(void)
{
    cl_datetime_s *dt = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    dt = new_cdatetime_s();

    if (NULL == dt)
        return NULL;

    if (gettimeofday(&dt->tv, NULL) < 0) {
        return NULL;
    }

    cvt_time(dt, false);

    return dt;
}

__PUB_API__ int cl_dt_day(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->day;
}

__PUB_API__ int cl_dt_month(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->month;
}

__PUB_API__ int cl_dt_year(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->year;
}

__PUB_API__ int cl_dt_hour(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->hour;
}

__PUB_API__ int cl_dt_minute(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->minute;
}

__PUB_API__ int cl_dt_second(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->second;
}

__PUB_API__ bool cl_dt_isdst(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, false);

    return t->isdst;
}

__PUB_API__ bool cl_dt_leap_year(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, false);

    return is_leap_year(t->year);
}

__PUB_API__ enum cl_weekday cl_dt_weekday(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, -1);

    return t->weekday;
}

__PUB_API__ unsigned int cl_dt_get_seconds(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, 0);

    return t->tv.tv_sec;
}

__PUB_API__ unsigned long long cl_dt_get_mseconds(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, 0);

    return ((unsigned long long)t->tv.tv_sec * 1000) + (t->tv.tv_usec / 1000);
}

__PUB_API__ unsigned long long cl_dt_get_useconds(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, 0);

    return ((unsigned long long)t->tv.tv_sec * 1000000) + t->tv.tv_usec;
}

__PUB_API__ cl_string_t *cl_dt_month_of_year(const cl_datetime_t *dt, bool full)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;
    int moy;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    moy = t->month;

    return cl_string_create("%s", (full == true) ? __moy_full[moy]
                                                 : __moy_abbrv[moy]);
}

__PUB_API__ cl_string_t *cl_dt_day_of_week(const cl_datetime_t *dt, bool full)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;
    int dow;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    dow = t->weekday;

    return cl_string_create("%s", (full == true) ? __dow_full[dow]
                                                 : __dow_abbrv[dow]);
}

/*
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
 */
__PUB_API__ cl_string_t *cl_dt_to_cstring(const cl_datetime_t *dt, const char *fmt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt;
    cl_string_t *d = NULL;
    int i;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    d = cl_string_create_empty(0);

    do {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case 'a':
                    cl_string_cat(d, "%s", __dow_abbrv[t->weekday]);
                    break;

                case 'A':
                    cl_string_cat(d, "%s", __dow_full[t->weekday]);
                    break;

                case 'b':
                    cl_string_cat(d, "%s", __moy_abbrv[t->month]);
                    break;

                case 'B':
                    cl_string_cat(d, "%s", __moy_full[t->month]);
                    break;

                case 'd':
                    cl_string_cat(d, "%02d", t->day);
                    break;

                case 'm':
                    cl_string_cat(d, "%02d", t->month + 1);
                    break;

                case 'y':
                    cl_string_cat(d, "%02d", t->year % 100);
                    break;

                case 'Y':
                    cl_string_cat(d, "%d", t->year);
                    break;

                case 'D':
                    cl_string_cat(d, "%02d/%02d/%02d", t->day, t->month + 1,
                                  t->year % 100);

                    break;

                case 'F':
                    cl_string_cat(d, "%d-%02d-%02d", t->year, t->month + 1,
                                  t->day);

                    break;

                case 'H':
                    cl_string_cat(d, "%02d", t->hour);
                    break;

                case 'I':
                    if (t->hour > 12)
                        i = t->hour / 12;
                    else
                        i = t->hour;

                    cl_string_cat(d, "%02d", i);
                    break;

                case 'M':
                    cl_string_cat(d, "%02d", t->minute);
                    break;

                case 'S':
                    cl_string_cat(d, "%02d", t->second);
                    break;

                case 'p':
                case 'P':
                    if ((t->hour >= 12) && (t->hour <= 23))
                        cl_string_cat(d, (*fmt == 'p') ? "PM" : "pm");
                    else
                        cl_string_cat(d, (*fmt == 'p') ? "AM" : "am");

                    break;

                case 'T':
                    cl_string_cat(d, "%02d:%02d:%02d", t->hour, t->minute,
                                  t->second);

                    break;

                case 'r':
                    if (t->hour > 12)
                        i = t->hour / 12;
                    else
                        i = t->hour;

                    cl_string_cat(d, "%02d:%02d:%02d ", i, t->minute, t->second);

                    if ((t->hour >= 12) && (t->hour <= 23))
                        cl_string_cat(d, "PM");
                    else
                        cl_string_cat(d, "AM");

                    break;

                case 'u':
                    cl_string_cat(d, "%d", (t->weekday == CL_SUNDAY)
                                                ? 7
                                                : t->weekday);

                    break;

                case 'w':
                    cl_string_cat(d, "%d", t->weekday);
                    break;

                case 'Z':
                    cl_string_cat(d, "%s", cl_string_valueof(t->tzone));
                    break;

                case '1':
                    cl_string_cat(d, "%03ld", t->tv.tv_usec / 1000);
                    break;
            }
        } else
            if (isprint(*fmt))
                cl_string_cat(d, "%c", *fmt);
    } while (*fmt++);

    return d;
}

/*
 * All comparisons are made using seconds precision.
 */

__PUB_API__ int cl_dt_cmp(const cl_datetime_t *t1, const cl_datetime_t *t2)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(t1, CL_OBJ_DATETIME) == false) ||
        (validate_object(t2, CL_OBJ_DATETIME) == false))
    {
        return -1;
    }

    s1 = cl_dt_get_seconds(t1);
    s2 = cl_dt_get_seconds(t2);

    if (s1 > s2)
        return 1;
    else if (s1 < s2)
        return -1;

    return 0;
}

__PUB_API__ bool cl_dt_isafter(const cl_datetime_t *dt, const cl_datetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CL_OBJ_DATETIME) == false) ||
        (validate_object(other, CL_OBJ_DATETIME) == false))
    {
        return false;
    }

    s1 = cl_dt_get_seconds(dt);
    s2 = cl_dt_get_seconds(other);

    if (s1 > s2)
        return true;

    return false;
}

__PUB_API__ bool cl_dt_isbefore(const cl_datetime_t *dt, const cl_datetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CL_OBJ_DATETIME) == false) ||
        (validate_object(other, CL_OBJ_DATETIME) == false))
    {
        return false;
    }

    s1 = cl_dt_get_seconds(dt);
    s2 = cl_dt_get_seconds(other);

    if (s1 < s2)
        return true;

    return false;
}

__PUB_API__ bool cl_dt_isequal(const cl_datetime_t *dt, const cl_datetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CL_OBJ_DATETIME) == false) ||
        (validate_object(other, CL_OBJ_DATETIME) == false))
    {
        return false;
    }

    s1 = cl_dt_get_seconds(dt);
    s2 = cl_dt_get_seconds(other);

    if (s1 == s2)
        return true;

    return false;
}

__PUB_API__ cl_datetime_t *cl_dt_dup(const cl_datetime_t *dt)
{
    cl_datetime_s *t = NULL, *tt = (cl_datetime_s *)dt;
    bool UTC = false;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    t = new_cdatetime_s();

    if (NULL == t)
        return NULL;

    if (is_GMT(tt) == true)
        UTC = true;

    /* dup internal data */
    t->tv.tv_sec = tt->tv.tv_sec;
    t->tv.tv_usec = tt->tv.tv_usec;

    cvt_time(t, UTC);

    return t;
}

__PUB_API__ cl_datetime_t *cl_dt_gmtime(void)
{
    cl_datetime_s *dt = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    dt = new_cdatetime_s();

    if (NULL == dt)
        return NULL;

    if (gettimeofday(&dt->tv, NULL) < 0) {
        return NULL;
    }

    cvt_time(dt, true);

    return dt;
}

__PUB_API__ cl_datetime_t *cl_dt_to_localtime(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt, *ddt = NULL;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(t);

    /* Already localtime, returns a copy */
    if (is_GMT(t) == false)
        return ddt;

    /* Convert to localtime */
    cvt_time(ddt, false);

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_to_gmtime(const cl_datetime_t *dt)
{
    cl_datetime_s *t = (cl_datetime_s *)dt, *ddt = NULL;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(t);

    /* Already GMT, returns a copy */
    if (is_GMT(t) == true)
        return ddt;

    /* Convert to GMT */
    cvt_time(ddt, true);

    return ddt;
}

static bool is_month(unsigned int month)
{
    if (month >= 12)
        return false;

    return true;
}

static bool is_day(unsigned int day)
{
    if ((day == 0) || (day > 31))
        return false;

    return true;
}

static bool is_hour(unsigned int hour)
{
    if (hour > 23)
        return false;

    return true;
}

static bool is_minute_or_second(unsigned int value)
{
    if (value > 59)
        return false;

    return true;
}

__PUB_API__ cl_datetime_t *cl_dt_mktime(unsigned int year, unsigned int month,
    unsigned int day, unsigned int hour, unsigned int minute,
    unsigned int second)
{
    cl_datetime_s *dt = NULL;
    struct tm tm;

    __clib_function_init__(false, NULL, -1, NULL);

    if ((is_month(month) == false) ||
        (is_day(day) == false) ||
        (is_hour(hour) == false) ||
        (is_minute_or_second(minute) == false) ||
        (is_minute_or_second(second) == false))
    {
        return NULL;
    }

    tm.tm_year = year - 1900;
    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;

    dt = new_cdatetime_s();

    if (NULL == dt)
        return NULL;

    dt->tv.tv_sec = mktime(&tm);
    cvt_time(dt, false);

    return dt;
}

__PUB_API__ cl_datetime_t *cl_dt_mktime_from_cstring(const cl_string_t *datetime)
{
    cl_string_t *p, *s;
    cl_string_list_t *l, *ld, *lt;
    int day, month, year, hour, min, sec;

    __clib_function_init__(true, datetime, CL_OBJ_STRING, NULL);
    s = cl_string_dup(datetime);
    cl_string_alltrim(s);

    l = cl_string_split(s, " ");
    ld = cl_string_split(cl_string_list_get(l, 0), "/-");
    lt = cl_string_split(cl_string_list_get(l, 1), ":");

    hour = cl_string_to_int(cl_string_list_get(lt, 0));
    min = cl_string_to_int(cl_string_list_get(lt, 1));
    sec = cl_string_to_int(cl_string_list_get(lt, 2));

    p = cl_string_list_get(ld, 0);
    month = cl_string_to_int(cl_string_list_get(ld, 1));

    if (cl_string_length(p) == 2) {
        day = cl_string_to_int(p);
        year = cl_string_to_int(cl_string_list_get(ld, 2));
    } else {
        year = cl_string_to_int(p);
        day = cl_string_to_int(cl_string_list_get(ld, 2));
    }

    cl_string_list_destroy(lt);
    cl_string_list_destroy(ld);
    cl_string_list_destroy(l);
    cl_string_unref(s);

    return cl_dt_mktime(year, month, day, hour, min, sec);
}

static bool is_leap_year(unsigned int year)
{
    if ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0))
        return true;

    return false;
}

static int get_number_of_leap_years(unsigned int year1, unsigned int year2)
{
    unsigned int i;
    int n=0;

    for (i = (year1 + 1); i < year2; i++)
        if (is_leap_year(i) == true)
            n++;

    return n;
}

__PUB_API__ cl_datetime_t *cl_dt_minus_years(const cl_datetime_t *dt,
    unsigned int years)
{
    cl_datetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    leap_years = get_number_of_leap_years(ddt->year - years, ddt->year);
    n = (leap_years * LEAP_YEAR_IN_SECS) + ((years - leap_years) * YEAR_IN_SECS);

    ddt->tv.tv_sec -= n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_minus_days(const cl_datetime_t *dt,
    unsigned int days)
{
    cl_datetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    n = days * DAY_IN_SECS;
    ddt->tv.tv_sec -= n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_minus_seconds(const cl_datetime_t *dt,
    unsigned int seconds)
{
    cl_datetime_s *ddt = NULL;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec -= seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_minus_minutes(const cl_datetime_t *dt,
    unsigned int minutes)
{
    return cl_dt_minus_seconds(dt, minutes * 60);
}

__PUB_API__ cl_datetime_t *cl_dt_minus_hours(const cl_datetime_t *dt,
    unsigned int hours)
{
    return cl_dt_minus_seconds(dt, hours * 3600);
}

__PUB_API__ cl_datetime_t *cl_dt_plus_days(const cl_datetime_t *dt,
    unsigned int days)
{
    cl_datetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    n = days * DAY_IN_SECS;
    ddt->tv.tv_sec += n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_plus_years(const cl_datetime_t *dt,
    unsigned int years)
{
    cl_datetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    leap_years = get_number_of_leap_years(ddt->year, ddt->year + years);
    n = (leap_years * LEAP_YEAR_IN_SECS) + ((years - leap_years) * YEAR_IN_SECS);

    ddt->tv.tv_sec += n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_plus_seconds(const cl_datetime_t *dt,
    unsigned int seconds)
{
    cl_datetime_s *ddt = NULL;

    __clib_function_init__(true, dt, CL_OBJ_DATETIME, NULL);
    ddt = cl_dt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec += seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

__PUB_API__ cl_datetime_t *cl_dt_plus_minutes(const cl_datetime_t *dt,
    unsigned int minutes)
{
    return cl_dt_plus_seconds(dt, minutes * 60);
}

__PUB_API__ cl_datetime_t *cl_dt_plus_hours(const cl_datetime_t *dt,
    unsigned int hours)
{
    return cl_dt_plus_seconds(dt, hours * 3600);
}

__PUB_API__ enum cl_weekday cl_dt_current_weekday(void)
{
    cl_datetime_t *dt = NULL;
    enum cl_weekday w;

    __clib_function_init__(false, NULL, -1, -1);
    dt = cl_dt_localtime();

    if (NULL == dt)
        return -1;

    w = cl_dt_weekday(dt);
    cl_dt_destroy(dt);

    return w;
}

__PUB_API__ enum cl_month cl_dt_current_month(void)
{
    cl_datetime_t *dt = NULL;
    enum cl_month m;

    __clib_function_init__(false, NULL, -1, -1);
    dt = cl_dt_localtime();

    if (NULL == dt)
        return -1;

    m = cl_dt_month(dt);
    cl_dt_destroy(dt);

    return m;
}

__PUB_API__ bool cl_dt_is_local_dst(void)
{
    time_t t;
    struct tm tm;

    __clib_function_init__(false, NULL, -1, false);
    time(&t);
    localtime_r(&t, &tm);

    if (tm.tm_isdst)
        return true;

    return false;
}

__PUB_API__ bool cl_dt_is_leap_year(void)
{
    time_t t;
    struct tm tm;

    __clib_function_init__(false, NULL, -1, false);
    time(&t);
    localtime_r(&t, &tm);

    return is_leap_year(tm.tm_year + 1900);
}

