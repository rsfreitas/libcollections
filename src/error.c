
/*
 * Description: Library internal error handling.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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
#include <pthread.h>

#include "collections.h"

static pthread_key_t cerrno_key;
static pthread_once_t cerrno_once = PTHREAD_ONCE_INIT;

static const char *__cdescriptions[] = {
    "Ok",
    "Invalid argument",
    "Invalid string index",
    "No internal memory available",
    "Element not found",
    "Underflow/overflow on number conversion",
    "Not valid a number",
    "Parse error",
    "Not a valid tty",
    "Error calling tcgetattr function",
    "Error calling tcsetattr function",
    "Error opening file",
    "Unrecognized configuration line",
    "Fork failed",
    "Invalid state",
    "Creation failed",
    "Event conditions wrongly initialized",
    "Unsupported type",
    "NULL data encountered",
    "Call ended by timeout",
    "External init function failed",
    "External uninit function failed",
    "Settime failed",
    "Pselect failed",
    "Accept failed",
    "Recv failed",
    "Send failed",
    "Connect failed",
    "IPC init failed",
    "IPC uninit failed",
    "IPC set up failed",
    "IPC driver init failed",
    "IPC driver uninit failed",
    "Error when trying to establish connection",
    "Connection received with errors",
    "Cannot prepare data to send",
    "Not able to process received data",
    "Wrong type",
    "Set value failed",
    "Invalid value",
    "File not found",
    "Plugin information loading error",
    "Plugin information uninit error",
    "Function not found",
    "Function symbol not found",
    "Plugin informations not found",
    "Plugin information symbole not found",
    "Plugin startup error",
    "Plugin shutdown error"
};

static const char *__cunknown_error = "Unknown error";

static void cerrno_free(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
}

static void cerrno_init(void)
{
    pthread_key_create(&cerrno_key, cerrno_free);
}

/**
 * @name cerrno_storage
 * @brief Gets a pointer to the global thread specific error variable.
 *
 * @return Returns a pointer to the global error variable.
 */
cerrno LIBEXPORT *cerrno_storage(void)
{
    cerrno *error = NULL;

    pthread_once(&cerrno_once, cerrno_init);
    error = pthread_getspecific(cerrno_key);

    if (!error) {
        error = malloc(sizeof(*error));
        pthread_setspecific(cerrno_key, error);
    }

    return error;
}

/**
 * @name cexit
 * @brief Terminate calling thread.
 *
 * This function must called at the end of main function if the user wants no
 * memory leak errors reported by the valgrind tool.
 */
void LIBEXPORT cexit(void)
{
    pthread_exit(NULL);
}

#define __cerrno      (*cerrno_storage())

void cerrno_clear(void)
{
    __cerrno = CL_NO_ERROR;
}

void cset_errno(enum cerror_code error_code)
{
    __cerrno = error_code;
}

/*
 * Gets the last error code internally occurred.
 */
enum cerror_code LIBEXPORT cget_last_error(void)
{
    return __cerrno;
}

/*
 * Converts a numeric error code in a text message.
 */
const char LIBEXPORT *cstrerror(enum cerror_code error_code)
{
    if (error_code >= CL_MAX_ERROR_CODE)
        return __cunknown_error;

    return __cdescriptions[error_code];
}

