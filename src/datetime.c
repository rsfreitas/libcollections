
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

#define cdatetime_members                       \
    cl_struct_member(unsigned int, day)         \
    cl_struct_member(unsigned int, month)       \
    cl_struct_member(unsigned int, year)        \
    cl_struct_member(unsigned int, hour)        \
    cl_struct_member(unsigned int, minute)      \
    cl_struct_member(unsigned int, second)      \
    cl_struct_member(enum cl_weekday, weekday)    \
    cl_struct_member(bool, isdst)               \
    cl_struct_member(struct timeval, tv)        \
    cl_struct_member(cstring_t *, tzone)

cl_struct_declare(cdatetime_s, cdatetime_members);

#define cdatetime_s         cl_struct(cdatetime_s)

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

static cdatetime_s *new_cdatetime_s(void)
{
    cdatetime_s *d = NULL;

    d = calloc(1, sizeof(cdatetime_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    d->tzone = NULL;
    set_typeof(CDATETIME, d);

    return d;
}

static void destroy_cdatetime_s(cdatetime_s *dt)
{
    if (NULL == dt)
        return;

    if (dt->tzone != NULL)
        cstring_destroy(dt->tzone);

    free(dt);
}

static bool is_GMT(cdatetime_s *dt)
{
    cstring_t *s;
    bool ret = false;

    s = cstring_create("GMT");

    if (cstring_cmp(dt->tzone, s) == 0)
        ret = true;

    cstring_destroy(s);

    return ret;
}

static void cvt_time(cdatetime_s *dt, bool UTC)
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
        cstring_destroy(dt->tzone);

    dt->tzone = cstring_create("%s", tm.tm_zone);
}

__PUB_API__ int cdt_destroy(cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);
    destroy_cdatetime_s(t);

    return 0;
}

__PUB_API__ cdatetime_t *cdt_localtime(void)
{
    cdatetime_s *dt = NULL;

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

__PUB_API__ int cdt_day(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->day;
}

__PUB_API__ int cdt_month(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->month;
}

__PUB_API__ int cdt_year(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->year;
}

__PUB_API__ int cdt_hour(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->hour;
}

__PUB_API__ int cdt_minute(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->minute;
}

__PUB_API__ int cdt_second(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->second;
}

__PUB_API__ bool cdt_isdst(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, false);

    return t->isdst;
}

__PUB_API__ bool cdt_leap_year(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, false);

    return is_leap_year(t->year);
}

__PUB_API__ enum cl_weekday cdt_weekday(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, -1);

    return t->weekday;
}

__PUB_API__ unsigned int cdt_get_seconds(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, 0);

    return t->tv.tv_sec;
}

__PUB_API__ unsigned long long cdt_get_mseconds(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, 0);

    return ((unsigned long long)t->tv.tv_sec * 1000) + (t->tv.tv_usec / 1000);
}

__PUB_API__ unsigned long long cdt_get_useconds(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt;

    __clib_function_init__(true, dt, CDATETIME, 0);

    return ((unsigned long long)t->tv.tv_sec * 1000000) + t->tv.tv_usec;
}

__PUB_API__ cstring_t *cdt_month_of_year(const cdatetime_t *dt, bool full)
{
    cdatetime_s *t = (cdatetime_s *)dt;
    int moy;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    moy = t->month;

    return cstring_create("%s", (full == true) ? __moy_full[moy]
                                               : __moy_abbrv[moy]);
}

__PUB_API__ cstring_t *cdt_day_of_week(const cdatetime_t *dt, bool full)
{
    cdatetime_s *t = (cdatetime_s *)dt;
    int dow;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    dow = t->weekday;

    return cstring_create("%s", (full == true) ? __dow_full[dow]
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
__PUB_API__ cstring_t *cdt_to_cstring(const cdatetime_t *dt, const char *fmt)
{
    cdatetime_s *t = (cdatetime_s *)dt;
    cstring_t *d = NULL;
    int i;

    __clib_function_init__(true, dt, CDATETIME, NULL);

    if (NULL == fmt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    d = cstring_create_empty(0);

    do {
        if (*fmt == '%') {
            fmt++;

            switch (*fmt) {
                case 'a':
                    cstring_cat(d, "%s", __dow_abbrv[t->weekday]);
                    break;

                case 'A':
                    cstring_cat(d, "%s", __dow_full[t->weekday]);
                    break;

                case 'b':
                    cstring_cat(d, "%s", __moy_abbrv[t->month]);
                    break;

                case 'B':
                    cstring_cat(d, "%s", __moy_full[t->month]);
                    break;

                case 'd':
                    cstring_cat(d, "%02d", t->day);
                    break;

                case 'm':
                    cstring_cat(d, "%02d", t->month + 1);
                    break;

                case 'y':
                    cstring_cat(d, "%02d", t->year % 100);
                    break;

                case 'Y':
                    cstring_cat(d, "%d", t->year);
                    break;

                case 'D':
                    cstring_cat(d, "%02d/%02d/%02d", t->day, t->month + 1,
                                t->year % 100);

                    break;

                case 'F':
                    cstring_cat(d, "%d-%02d-%02d", t->year, t->month + 1,
                                t->day);

                    break;

                case 'H':
                    cstring_cat(d, "%02d", t->hour);
                    break;

                case 'I':
                    if (t->hour > 12)
                        i = t->hour / 12;
                    else
                        i = t->hour;

                    cstring_cat(d, "%02d", i);
                    break;

                case 'M':
                    cstring_cat(d, "%02d", t->minute);
                    break;

                case 'S':
                    cstring_cat(d, "%02d", t->second);
                    break;

                case 'p':
                case 'P':
                    if ((t->hour >= 12) && (t->hour <= 23))
                        cstring_cat(d, (*fmt == 'p') ? "PM" : "pm");
                    else
                        cstring_cat(d, (*fmt == 'p') ? "AM" : "am");

                    break;

                case 'T':
                    cstring_cat(d, "%02d:%02d:%02d", t->hour, t->minute,
                                t->second);

                    break;

                case 'r':
                    if (t->hour > 12)
                        i = t->hour / 12;
                    else
                        i = t->hour;

                    cstring_cat(d, "%02d:%02d:%02d ", i, t->minute, t->second);

                    if ((t->hour >= 12) && (t->hour <= 23))
                        cstring_cat(d, "PM");
                    else
                        cstring_cat(d, "AM");

                    break;

                case 'u':
                    cstring_cat(d, "%d", (t->weekday == CL_SUNDAY)
                                                ? 7
                                                : t->weekday);

                    break;

                case 'w':
                    cstring_cat(d, "%d", t->weekday);
                    break;

                case 'Z':
                    cstring_cat(d, "%s", cstring_valueof(t->tzone));
                    break;

                case '1':
                    cstring_cat(d, "%03ld", t->tv.tv_usec / 1000);
                    break;
            }
        } else
            if (isprint(*fmt))
                cstring_cat(d, "%c", *fmt);
    } while (*fmt++);

    return d;
}

/*
 * All comparisons are made using seconds precision.
 */

__PUB_API__ int cdt_cmp(const cdatetime_t *t1, const cdatetime_t *t2)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(t1, CDATETIME) == false) ||
        (validate_object(t2, CDATETIME) == false))
    {
        return -1;
    }

    s1 = cdt_get_seconds(t1);
    s2 = cdt_get_seconds(t2);

    if (s1 > s2)
        return 1;
    else if (s1 < s2)
        return -1;

    return 0;
}

__PUB_API__ bool cdt_isafter(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CDATETIME) == false) ||
        (validate_object(other, CDATETIME) == false))
    {
        return false;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 > s2)
        return true;

    return false;
}

__PUB_API__ bool cdt_isbefore(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CDATETIME) == false) ||
        (validate_object(other, CDATETIME) == false))
    {
        return false;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 < s2)
        return true;

    return false;
}

__PUB_API__ bool cdt_isequal(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    __clib_function_init__(false, NULL, -1, false);

    if ((validate_object(dt, CDATETIME) == false) ||
        (validate_object(other, CDATETIME) == false))
    {
        return false;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 == s2)
        return true;

    return false;
}

__PUB_API__ cdatetime_t *cdt_dup(const cdatetime_t *dt)
{
    cdatetime_s *t = NULL, *tt = (cdatetime_s *)dt;
    bool UTC = false;

    __clib_function_init__(true, dt, CDATETIME, NULL);
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

__PUB_API__ cdatetime_t *cdt_gmtime(void)
{
    cdatetime_s *dt = NULL;

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

__PUB_API__ cdatetime_t *cdt_to_localtime(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt, *ddt = NULL;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(t);

    /* Already localtime, returns a copy */
    if (is_GMT(t) == false)
        return ddt;

    /* Convert to localtime */
    cvt_time(ddt, false);

    return ddt;
}

__PUB_API__ cdatetime_t *cdt_to_gmtime(const cdatetime_t *dt)
{
    cdatetime_s *t = (cdatetime_s *)dt, *ddt = NULL;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(t);

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

__PUB_API__ cdatetime_t *cdt_mktime(unsigned int year, unsigned int month,
    unsigned int day, unsigned int hour, unsigned int minute,
    unsigned int second)
{
    cdatetime_s *dt = NULL;
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

__PUB_API__ cdatetime_t *cdt_mktime_from_cstring(const cstring_t *datetime)
{
    cstring_t *p, *s;
    cstring_list_t *l, *ld, *lt;
    int day, month, year, hour, min, sec;

    __clib_function_init__(true, datetime, CSTRING, NULL);
    s = cstring_dup(datetime);
    cstring_alltrim(s);

    l = cstring_split(s, " ");
    ld = cstring_split(cstring_list_get(l, 0), "/-");
    lt = cstring_split(cstring_list_get(l, 1), ":");

    hour = cstring_to_int(cstring_list_get(lt, 0));
    min = cstring_to_int(cstring_list_get(lt, 1));
    sec = cstring_to_int(cstring_list_get(lt, 2));

    p = cstring_list_get(ld, 0);
    month = cstring_to_int(cstring_list_get(ld, 1));

    if (cstring_length(p) == 2) {
        day = cstring_to_int(p);
        year = cstring_to_int(cstring_list_get(ld, 2));
    } else {
        year = cstring_to_int(p);
        day = cstring_to_int(cstring_list_get(ld, 2));
    }

    cstring_list_destroy(lt);
    cstring_list_destroy(ld);
    cstring_list_destroy(l);
    cstring_unref(s);

    return cdt_mktime(year, month, day, hour, min, sec);
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

__PUB_API__ cdatetime_t *cdt_minus_years(const cdatetime_t *dt, unsigned int years)
{
    cdatetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

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

__PUB_API__ cdatetime_t *cdt_minus_days(const cdatetime_t *dt, unsigned int days)
{
    cdatetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    n = days * DAY_IN_SECS;
    ddt->tv.tv_sec -= n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cdatetime_t *cdt_minus_seconds(const cdatetime_t *dt,
    unsigned int seconds)
{
    cdatetime_s *ddt = NULL;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec -= seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

__PUB_API__ cdatetime_t *cdt_minus_minutes(const cdatetime_t *dt,
    unsigned int minutes)
{
    return cdt_minus_seconds(dt, minutes * 60);
}

__PUB_API__ cdatetime_t *cdt_minus_hours(const cdatetime_t *dt, unsigned int hours)
{
    return cdt_minus_seconds(dt, hours * 3600);
}

__PUB_API__ cdatetime_t *cdt_plus_days(const cdatetime_t *dt, unsigned int days)
{
    cdatetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    if (is_GMT(ddt) == true)
        UTC = true;

    n = days * DAY_IN_SECS;
    ddt->tv.tv_sec += n;
    cvt_time(ddt, UTC);

    return ddt;
}

__PUB_API__ cdatetime_t *cdt_plus_years(const cdatetime_t *dt, unsigned int years)
{
    cdatetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

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

__PUB_API__ cdatetime_t *cdt_plus_seconds(const cdatetime_t *dt,
    unsigned int seconds)
{
    cdatetime_s *ddt = NULL;

    __clib_function_init__(true, dt, CDATETIME, NULL);
    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec += seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

__PUB_API__ cdatetime_t *cdt_plus_minutes(const cdatetime_t *dt,
    unsigned int minutes)
{
    return cdt_plus_seconds(dt, minutes * 60);
}

__PUB_API__ cdatetime_t *cdt_plus_hours(const cdatetime_t *dt, unsigned int hours)
{
    return cdt_plus_seconds(dt, hours * 3600);
}

__PUB_API__ enum cl_weekday cdt_current_weekday(void)
{
    cdatetime_t *dt = NULL;
    enum cl_weekday w;

    __clib_function_init__(false, NULL, -1, -1);
    dt = cdt_localtime();

    if (NULL == dt)
        return -1;

    w = cdt_weekday(dt);
    cdt_destroy(dt);

    return w;
}

__PUB_API__ enum cl_month cdt_current_month(void)
{
    cdatetime_t *dt = NULL;
    enum cl_month m;

    __clib_function_init__(false, NULL, -1, -1);
    dt = cdt_localtime();

    if (NULL == dt)
        return -1;

    m = cdt_month(dt);
    cdt_destroy(dt);

    return m;
}

__PUB_API__ bool cdt_is_local_dst(void)
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

__PUB_API__ bool cdt_is_leap_year(void)
{
    time_t t;
    struct tm tm;

    __clib_function_init__(false, NULL, -1, false);
    time(&t);
    localtime_r(&t, &tm);

    return is_leap_year(tm.tm_year + 1900);
}

