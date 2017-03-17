
/*
 * Description: API to use POSIX timers to run specific tasks.
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
#include <string.h>
#include <signal.h>

#include <pthread.h>

#include "collections.h"

#define DEFAULT_TIMER_FINISH_TIMEOUT                200 /* milliseconds */

/* Structure to constant informations about a timer for the user */
#define ctimer_info_members         \
    cl_struct_member(char **, info) \
    cl_struct_member(void *, data)

cl_struct_declare(ctimer_info_s, ctimer_info_members);

#define ctimer_info_s       cl_struct(ctimer_info_s)

/* Internal timer information */
struct ctimer_internal_data_s {
    char            *name;
    void            *data;
    unsigned int    finish_timeout;

    /* internal */
    void            *timers_list;
};

/* Timer */
struct ctimer_s {
    clist_entry_t                   *prev;
    clist_entry_t                   *next;
    struct cobject_hdr              hdr;
    struct ctimer_internal_data_s   tid;
    enum cl_timer_state               state;
    enum cl_timer_interval_mode       imode;
    struct sigevent                 evp;
    union sigval                    sigval;
    pthread_attr_t                  attr;
    timer_t                         timerid;
    struct itimerspec               its;
    struct timespec                 ts_disarmed;

    /*
     * User functions called to initialize and to end custom environments to
     * a specific timer.
     */
    int                             (*init)(void *);
    int                             (*uninit)(void *);
};

#define CTIMER_OBJECT_OFFSET            \
    (sizeof(clist_entry_t *) + sizeof(clist_entry_t *))

static int __search_timer(void *a, void *b)
{
    struct ctimer_s *timer = (struct ctimer_s *)a;
    char *name = (char *)b;

    if (strcmp(timer->tid.name, name) == 0)
        return 1;

    return 0;
}

/*
 * Search for a specific timer inside the registered timers list using its
 * as key.
 */
static struct ctimer_s *search_timer(struct ctimer_s *timer,
    const char *timer_name)
{
    return cdll_map(timer, __search_timer, (void *)timer_name);
}

static void gettime(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
}

/*
 * Function to set actual state of a timer inside the library environment.
 */
static int set_state(struct ctimer_s *timer, enum cl_timer_state state)
{
    if (state >= TIMER_MAX_STATE) {
        cset_errno(CL_INVALID_STATE);
        return -1;
    }

    /*
     * Handle two special states: TIMER_ST_RUNNING and TIMER_ST_WAITING,
     * to, in the first case, suspend the timer execution and after the
     * second, resume its execution normally, taking into account the
     * time spent by the function during its execution.
     */

    if (state == TIMER_ST_RUNNING) {
        if (ctimer_disarm(timer) < 0)
            return -1;
    } else if (state == TIMER_ST_WAITING) {
        if (ctimer_arm(timer) < 0)
            return -1;
    }

    timer->state = state;

    return 0;
}

__PUB_API__ int ctimer_set_state(ctimer_arg_t arg, enum cl_timer_state state)
{
    struct ctimer_s *t = NULL, *tlist = NULL;
    char *timer_name = NULL;
    struct ctimer_internal_data_s *tid;

    __clib_function_init__(false, NULL, -1, -1);
    tid = arg.sival_ptr;
    timer_name = tid->name;

    if (NULL == timer_name) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    tlist = tid->timers_list;

    if (NULL == tlist) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    t = search_timer(tlist, timer_name);

    if (NULL == t) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    return set_state(t, state);
}

__PUB_API__ ctimer_t *ctimer_get_timer(const ctimer_t *timers_list,
    const char *timer_name)
{
    struct ctimer_s *tlist = (struct ctimer_s *)timers_list;
    struct ctimer_s *t = NULL;

    __clib_function_init_ex__(true, timers_list, CTIMER, CTIMER_OBJECT_OFFSET,
                              NULL);

    if (timer_name == NULL) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    t = search_timer(tlist, timer_name);

    if (NULL == t) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    return t;
}

static bool validate_imode(enum cl_timer_interval_mode imode)
{
    if ((imode == TIMER_IMODE_DEFAULT) ||
        (imode == TIMER_IMODE_DISCOUNT_RUNTIME))
    {
        return true;
    }

    return false;
}

static const char *translate_imode(enum cl_timer_interval_mode imode)
{
    switch (imode) {
        case TIMER_IMODE_DEFAULT:
            return "default";

        case TIMER_IMODE_DISCOUNT_RUNTIME:
            return "discount runtime";

        default:
            break;
    }

    return "unknown";
}

__PUB_API__ int ctimer_update_interval(ctimer_t *timer, unsigned int interval)
{
    struct ctimer_s *t = (struct ctimer_s *)timer;

    __clib_function_init_ex__(true, timer, CTIMER, CTIMER_OBJECT_OFFSET, -1);

    /* Disables timer */
    ctimer_disarm(t);

    /* Update its new execution interval */
    t->its.it_interval.tv_sec = interval;
    t->its.it_interval.tv_nsec = 0;

    /* Enables the timer again */
    ctimer_arm(t);

    return 0;
}

static ctimer_info_s *new_timer_info(struct ctimer_s *timer)
{
    ctimer_info_s *i;

    i = calloc(1, sizeof(ctimer_info_s));

    if (NULL == i) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    i->info = calloc(TIMER_MAX_INFO, sizeof(char *));

    if (NULL == i->info) {
        cset_errno(CL_NO_MEM);
        free(i);
        return NULL;
    }

    i->info[TIMER_INFO_NAME] = strdup(timer->tid.name);
    asprintf(&i->info[TIMER_INFO_INTERVAL], "%ld",
             timer->its.it_interval.tv_sec);

    asprintf(&i->info[TIMER_INFO_OVERRUN], "%d",
             (timer->state >= TIMER_ST_INSTALLED)
                    ? timer_getoverrun(timer->timerid)
                    : 0);

    asprintf(&i->info[TIMER_INFO_FINISH_TIMEOUT], "%d",
             timer->tid.finish_timeout);

    asprintf(&i->info[TIMER_INFO_IMODE], "%d", timer->imode);
    asprintf(&i->info[TIMER_INFO_IMODE_DESC], "%s",
             translate_imode(timer->imode));

    i->data = timer->tid.data;
    set_typeof(CTIMER_INFO, i);

    return i;
}

static void destroy_timer_info(ctimer_info_s *info)
{
    int i;

    for (i = 0; i < TIMER_MAX_INFO; i++) {
        if (i == TIMER_INFO_DATA)
            continue;

        free(info->info[i]);
    }

    free(info->info);
    free(info);
}

static ctimer_info_s *get_timer_info(struct ctimer_s *timer)
{
    ctimer_info_s *inode;

    inode = new_timer_info(timer);

    if (NULL == inode)
        return NULL;

    return inode;
}

__PUB_API__ int ctimer_unload_info(ctimer_info_t *timer_info)
{
    ctimer_info_s *tinfo = (ctimer_info_s *)timer_info;

    __clib_function_init__(true, timer_info, CTIMER_INFO, -1);
    destroy_timer_info(tinfo);

    return 0;
}

__PUB_API__ ctimer_info_t *ctimer_load_info(const ctimer_t *timer)
{
    struct ctimer_s *t = (struct ctimer_s *)timer;

    __clib_function_init_ex__(true, timer, CTIMER, CTIMER_OBJECT_OFFSET, NULL);

    return get_timer_info(t);
}

__PUB_API__ ctimer_info_t *ctimer_load_info_within_timer(ctimer_arg_t arg)
{
    struct ctimer_s *t = NULL, *tlist;
    char *timer_name;
    struct ctimer_internal_data_s *tid;

    __clib_function_init__(false, NULL, -1, NULL);

    /* Extracts timer identifications */
    tid = arg.sival_ptr;
    timer_name = tid->name;

    if (NULL == timer_name) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    tlist = tid->timers_list;

    if (NULL == tlist) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    t = search_timer(tlist, timer_name);

    if (NULL == t) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    return get_timer_info(t);
}

__PUB_API__ void *ctimer_get_info_data(const ctimer_info_t *timer_info,
    enum cl_timer_info_field info)
{
    ctimer_info_s *tinfo = (ctimer_info_s *)timer_info;

    __clib_function_init__(true, timer_info, CTIMER_INFO, NULL);

    if (info >= TIMER_MAX_INFO) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    if (info == TIMER_INFO_DATA)
        return tinfo->data;

    /*
     * XXX: The user must know what he's doing, this way it's his responsability
     *      to do a cast to this returned value.
     */
    return (void *)tinfo->info[info];
}

static struct ctimer_s *new_timer(const char *timer_name)
{
    struct ctimer_s *t;

    t = calloc(1, sizeof(struct ctimer_s));

    if (NULL == t) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    t->tid.name = strdup(timer_name);
    set_typeof_with_offset(CTIMER, t, CTIMER_OBJECT_OFFSET);
    set_state(t, TIMER_ST_CREATED);

    return t;
}

static void destroy_timer(struct ctimer_s *timer)
{
    free(timer->tid.name);
    free(timer);
}

/*
 * Sets a few internal timer informations, such as thread attributes, thread
 * arguments and execution interval.
 */
static void set_timer_info(struct ctimer_s *timer, unsigned int exec_interval,
    enum cl_timer_interval_mode imode, unsigned int finish_timeout,
    void *timer_function, void *arg)
{
    /* Sets thread attributes */
    pthread_attr_init(&timer->attr);
    pthread_attr_setdetachstate(&timer->attr, PTHREAD_CREATE_DETACHED);

    /* Whenever informed 0, the closing timeout will be the default. */
    if (finish_timeout == 0)
        timer->tid.finish_timeout = DEFAULT_TIMER_FINISH_TIMEOUT;
    else
        timer->tid.finish_timeout = finish_timeout;

    /* Stores the user data to pass to the timer function. */
    timer->tid.data = arg;

    /* Initialize timer informations */
    timer->sigval.sival_ptr = &timer->tid;
    timer->evp.sigev_notify = SIGEV_THREAD;
    timer->evp.sigev_value = timer->sigval;
    timer->evp.sigev_notify_function = timer_function;
    timer->evp.sigev_notify_attributes = &timer->attr;

    /* Also initialize execution timer informations */
    timer->its.it_value.tv_sec = 0;
    timer->its.it_value.tv_nsec = 0;
    timer->its.it_interval.tv_sec = exec_interval;
    timer->its.it_interval.tv_nsec = 0;

    timer->imode = imode;
    set_state(timer, TIMER_ST_REGISTERED);
}

__PUB_API__ int ctimer_register(ctimer_t *timers_list, unsigned int exec_interval,
    enum cl_timer_interval_mode imode, unsigned int finish_timeout,
    const char *timer_name, void (*timer_function)(ctimer_arg_t), void *arg,
    int (*init_function)(void *), int (*uninit_function)(void *))
{
    struct ctimer_s **tlist = timers_list;
    struct ctimer_s *t;

//    __clib_function_init_ex__(true, timers_list, CTIMER,
//                              CTIMER_OBJECT_OFFSET, -1);

    if ((NULL == timer_name) || (NULL == timer_function)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if ((exec_interval == 0) || (validate_imode(imode) == false)) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    t = new_timer(timer_name);

    if (NULL == t)
        return -1;

    /* Call timer initialization function */
    if (init_function != NULL) {
        if ((init_function)(arg) < 0) {
            destroy_timer(t);
            cset_errno(CL_EXTERNAL_INIT_ERROR);
            return -1;
        }
    }

    t->init = init_function;
    t->uninit = uninit_function;

    set_timer_info(t, exec_interval, imode, finish_timeout, timer_function,
                   arg);

    *tlist = cdll_unshift(*tlist, t);

    return 0;
}

/*
 * Wait for a timer to end. Returns which was the ending format, 0 to normal
 * 1 for timeout expiration.
 */
static int wait_for_timer_to_end(struct ctimer_s *timer)
{
    ctimeout_t *t;
    int ret = 0;

    t = ctimeout_create(timer->tid.finish_timeout, CL_TM_MSECONDS);

    while (1) {
        if (timer->state == TIMER_ST_WAITING) {
            ret = 0;
            break;
        }

        if (ctimeout_expired(t) == true) {
            ret = 1;
            break;
        }
    }

    ctimeout_destroy(t);

    return ret;
}

static int unregister_timer(void *a)
{
    struct ctimer_s *timer = (struct ctimer_s *)a;

    /*
     * Disables the timer preventing it from running while in process of
     * closing.
     */
    if (timer->state != TIMER_ST_REGISTERED) {
        ctimer_disarm(timer);

        /* Wait for timer completion. */
        if (wait_for_timer_to_end(timer) == 1) {
            cset_errno(CL_ENDED_WITH_TIMEOUT);

            /*
             * XXX: Do not return in this point because whe still need to remove
             *      some timer informations.
             */
        }
    }

    if (timer->uninit != NULL) {
        set_state(timer, TIMER_ST_TERMINATING);

        /* Call timer ending function */
        if ((timer->uninit)(timer->tid.data) < 0) {
            cset_errno(CL_EXTERNAL_UNINIT_ERROR);
            return -1;
        }
    }

    if (timer->state != TIMER_ST_REGISTERED)
        timer_delete(timer->timerid);

    set_state(timer, TIMER_ST_FINALIZED);
    destroy_timer(timer);

    return 0;
}

__PUB_API__ int ctimer_unregister(ctimer_t *timers_list, const char *timer_name)
{
    struct ctimer_s *tlist = (struct ctimer_s *)timers_list;
    struct ctimer_s *t;
    int ret = 0;

    __clib_function_init_ex__(true, timers_list, CTIMER, CTIMER_OBJECT_OFFSET,
                              -1);

    if (NULL == timer_name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    t = search_timer(tlist, timer_name);

    if (NULL == t) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    ret = unregister_timer(t);

    if ((ret < 0) || (cget_last_error() != 0))
        return -1;

    return 0;
}

/*
 * Fixes the timer initialization moment (its first execution) according to
 * CLOCK_REALTIME, using the value defined as the interval timer execution
 * as the waiting time for this first execution.
 */
static void adjust_timer_start_time(struct ctimer_s *timer)
{
    struct timespec ts;

    gettime(&ts);

    timer->its.it_value.tv_sec = ts.tv_sec + timer->its.it_interval.tv_sec;
    timer->its.it_value.tv_nsec = ts.tv_nsec;
}

static int install_timer(void *a, void *b __attribute__((unused)))
{
    struct ctimer_s *timer = (struct ctimer_s *)a;
    struct ctimer_s *tlist = (struct ctimer_s *)b;

    if (timer_create(CLOCK_REALTIME, &timer->evp, &timer->timerid) == -1) {
        cset_errno(CL_CREATE_FAILED);
        return -1;
    }

    adjust_timer_start_time(timer);

    /* Puts the timer in execution */
    if (timer_settime(timer->timerid, TIMER_ABSTIME, &timer->its,
                      NULL) == -1)
    {
        cset_errno(CL_SETTIME_FAILED);
        return -1;
    }

    /*
     * Saves the timers list pointer to allow access it inside the timer
     * function when needed, in a transparent way for the user.
     */
    timer->tid.timers_list = tlist;

    return 0;
}

__PUB_API__ int ctimer_install(ctimer_t *timers_list)
{
    struct ctimer_s *tlist = (struct ctimer_s *)timers_list;
    struct ctimer_s *t = NULL;

    __clib_function_init_ex__(true, timers_list, CTIMER, CTIMER_OBJECT_OFFSET,
                              -1);

    t = cdll_map(tlist, install_timer, tlist);

    if (t != NULL)
        return -1;

    return 0;
}

/*
 * Just a way to avoid warning while calling this function from 'cdll_free' and
 * continue to use just one function to release a timer.
 */
static void __unregister_timer(void *a)
{
    unregister_timer(a);
}

__PUB_API__ int ctimer_uninstall(ctimer_t *timers_list)
{
    struct ctimer_s *tlist = (struct ctimer_s *)timers_list;

    __clib_function_init_ex__(true, timers_list, CTIMER, CTIMER_OBJECT_OFFSET,
                              -1);

    cdll_free(tlist, __unregister_timer);

    if (cget_last_error() != 0)
        return -1;

    return 0;
}

__PUB_API__ int ctimer_disarm(ctimer_t *timer)
{
    struct ctimer_s *t = (struct ctimer_s *)timer;

    __clib_function_init_ex__(true, timer, CTIMER, CTIMER_OBJECT_OFFSET, -1);

    /*
     * Saves the current time to put the timer in execution again in the
     * right time.
     */
    gettime(&t->ts_disarmed);

    t->its.it_value.tv_sec = 0;
    t->its.it_value.tv_nsec = 0;

    /* Suspends timer execution */
    if (timer_settime(t->timerid, TIMER_ABSTIME, &t->its, NULL) == -1) {
        cset_errno(CL_SETTIME_FAILED);
        return -1;
    }

    return 0;
}

__PUB_API__ int ctimer_arm(ctimer_t *timer)
{
    struct ctimer_s *t = (struct ctimer_s *)timer;
    struct timespec ts;
    time_t sec = 0;
    long nsec = 0;

    __clib_function_init_ex__(true, timer, CTIMER, CTIMER_OBJECT_OFFSET, -1);

    if (t->imode == TIMER_IMODE_DEFAULT)
        adjust_timer_start_time(timer);
    else if (t->imode == TIMER_IMODE_DISCOUNT_RUNTIME) {
        gettime(&ts);

        sec = t->its.it_interval.tv_sec -
            (ts.tv_sec - t->ts_disarmed.tv_sec);

        if (sec < 0)
            sec = 0;

        nsec = t->its.it_interval.tv_nsec -
            (ts.tv_nsec - t->ts_disarmed.tv_nsec);

        if (nsec < 0)
            nsec = 0;

        t->its.it_value.tv_sec = ts.tv_sec + sec;
        t->its.it_value.tv_nsec = ts.tv_nsec + nsec;
    }

    /* Puts the timer in execution */
    if (timer_settime(t->timerid, TIMER_ABSTIME, &t->its, NULL) == -1) {
        cset_errno(CL_SETTIME_FAILED);
        return -1;
    }

    return 0;
}

