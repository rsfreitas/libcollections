
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

#ifndef _COLLECTIONS_CL_THREAD_H
#define _COLLECTIONS_CL_THREAD_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_thread.h> directly; include <collections.h> instead."
# endif
#endif

/** Types of threads */
enum cl_thread_type {
    CL_THREAD_JOINABLE,
    CL_THREAD_DETACHED
};

/** Possible thread states */
enum cl_thread_state {
    CL_THREAD_ST_CREATED,
    CL_THREAD_ST_INITIALIZED,
    CL_THREAD_ST_INIT_ERROR,

    CL_THREAD_ST_MAX_STATE
};

/**
 * @name cl_thread_spawn
 * @brief Creates a new thread.
 *
 * This function creates a new thread to execute the \a start_routine function.
 *
 * @param [in] type: The thread type.
 * @param [in] start_routine: The function which will be executed.
 * @param [in] user_data: Some custom data for thread function.
 *
 * @return On success returns a cl_thread_t object or NULL otherwise.
 */
cl_thread_t *cl_thread_spawn(enum cl_thread_type type,
                             void *(*start_routine)(cl_thread_t *),
                             void *user_data);

/**
 * @name cl_thread_destroy
 * @brief Shutdown a thread.
 *
 * Besides shutdown the thread this function will release all memory allocated
 * for it.
 *
 * @param [in] t: The cl_thread_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_thread_destroy(cl_thread_t *t);

/**
 * @name cl_thread_wait_startup
 * @brief Awaits a thread creation.
 *
 * @param [in] t: The cl_thread_t object.
 *
 * @return Returns 0 if the thread has been successfully initialized, 1 if it
 *         has been initialized with errors or -1 otherwise.
 */
int cl_thread_wait_startup(const cl_thread_t *t);

/**
 * @name cthred_set_state
 * @brief Sets the internal state of a thread.
 *
 * @param [in] t: The cl_thread_t object.
 * @param [in] state: The new state of a thread.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_thread_set_state(cl_thread_t *t, enum cl_thread_state state);

/**
 * @name cl_thread_get_user_data
 * @brief Gets the user data passed to a thread.
 *
 * @param [in] arg: The cl_thread_t object passed as argument to a thread.
 *
 * @return Returns the original user data argument of the thread function or
 *         NULL otherwise.
 */
void *cl_thread_get_user_data(cl_thread_t *arg);

/**
 * @name cl_thread_force_finish
 * @brief Forces a thread to finish itself.
 *
 * @param [in] t: The cl_thread_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_thread_force_finish(cl_thread_t *t);

#endif

