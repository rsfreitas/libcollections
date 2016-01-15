
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
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

#ifndef _COLLECTIONS_CL_PROCESS_H
#define _COLLECTIONS_CL_PROCESS_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_process.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cprcs_daemon_start
 * @brief Makes the actual process run in daemon mode (background).
 */
void cprcs_deamon_start(void);

/**
 * @name csystem
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
int csystem(bool close_parent_files, const char *fmt, ...);

/**
 * @name cmsleep
 * @brief Suspend execution for milliseconds interval.
 *
 * @param [in] mseconds: Milliseconds interval.
 */
void cmsleep(long mseconds);

/**
 * @name ctrap
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
int ctrap(int signum, void (*f)(int));

#endif

