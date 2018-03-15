
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

#define DEFAULT_CL_TIMER_FINISH_TIMEOUT                200 /* milliseconds */

/* Structure to constant information about a timer for the user */
#define cl_timer_info_members       \
    cl_struct_member(char **, info) \
    cl_struct_member(void *, data)

cl_struct_declare(cl_timer_info_s, cl_timer_info_members);

#define cl_timer_info_s       cl_struct(cl_timer_info_s)

/* Internal timer information */
struct cl_timer_internal_data_s {
    char            *name;
    void            *data;
    unsigned int    finish_timeout;

    /* internal */
    void            *timers_list;
};

/* Timer */
struct cl_timer_s {
    cl_list_entry_t                 *prev;
    cl_list_entry_t                 *next;
    struct cl_object_hdr            hdr;
    struct cl_timer_internal_data_s tid;
    enum cl_timer_state             state;
    enum cl_timer_interval_mode     imode;
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

#define CL_TIMER_OBJECT_OFFSET            \
    (sizeof(cl_list_entry_t *) + sizeof(cl_list_entry_t *))

static int __search_timer(void *a, void *b)
{
    struct cl_timer_s *timer = (struct cl_timer_s *)a;
    char *name = (char *)b;

    if (strcmp(timer->tid.name, name) == 0)
        return 1;

    return 0;
}

/*
 * Search for a specific timer inside the registered timers list using its
 * as key.
 */
static struct cl_timer_s *search_timer(struct cl_timer_s *timer,
    const char *timer_name)
{
    return cl_dll_map(timer, __search_timer, (void *)timer_name);
}

static void gettime(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
}

/*
 * Function to set actual state of a timer inside the library environment.
 */
static int set_state(struct cl_timer_s *timer, enum cl_timer_state state)
{
    if (state >= CL_TIMER_MAX_STATE) {
        cset_errno(CL_INVALID_STATE);
        return -1;
    }

    /*
     * Handle two special states: CL_TIMER_ST_RUNNING and CL_TIMER_ST_WAITING,
     * to, in the first case, suspend the timer execution and after the
     * second, resume its execution normally, taking into account the
     * time spent by the function during its execution.
     */

    if (state == CL_TIMER_ST_RUNNING) {
        if (cl_timer_disarm(timer) < 0)
            return -1;
    } else if (state == CL_TIMER_ST_WAITING) {
        if (cl_timer_arm(timer) < 0)
            return -1;
    }

    timer->state = state;

    return 0;
}

__PUB_API__ int cl_timer_set_state(cl_timer_arg_t arg, enum cl_timer_state state)
{
    struct cl_timer_s *t = NULL, *tlist = NULL;
    char *timer_name = NULL;
    struct cl_timer_internal_data_s *tid;

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

__PUB_API__ cl_timer_t *cl_timer_get_timer(const cl_timer_t *timers_list,
    const char *timer_name)
{
    struct cl_timer_s *tlist = (struct cl_timer_s *)timers_list;
    struct cl_timer_s *t = NULL;

    __clib_function_init_ex__(true, timers_list, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, NULL);

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
    if ((imode == CL_TIMER_IMODE_DEFAULT) ||
        (imode == CL_TIMER_IMODE_DISCOUNT_RUNTIME))
    {
        return true;
    }

    return false;
}

static const char *translate_imode(enum cl_timer_interval_mode imode)
{
    switch (imode) {
        case CL_TIMER_IMODE_DEFAULT:
            return "default";

        case CL_TIMER_IMODE_DISCOUNT_RUNTIME:
            return "discount runtime";

        default:
            break;
    }

    return "unknown";
}

__PUB_API__ int cl_timer_update_interval(cl_timer_t *timer, unsigned int interval)
{
    struct cl_timer_s *t = (struct cl_timer_s *)timer;

    __clib_function_init_ex__(true, timer, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

    /* Disables timer */
    cl_timer_disarm(t);

    /* Update its new execution interval */
    t->its.it_interval.tv_sec = interval;
    t->its.it_interval.tv_nsec = 0;

    /* Enables the timer again */
    cl_timer_arm(t);

    return 0;
}

static cl_timer_info_s *new_timer_info(struct cl_timer_s *timer)
{
    cl_timer_info_s *i;

    i = calloc(1, sizeof(cl_timer_info_s));

    if (NULL == i) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    i->info = calloc(CL_TIMER_MAX_INFO, sizeof(char *));

    if (NULL == i->info) {
        cset_errno(CL_NO_MEM);
        free(i);
        return NULL;
    }

    i->info[CL_TIMER_INFO_NAME] = strdup(timer->tid.name);
    asprintf(&i->info[CL_TIMER_INFO_INTERVAL], "%ld",
             timer->its.it_interval.tv_sec);

#ifdef GNU_LINUX
    asprintf(&i->info[CL_TIMER_INFO_OVERRUN], "%d",
             (timer->state >= CL_TIMER_ST_INSTALLED)
                    ? timer_getoverrun(timer->timerid)
                    : 0);
#else
    /* We don't have this info... */
    asprintf(&i->info[CL_TIMER_INFO_OVERRUN], "%d", 0);
#endif

    asprintf(&i->info[CL_TIMER_INFO_FINISH_TIMEOUT], "%d",
             timer->tid.finish_timeout);

    asprintf(&i->info[CL_TIMER_INFO_IMODE], "%d", timer->imode);
    asprintf(&i->info[CL_TIMER_INFO_IMODE_DESC], "%s",
             translate_imode(timer->imode));

    i->data = timer->tid.data;
    typeof_set(CL_OBJ_TIMER_INFO, i);

    return i;
}

static void destroy_timer_info(cl_timer_info_s *info)
{
    int i;

    for (i = 0; i < CL_TIMER_MAX_INFO; i++) {
        if (i == CL_TIMER_INFO_DATA)
            continue;

        free(info->info[i]);
    }

    free(info->info);
    free(info);
}

static cl_timer_info_s *get_timer_info(struct cl_timer_s *timer)
{
    cl_timer_info_s *inode;

    inode = new_timer_info(timer);

    if (NULL == inode)
        return NULL;

    return inode;
}

__PUB_API__ int cl_timer_unload_info(cl_timer_info_t *timer_info)
{
    cl_timer_info_s *tinfo = (cl_timer_info_s *)timer_info;

    __clib_function_init__(true, timer_info, CL_OBJ_TIMER_INFO, -1);
    destroy_timer_info(tinfo);

    return 0;
}

__PUB_API__ cl_timer_info_t *cl_timer_load_info(const cl_timer_t *timer)
{
    struct cl_timer_s *t = (struct cl_timer_s *)timer;

    __clib_function_init_ex__(true, timer, CL_OBJ_TIMER, CL_TIMER_OBJECT_OFFSET,
                              NULL);

    return get_timer_info(t);
}

__PUB_API__ cl_timer_info_t *cl_timer_load_info_within_timer(cl_timer_arg_t arg)
{
    struct cl_timer_s *t = NULL, *tlist;
    char *timer_name;
    struct cl_timer_internal_data_s *tid;

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

__PUB_API__ void *cl_timer_get_info_data(const cl_timer_info_t *timer_info,
    enum cl_timer_info_field info)
{
    cl_timer_info_s *tinfo = (cl_timer_info_s *)timer_info;

    __clib_function_init__(true, timer_info, CL_OBJ_TIMER_INFO, NULL);

    if (info >= CL_TIMER_MAX_INFO) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    if (info == CL_TIMER_INFO_DATA)
        return tinfo->data;

    /*
     * XXX: The user must know what he's doing, this way it's his responsability
     *      to do a cast to this returned value.
     */
    return (void *)tinfo->info[info];
}

static struct cl_timer_s *new_timer(const char *timer_name)
{
    struct cl_timer_s *t;

    t = calloc(1, sizeof(struct cl_timer_s));

    if (NULL == t) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    t->tid.name = strdup(timer_name);
    typeof_set_with_offset(CL_OBJ_TIMER, t, CL_TIMER_OBJECT_OFFSET);
    set_state(t, CL_TIMER_ST_CREATED);

    return t;
}

static void destroy_timer(struct cl_timer_s *timer)
{
    free(timer->tid.name);
    free(timer);
}

/*
 * Sets a few internal timer information, such as thread attributes, thread
 * arguments and execution interval.
 */
static void set_timer_info(struct cl_timer_s *timer, unsigned int exec_interval,
    enum cl_timer_interval_mode imode, unsigned int finish_timeout,
    void *timer_function, void *arg)
{
    /* Sets thread attributes */
    pthread_attr_init(&timer->attr);
    pthread_attr_setdetachstate(&timer->attr, PTHREAD_CREATE_DETACHED);

    /* Whenever informed 0, the closing timeout will be the default. */
    if (finish_timeout == 0)
        timer->tid.finish_timeout = DEFAULT_CL_TIMER_FINISH_TIMEOUT;
    else
        timer->tid.finish_timeout = finish_timeout;

    /* Stores the user data to pass to the timer function. */
    timer->tid.data = arg;

    /* Initialize timer information */
    timer->sigval.sival_ptr = &timer->tid;
    timer->evp.sigev_notify = SIGEV_THREAD;
    timer->evp.sigev_value = timer->sigval;
    timer->evp.sigev_notify_function = timer_function;
    timer->evp.sigev_notify_attributes = &timer->attr;

    /* Also initialize execution timer information */
    timer->its.it_value.tv_sec = 0;
    timer->its.it_value.tv_nsec = 0;
    timer->its.it_interval.tv_sec = exec_interval;
    timer->its.it_interval.tv_nsec = 0;

    timer->imode = imode;
    set_state(timer, CL_TIMER_ST_REGISTERED);
}

__PUB_API__ int cl_timer_register(cl_timer_t *timers_list,
    unsigned int exec_interval, enum cl_timer_interval_mode imode,
    unsigned int finish_timeout, const char *timer_name,
    void (*timer_function)(cl_timer_arg_t), void *arg,
    int (*init_function)(void *), int (*uninit_function)(void *))
{
    struct cl_timer_s **tlist = timers_list;
    struct cl_timer_s *t;

    if (*tlist == NULL) {
        __clib_function_init__(false, NULL, -1, -1);
    } else {
        __clib_function_init_ex__(true, *tlist, CL_OBJ_TIMER,
                                  CL_TIMER_OBJECT_OFFSET, -1);
    }

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

    *tlist = cl_dll_unshift(*tlist, t);

    return 0;
}

/*
 * Wait for a timer to end. Returns which was the ending format, 0 to normal
 * 1 for timeout expiration.
 */
static int wait_for_timer_to_end(struct cl_timer_s *timer)
{
    cl_timeout_t *t;
    int ret = 0;

    t = cl_timeout_create(timer->tid.finish_timeout, CL_TM_MSECONDS);

    while (1) {
        if (timer->state == CL_TIMER_ST_WAITING) {
            ret = 0;
            break;
        }

        if (cl_timeout_expired(t) == true) {
            ret = 1;
            break;
        }
    }

    cl_timeout_destroy(t);

    return ret;
}

static int unregister_timer(void *a)
{
    struct cl_timer_s *timer = (struct cl_timer_s *)a;

    /*
     * Disables the timer preventing it from running while in process of
     * closing.
     */
    if (timer->state != CL_TIMER_ST_REGISTERED) {
        cl_timer_disarm(timer);

        /* Wait for timer completion. */
        if (wait_for_timer_to_end(timer) == 1) {
            cset_errno(CL_ENDED_WITH_TIMEOUT);

            /*
             * XXX: Do not return in this point because whe still need to remove
             *      some timer information.
             */
        }
    }

    if (timer->uninit != NULL) {
        set_state(timer, CL_TIMER_ST_TERMINATING);

        /* Call timer ending function */
        if ((timer->uninit)(timer->tid.data) < 0) {
            cset_errno(CL_EXTERNAL_UNINIT_ERROR);
            return -1;
        }
    }

    if (timer->state != CL_TIMER_ST_REGISTERED)
        timer_delete(timer->timerid);

    set_state(timer, CL_TIMER_ST_FINALIZED);
    destroy_timer(timer);

    return 0;
}

__PUB_API__ int cl_timer_unregister(cl_timer_t *timers_list,
    const char *timer_name)
{
    struct cl_timer_s *tlist = (struct cl_timer_s *)timers_list;
    struct cl_timer_s *t;
    int ret = 0;

    __clib_function_init_ex__(true, timers_list, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

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

    if ((ret < 0) || (cl_get_last_error() != 0))
        return -1;

    return 0;
}

/*
 * Fixes the timer initialization moment (its first execution) according to
 * CLOCK_REALTIME, using the value defined as the interval timer execution
 * as the waiting time for this first execution.
 */
static void adjust_timer_start_time(struct cl_timer_s *timer)
{
    struct timespec ts;

    gettime(&ts);

    timer->its.it_value.tv_sec = ts.tv_sec + timer->its.it_interval.tv_sec;
    timer->its.it_value.tv_nsec = ts.tv_nsec;
}

static int install_timer(void *a, void *b __attribute__((unused)))
{
    struct cl_timer_s *timer = (struct cl_timer_s *)a;
    struct cl_timer_s *tlist = (struct cl_timer_s *)b;

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

__PUB_API__ int cl_timer_install(cl_timer_t *timers_list)
{
    struct cl_timer_s *tlist = (struct cl_timer_s *)timers_list;
    struct cl_timer_s *t = NULL;

    __clib_function_init_ex__(true, timers_list, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

    t = cl_dll_map(tlist, install_timer, tlist);

    if (t != NULL)
        return -1;

    return 0;
}

/*
 * Just a way to avoid warning while calling this function from 'cl_dll_free' and
 * continue to use just one function to release a timer.
 */
static void __unregister_timer(void *a)
{
    unregister_timer(a);
}

__PUB_API__ int cl_timer_uninstall(cl_timer_t *timers_list)
{
    struct cl_timer_s *tlist = (struct cl_timer_s *)timers_list;

    __clib_function_init_ex__(true, timers_list, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

    cl_dll_free(tlist, __unregister_timer);

    if (cl_get_last_error() != 0)
        return -1;

    return 0;
}

__PUB_API__ int cl_timer_disarm(cl_timer_t *timer)
{
    struct cl_timer_s *t = (struct cl_timer_s *)timer;

    __clib_function_init_ex__(true, timer, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

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

__PUB_API__ int cl_timer_arm(cl_timer_t *timer)
{
    struct cl_timer_s *t = (struct cl_timer_s *)timer;
    struct timespec ts;
    time_t sec = 0;
    long nsec = 0;

    __clib_function_init_ex__(true, timer, CL_OBJ_TIMER,
                              CL_TIMER_OBJECT_OFFSET, -1);

    if (t->imode == CL_TIMER_IMODE_DEFAULT)
        adjust_timer_start_time(timer);
    else if (t->imode == CL_TIMER_IMODE_DISCOUNT_RUNTIME) {
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

