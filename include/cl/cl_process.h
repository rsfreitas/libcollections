
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
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

#ifndef _COLLECTIONS_CL_PROCESS_H
#define _COLLECTIONS_CL_PROCESS_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_process.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _SIGNAL_H
# include <signal.h>
#endif

/**
 * @name cl_daemon_start
 * @brief Makes the actual process run in daemon mode (background).
 */
void cl_daemon_start(void);

/**
 * @name cl_system
 * @brief Execute a shell command.
 *
 * @param [in] close_parent_files: Boolean flag to indicate the need to close
 *                                 any open files of the parent process.
 * @param [in] fmt: The command.
 * @param [in] ...: The values passed to the command.
 *
 * @return On success returns the returned value from the executed command or
 *         -1 otherwise.
 */
int cl_system(bool close_parent_files, const char *fmt, ...)
              __attribute__((format(printf, 2, 3)));

/**
 * @name cl_msleep
 * @brief Suspend execution for milliseconds interval.
 *
 * @param [in] mseconds: Milliseconds interval.
 */
void cl_msleep(long mseconds);

/**
 * @name cl_trap
 * @brief Trap signals.
 *
 * Defines a function to be called when an application receives a specific
 * signal.
 *
 * @param [in] signum: The signal number.
 * @param [in] f: The function which will be called when a signal occurs.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_trap(int signum, void (*f)(int));

/**
 * @name cl_instance_active
 * @brief Checks if an instance is running in the system.
 *
 * This function checks if an application, started by the same user with the
 * same mechanism, is active in the system by using a PID, read from a file
 * with its \a name. The application name is check it with with a name read
 * from the file /proc/PID/cmdline.
 *
 * If \a name is NULL we check ourselves.
 *
 * In most cases \a filename and \a instance_name will be the same.
 *
 * @param [in] filename: The instance file name, without extension.
 * @param [in] instance_name: The instance name.
 *
 * @return In case the instance is active returns true otherwise returns false.
 */
bool cl_instance_active(const char *filename, const char *instance_name);

/**
 * @name cl_set_instance_as_active
 * @brief Sets the current instance as active.
 *
 * Sets the current instance as active writing a file with the application name
 * containing its PID.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_set_instance_as_active(void);

#endif

