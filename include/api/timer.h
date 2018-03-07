
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

#ifndef _COLLECTIONS_API_TIMER_H
#define _COLLECTIONS_API_TIMER_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <timer.h> directly; include <collections.h> instead."
# endif
#endif

/** Timer states */
enum cl_timer_state {
    CL_TIMER_ST_CREATED,
    CL_TIMER_ST_REGISTERED,
    CL_TIMER_ST_INSTALLED,
    CL_TIMER_ST_WAITING,
    CL_TIMER_ST_RUNNING,
    CL_TIMER_ST_TERMINATING,
    CL_TIMER_ST_FINALIZED,

    CL_TIMER_MAX_STATE
};

/** Timer informations */
enum cl_timer_info_field {
    CL_TIMER_INFO_NAME = 0,
    CL_TIMER_INFO_INTERVAL,
    CL_TIMER_INFO_OVERRUN,
    CL_TIMER_INFO_FINISH_TIMEOUT,
    CL_TIMER_INFO_IMODE,
    CL_TIMER_INFO_IMODE_DESC,
    CL_TIMER_INFO_DATA,            /* XXX: Cannot be freed */
    CL_TIMER_MAX_INFO,
};

/** Timer execution check modes */
enum cl_timer_interval_mode {
    CL_TIMER_IMODE_DEFAULT,
    CL_TIMER_IMODE_DISCOUNT_RUNTIME
};

/**
 * @name cl_timer_set_state
 * @brief Sets the actual state of a atimer.
 *
 * This function must be called inside the timer function.
 *
 * @param [in] arg: Argument received by the timer function.
 * @param [in] state: The new timer state.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_set_state(cl_timer_arg_t arg, enum cl_timer_state state);

/**
 * @name cl_timer_get_timer
 * @brief Gets a pointer to a specfic cl_timer_t object.
 *
 * Gets a pointer to a specific timer from the \a timers_list using
 * its name \a timer_name as a search key.
 *
 * @param [in] timers_list: Registered timers list.
 * @param [in] timer_name: Timer identification name.
 *
 * @return On success returns a cl_timer_t object pointing to the timer
 *         or NULL otherwise.
 */
cl_timer_t *cl_timer_get_timer(const cl_timer_t *timers_list,
                               const char *timer_name);

/**
 * @name cl_timer_update_interval
 * @brief Updates the execution interval of a timer.
 *
 * @param [in] timer: The cl_timer_t object from the timer.
 * @param [in] interval: New execution interval (in seconds).
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_update_interval(cl_timer_t *timer, unsigned int interval);

/**
 * @name cl_timer_unload_info
 * @brief Releases memory used by a timer info object.
 *
 * @param [in] timer_info: The cl_timer_info_t object from a timer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_unload_info(cl_timer_info_t *timer_info);

/**
 * @name cl_timer_load_info
 * @brief Gets informations about a specific timer.
 *
 * @param [in] timer: The cl_timer_t object from the timer.
 *
 * @return On success returns a cl_timer_info_t object from the timer or
 *         NULL otherwise.
 */
cl_timer_info_t *cl_timer_load_info(const cl_timer_t *timer);

/**
 * @name cl_timer_load_info_within_timer
 * @brief Gets informations about a specific timer.
 *
 * This function must be called inside the timer function.
 *
 * @param [in] arg: Argument received by the timer function.
 *
 * @return On success returns a cl_timer_info_t object from the timer or
 *         NULL otherwise.
 */
cl_timer_info_t *cl_timer_load_info_within_timer(cl_timer_arg_t arg);

/**
 * @name cl_timer_get_info_data
 * @brief Gets a specific information from a cl_timer_info_t object.
 *
 * @param [in] timer_info: The cl_timer_info_t object.
 * @param [in] info: The required information.
 *
 * @return On success gets a pointer to the required information or
 *         NULL otherwise.
 */
void *cl_timer_get_info_data(const cl_timer_info_t *timer_info,
                             enum cl_timer_info_field info);

/**
 * @name cl_timer_register
 * @brief Creates a function as a timer function.
 *
 * Creates a mechanism, using posix timers, to register a function to be called
 * within a process over a given interval. The argument \a timers_list must be
 * NULL in its fisrt call, in such a way as to enable the creation of a
 * registered timers list so that the library can handle it properly.
 *
 * The argument \a timer_name must indicate a unique name for each timer so
 * that is possible to identify it. It is recommended the utilization of simple
 * names, and if compounds, joined by underline characters or hyphen.
 *
 * The argument \a init_function should point to a function in order to
 * initialize any type of data used by the timer function. It should receive
 * as a argument a pointer to the same argument passed to the timer function.
 *
 * The argument \a uninit_function should point to a function in order to
 * release any type of data used by the timer function. Like the initialization
 * function, it must receive as argument a pointer to the same argument passed
 * to the timer function.
 *
 * Both initialization and closing functions should return 0 on success or -1
 * otherwise. If they are unnecessary should be passed as NULL.
 *
 * The timer function, pointed by \a timer_function, is responsible for set
 * two states, internally, through 'cl_timer_set_state' function, to maintain
 * the correct internal logic during library treatments. They are:
 *
 * CL_TIMER_ST_RUNNING - As soon as the function enters.
 * CL_TIMER_ST_WAITING - Right before ending the function.
 *
 * This timer function should not have infinite loops. The system will be in
 * charge of running it again, from time to time.
 *
 * The argument \a imode should report the execution check mode between each
 * timer function call. In the case of CL_TIMER_IMODE_DEFAULT the next call will
 * wait all execution interval \a exec_interval, right after CL_TIMER_ST_WAITING
 * state. The CL_TIMER_IMODE_DISCOUNT_RUNTINME discounts the execution time spent
 * by the timer function on the next call.
 *
 * It is important to remember that regardless of the execution check mode,
 * there will never be simultaneous executions of the same timer function.
 *
 * @param [in,out] timers_list: Registered timers list.
 * @param [in] exec_interval: Execution interval of the timer (in seconds).
 * @param [in] imode: Timer execution check mode.
 * @param [in] finish_timeout: Timeout (in milliseconds) to wait for this
 *                             timer to end.
 * @param [in] timer_name: Timer identification name.
 * @param [in] timer_function: The real timer function.
 * @param [in] arg: Argument passed to the timer function.
 * @param [in] init_function: Custom initialization timer function.
 * @param [in] uninit_function: Custom closing timer function.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_register(cl_timer_t *timers_list, unsigned int exec_interval,
                      enum cl_timer_interval_mode imode,
                      unsigned int finish_timeout,
                      const char *timer_name,
                      void (*timer_function)(cl_timer_arg_t),
                      void *arg, int (*init_function)(void *),
                      int (*uninit_function)(void *));

/**
 * @name cl_timer_unregister
 * @brief Removes a timer from the current execution timers list.
 *
 * Removes a timer from the current execution timers list by searching it
 * using \a timer_name as a key.
 *
 * @param [in] timers_list: Registered timers list.
 * @param [in] timer_name: Timer identification name.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_unregister(cl_timer_t *timers_list, const char *timer_name);

/**
 * @name cl_timer_install
 * @brief Puts all registered timers to run.
 *
 * @param [in] timers_list: Registered timers list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_install(cl_timer_t *timer_list);

/**
 * @name cl_timer_uninstall
 * @brief Ends all registered timers.
 *
 * @param [in] timers_list: Registered timers list.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_uninstall(cl_timer_t *timer_list);

/**
 * @name cl_timer_disarm
 * @brief Suspends the execution of a specific timer.
 *
 * The user should pay attention to not call this function over the registered
 * timers list but over a cl_timer_t object returned from the cl_timer_get_timer
 * function.
 *
 * @param [in] timer: The cl_timer_t object from the timer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_disarm(cl_timer_t *timer);

/**
 * @name cl_timer_arm
 * @brief Restores the execution of a specific timer.
 *
 * The user should pay attention to not call this function over the registered
 * timers list but over a cl_timer_t object returned from the cl_timer_get_timer
 * function.
 *
 * @param [in] timer: The cl_timer_t object from the timer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_timer_arm(cl_timer_t *timer);

#endif

