
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
enum cthread_type {
    CL_THREAD_JOINABLE,
    CL_THREAD_DETACHED
};

/** Possible thread states */
enum cthread_state {
    CL_THREAD_ST_CREATED,
    CL_THREAD_ST_INITIALIZED,
    CL_THREAD_ST_INIT_ERROR,

    CL_THREAD_ST_MAX_STATE
};

/**
 * @name cthread_spawn
 * @brief Creates a new thread.
 *
 * This function creates a new thread to execute the \a start_routine function.
 * It uses 'pthread' functions inside.
 *
 * @param [in] type: The thread type.
 * @param [in] start_routine: The function which will be executed.
 * @param [in] user_data: Some custom data for thread function.
 *
 * @return On success returns a cthread_t object or NULL otherwise.
 */
cthread_t *cthread_spawn(enum cthread_type type,
                         void *(*start_routine)(cthread_t *), void *user_data);

/**
 * @name cthread_destroy
 * @brief Shutdown a thread.
 *
 * Besides shutdown the thread this function will release all memory allocated
 * for it.
 *
 * @param [in] t: The cthread_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cthread_destroy(cthread_t *t);

/**
 * @name cthread_wait_startup
 * @brief Awaits a thread creation.
 *
 * @param [in] t: The cthread_t object.
 *
 * @return Returns 0 if the thread has been successfully initialized, 1 if it
 *         has been initialized with errors or -1 otherwise.
 */
int cthread_wait_startup(const cthread_t *t);

/**
 * @name cthred_set_state
 * @brief Sets the internal state of a thread.
 *
 * @param [in] t: The cthread_t object.
 * @param [in] state: The new state of a thread.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cthread_set_state(cthread_t *t, enum cthread_state state);

/**
 * @name cthread_get_user_data
 * @brief Gets the user data passed to a thread.
 *
 * @param [in] arg: The cthread_t object passed as argument to a thread.
 *
 * @return Returns the original user data argument of the thread function or
 *         NULL otherwise.
 */
void *cthread_get_user_data(cthread_t *arg);

#endif

