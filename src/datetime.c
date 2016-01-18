
/*
 * Description: API to handle a date/time object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "collections.h"

#define YEAR_IN_SECS            31536000
#define LEAP_YEAR_IN_SECS       31622400
#define DAY_IN_SECS             86400
#define HOUR_IN_SECS            3600

struct cdatetime_s {
    unsigned int    day;        /* 1 to 31 */
    unsigned int    month;      /* 0 to 11 */
    unsigned int    year;
    unsigned int    hour;
    unsigned int    minute;
    unsigned int    second;
    enum cweekday   weekday;
    bool            isdst;

    struct timeval  tv;         /* seconds and useconds */
    cstring_t       *tzone;
};

struct ctimeout_s {
    struct cdatetime_s  *dt;
    enum ctimeout       precision;
    unsigned int        interval;
};

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

static struct cdatetime_s *new_cdatetime_s(void)
{
    struct cdatetime_s *d = NULL;

    d = calloc(1, sizeof(struct cdatetime_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    d->tzone = NULL;

    return d;
}

static void destroy_cdatetime_s(struct cdatetime_s *dt)
{
    if (NULL == dt)
        return;

    if (dt->tzone != NULL)
        cstring_destroy(dt->tzone);

    free(dt);
}

static bool is_GMT(struct cdatetime_s *dt)
{
    cstring_t *s;
    bool ret = false;

    s = cstring_new("GMT");

    if (cstring_cmp(dt->tzone, s) == 0)
        ret = true;

    cstring_destroy(s);

    return ret;
}

static void cvt_time(struct cdatetime_s *dt, bool UTC)
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

    dt->tzone = cstring_new("%s", tm.tm_zone);
}

int LIBEXPORT cdt_destroy(cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_cdatetime_s(t);

    return 0;
}

cdatetime_t LIBEXPORT *cdt_localtime(void)
{
    struct cdatetime_s *dt = NULL;

    cerrno_clear();
    dt = new_cdatetime_s();

    if (NULL == dt)
        return NULL;

    if (gettimeofday(&dt->tv, NULL) < 0) {
        return NULL;
    }

    cvt_time(dt, false);

    return dt;
}

int LIBEXPORT cdt_day(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->day;
}

int LIBEXPORT cdt_month(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->month;
}

int LIBEXPORT cdt_year(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->year;
}

int LIBEXPORT cdt_hour(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->hour;
}

int LIBEXPORT cdt_minute(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->minute;
}

int LIBEXPORT cdt_second(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->second;
}

bool LIBEXPORT cdt_isdst(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->isdst;
}

bool LIBEXPORT cdt_leap_year(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return is_leap_year(t->year);
}

enum cweekday LIBEXPORT cdt_weekday(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return t->weekday;
}

unsigned int LIBEXPORT cdt_get_seconds(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return 0;
    }

    return t->tv.tv_sec;
}

unsigned long long LIBEXPORT cdt_get_mseconds(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return 0;
    }

    return ((unsigned long long)t->tv.tv_sec * 1000) + (t->tv.tv_usec / 1000);
}

unsigned long long LIBEXPORT cdt_get_useconds(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return 0;
    }

    return ((unsigned long long)t->tv.tv_sec * 1000000) + t->tv.tv_usec;
}

cstring_t LIBEXPORT *cdt_month_of_year(const cdatetime_t *dt, bool full)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;
    int moy;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    moy = t->month;

    return cstring_new("%s", (full == true) ? __moy_full[moy]
                                            : __moy_abbrv[moy]);
}

cstring_t LIBEXPORT *cdt_day_of_week(const cdatetime_t *dt, bool full)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;
    int dow;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    dow = t->weekday;

    return cstring_new("%s", (full == true) ? __dow_full[dow]
                                            : __dow_abbrv[dow]);
}

/*
 * Supported formats:
 *
 * %a - abbreviated weekday
 * %A - full weekday
 * %b - abbreviated month
 * %b - full month
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
 */
cstring_t LIBEXPORT *cdt_to_string(const cdatetime_t *dt, const char *fmt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt;
    cstring_t *d = NULL;
    int i;

    cerrno_clear();

    if ((NULL == dt) || (NULL == fmt)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    d = cstring_new("");

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
            }
        } else
            cstring_cat(d, "%c", *fmt);
    } while (*fmt++);

    return d;
}

/*
 * All comparisons are made using seconds precision.
 */

int LIBEXPORT cdt_cmp(const cdatetime_t *t1, const cdatetime_t *t2)
{
    unsigned int s1, s2;

    cerrno_clear();

    if ((NULL == t1) || (NULL == t2)) {
        cset_errno(CL_NULL_ARG);
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

bool LIBEXPORT cdt_isafter(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    cerrno_clear();

    if ((NULL == dt) || (NULL == other)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 > s2)
        return true;

    return false;
}

bool LIBEXPORT cdt_isbefore(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    cerrno_clear();

    if ((NULL == dt) || (NULL == other)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 < s2)
        return true;

    return false;
}

bool LIBEXPORT cdt_isequal(const cdatetime_t *dt, const cdatetime_t *other)
{
    unsigned int s1, s2;

    cerrno_clear();

    if ((NULL == dt) || (NULL == other)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s1 = cdt_get_seconds(dt);
    s2 = cdt_get_seconds(other);

    if (s1 == s2)
        return true;

    return false;
}

cdatetime_t LIBEXPORT *cdt_dup(const cdatetime_t *dt)
{
    struct cdatetime_s *t = NULL, *tt = (struct cdatetime_s *)dt;
    bool UTC = false;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

cdatetime_t LIBEXPORT *cdt_gmtime(void)
{
    struct cdatetime_s *dt = NULL;

    cerrno_clear();
    dt = new_cdatetime_s();

    if (NULL == dt)
        return NULL;

    if (gettimeofday(&dt->tv, NULL) < 0) {
        return NULL;
    }

    cvt_time(dt, true);

    return dt;
}

cdatetime_t LIBEXPORT *cdt_to_localtime(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt, *ddt = NULL;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ddt = cdt_dup(t);

    /* Already localtime, returns a copy */
    if (is_GMT(t) == false)
        return ddt;

    /* Convert to localtime */
    cvt_time(ddt, false);

    return ddt;
}

cdatetime_t LIBEXPORT *cdt_to_gmtime(const cdatetime_t *dt)
{
    struct cdatetime_s *t = (struct cdatetime_s *)dt, *ddt = NULL;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

cdatetime_t LIBEXPORT *cdt_mktime(unsigned int year, unsigned int month,
    unsigned int day, unsigned int hour, unsigned int minute,
    unsigned int second)
{
    struct cdatetime_s *dt = NULL;
    struct tm tm;

    cerrno_clear();

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

cdatetime_t LIBEXPORT *cdt_string_mktime(const cstring_t *datetime)
{
    cstring_t *d, *p, *s = cstring_ref((cstring_t *)datetime);;
    cstring_list_t *l, *ld, *lt;
    int day, month, year, hour, min, sec;

    cerrno_clear();

    if (NULL == s) {
        cstring_unref(s);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    d = cstring_alltrim(s);
    l = cstring_split(d, " ");
    ld = cstring_split(cstring_list_get(l, 0), "/-");
    lt = cstring_split(cstring_list_get(l, 1), ":");

    hour = cstring_value_as_int(cstring_list_get(lt, 0));
    min = cstring_value_as_int(cstring_list_get(lt, 1));
    sec = cstring_value_as_int(cstring_list_get(lt, 2));


    p = cstring_list_get(ld, 0);
    month = cstring_value_as_int(cstring_list_get(ld, 1));

    if (cstring_length(p) == 2) {
        day = cstring_value_as_int(p);
        year = cstring_value_as_int(cstring_list_get(ld, 2));
    } else {
        year = cstring_value_as_int(p);
        day = cstring_value_as_int(cstring_list_get(ld, 2));
    }

    cstring_list_destroy(lt);
    cstring_list_destroy(ld);
    cstring_list_destroy(l);
    cstring_destroy(d);
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

cdatetime_t LIBEXPORT *cdt_minus_years(const cdatetime_t *dt, unsigned int years)
{
    struct cdatetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

cdatetime_t LIBEXPORT *cdt_minus_days(const cdatetime_t *dt, unsigned int days)
{
    struct cdatetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

/*cdatetime_t LIBEXPORT *cdt_minus_months(const cdatetime_t *dt,
    unsigned int months)
{
    struct cdatetime_s *ddt = NULL;
    unsigned int n;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;
*/
    /* TODO */
/*
    return ddt;
}*/

cdatetime_t LIBEXPORT *cdt_minus_seconds(const cdatetime_t *dt,
    unsigned int seconds)
{
    struct cdatetime_s *ddt = NULL;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec -= seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

cdatetime_t LIBEXPORT *cdt_minus_minutes(const cdatetime_t *dt,
    unsigned int minutes)
{
    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cdt_minus_seconds(dt, minutes * 60);
}

cdatetime_t LIBEXPORT *cdt_minus_hours(const cdatetime_t *dt, unsigned int hours)
{
    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cdt_minus_seconds(dt, hours * 3600);
}

cdatetime_t LIBEXPORT *cdt_plus_days(const cdatetime_t *dt, unsigned int days)
{
    struct cdatetime_s *ddt = NULL;
    unsigned int n;
    bool UTC = false;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

/*cdatetime_t LIBEXPORT *cdt_plus_months(const cdatetime_t *dt, unsigned int months)
{
}*/

cdatetime_t LIBEXPORT *cdt_plus_years(const cdatetime_t *dt, unsigned int years)
{
    struct cdatetime_s *ddt = NULL;
    unsigned int n;
    int leap_years=0;
    bool UTC = false;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

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

cdatetime_t LIBEXPORT *cdt_plus_seconds(const cdatetime_t *dt,
    unsigned int seconds)
{
    struct cdatetime_s *ddt = NULL;

    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ddt = cdt_dup(dt);

    if (NULL == ddt)
        return NULL;

    ddt->tv.tv_sec += seconds;
    cvt_time(ddt, is_GMT(ddt));

    return ddt;
}

cdatetime_t LIBEXPORT *cdt_plus_minutes(const cdatetime_t *dt,
    unsigned int minutes)
{
    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cdt_plus_seconds(dt, minutes * 60);
}

cdatetime_t LIBEXPORT *cdt_plus_hours(const cdatetime_t *dt, unsigned int hours)
{
    cerrno_clear();

    if (NULL == dt) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cdt_plus_seconds(dt, hours * 3600);
}

enum cweekday LIBEXPORT cdt_current_weekday(void)
{
    cdatetime_t *dt = NULL;
    enum cweekday w;

    cerrno_clear();
    dt = cdt_localtime();

    if (NULL == dt)
        return -1;

    w = cdt_weekday(dt);
    cdt_destroy(dt);

    return w;
}

enum cmonth LIBEXPORT cdt_current_month(void)
{
    cdatetime_t *dt = NULL;
    enum cmonth m;

    cerrno_clear();
    dt = cdt_localtime();

    if (NULL == dt)
        return -1;

    m = cdt_month(dt);
    cdt_destroy(dt);

    return m;
}

bool LIBEXPORT cdt_is_local_dst(void)
{
    time_t t;
    struct tm tm;

    cerrno_clear();
    time(&t);
    localtime_r(&t, &tm);

    if (tm.tm_isdst)
        return true;

    return false;
}

bool LIBEXPORT cdt_is_leap_year(void)
{
    time_t t;
    struct tm tm;

    cerrno_clear();
    time(&t);
    localtime_r(&t, &tm);

    return is_leap_year(tm.tm_year + 1900);
}

static struct ctimeout_s *new_ctimeout_s(unsigned int interval,
    enum ctimeout precision)
{
    cdatetime_t *dt = NULL;
    struct ctimeout_s *t = NULL;

    dt = cdt_localtime();

    if (NULL == dt)
        return NULL;

    t = calloc(1, sizeof(struct ctimeout_s));

    if (NULL == t) {
        cdt_destroy(dt);
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    t->dt = dt;
    t->precision = precision;
    t->interval = interval;

    return t;
}

static void destroy_ctimeout_s(struct ctimeout_s *t)
{
    if (NULL == t)
        return;

    if (t->dt != NULL)
        cdt_destroy(t->dt);

    free(t);
}

ctimeout_t LIBEXPORT *cdt_inic_timeout(unsigned int interval,
   enum ctimeout precision)
{
    struct ctimeout_s *t;

    cerrno_clear();
    t = new_ctimeout_s(interval, precision);

    if (NULL == t)
        return NULL;

    return t;
}

int LIBEXPORT cdt_destroy_timeout(ctimeout_t *t)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_ctimeout_s(ct);

    return 0;
}

int LIBEXPORT cdt_reset_timeout(ctimeout_t *t, unsigned int interval,
    enum ctimeout precision)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (ct->dt != NULL)
        cdt_destroy(ct->dt);

    ct->dt = cdt_localtime();

    if (NULL == ct->dt)
        return -1;

    ct->precision = precision;
    ct->interval = interval;

    return 0;
}

bool LIBEXPORT cdt_expired_timeout(const ctimeout_t *t)
{
    struct ctimeout_s *ct = (struct ctimeout_s *)t;
    struct timeval tv;
    unsigned int i;
    unsigned long long l;

    cerrno_clear();

    if (NULL == t) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    gettimeofday(&tv, NULL);

    switch (ct->precision) {
        case CL_TM_SECONDS:
            i = cdt_get_seconds(ct->dt) + ct->interval;

            if (tv.tv_sec > (time_t)i)
                return true;

            break;

        case CL_TM_MSECONDS:
            i = cdt_get_mseconds(ct->dt) + ct->interval;

            if ((unsigned int)((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) > i)
                return true;

            break;

        case CL_TM_USECONDS:
            l = cdt_get_useconds(ct->dt) + ct->interval;

            if ((unsigned long long)((tv.tv_sec * 1000000) + tv.tv_usec) > l)
                return true;

            break;
    }

    return false;
}

