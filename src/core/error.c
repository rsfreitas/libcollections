
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

static const char *__cdescriptions[] = {
    cl_tr_noop("Ok"),
    cl_tr_noop("Invalid argument"),
    cl_tr_noop("Invalid string index"),
    cl_tr_noop("No internal memory available"),
    cl_tr_noop("Object not found"),
    cl_tr_noop("Underflow/overflow on number conversion"),
    cl_tr_noop("Not valid a number"),
    cl_tr_noop("Parse error"),
    cl_tr_noop("Not a valid tty"),
    cl_tr_noop("Error calling tcgetattr function"),
    cl_tr_noop("Error calling tcsetattr function"),
    cl_tr_noop("Error opening file"),
    cl_tr_noop("Unrecognized configuration line"),
    cl_tr_noop("Fork failed"),
    cl_tr_noop("Invalid state"),
    cl_tr_noop("Creation failed"),
    cl_tr_noop("Event conditions wrongly initialized"),
    cl_tr_noop("Unsupported type"),
    cl_tr_noop("NULL data encountered"),
    cl_tr_noop("Call ended by timeout"),
    cl_tr_noop("External init function failed"),
    cl_tr_noop("External uninit function failed"),
    cl_tr_noop("Settime failed"),
    cl_tr_noop("Pselect failed"),
    cl_tr_noop("Accept failed"),
    cl_tr_noop("Recv failed"),
    cl_tr_noop("Send failed"),
    cl_tr_noop("Connect failed"),
    cl_tr_noop("IPC init failed"),
    cl_tr_noop("IPC uninit failed"),
    cl_tr_noop("IPC set up failed"),
    cl_tr_noop("IPC driver init failed"),
    cl_tr_noop("IPC driver uninit failed"),
    cl_tr_noop("Error when trying to establish connection"),
    cl_tr_noop("Connection received with errors"),
    cl_tr_noop("Cannot prepare data to send"),
    cl_tr_noop("Not able to process received data"),
    cl_tr_noop("Wrong type"),
    cl_tr_noop("Set value failed"),
    cl_tr_noop("Invalid value"),
    cl_tr_noop("File not found"),
    cl_tr_noop("Failed to load external module"),
    cl_tr_noop("Failed to unload external module"),
    cl_tr_noop("Value not found"),
    cl_tr_noop("Plugin information not found"),
    cl_tr_noop("Plugin startup error"),
    cl_tr_noop("Plugin shutdown error"),
    cl_tr_noop("Failed to load python plugin"),
    cl_tr_noop("Failed to get python object dictionary"),
    cl_tr_noop("Data convertion failed"),
    cl_tr_noop("The library was not initialized"),
    cl_tr_noop("Failed creating directory"),
    cl_tr_noop("A collision inside the hashtable just happened"),
    cl_tr_noop("Unsupported RAW image"),
    cl_tr_noop("Unable to load image"),
    cl_tr_noop("Unable to create temporary internal image"),
    cl_tr_noop("Invalid read file size")
};

static const char *__cunknown_error = cl_tr_noop("Unknown error");

/* Our error storage */
struct error_storage {
    int error;
};

/*
 *
 * Internal API
 *
 */

cl_error_storage_declare(__storage__, sizeof(struct error_storage))
#define __cerrno        (cl_errno_storage(&__storage__))

/* TODO: Rename this */
CL_INTERNAL_API
void cerrno_clear(void)
{
    struct error_storage *e = __cerrno;

    e->error = CL_NO_ERROR;
}

/* TODO: Rename this */
CL_INTERNAL_API
void cset_errno(enum cl_error_code error_code)
{
    struct error_storage *e = __cerrno;

    e->error = error_code;
}

/*
 *
 * API
 *
 */

/**
 * @name cl_errno_storage
 * @brief Gets a pointer to the global thread specific error variable.
 *
 * @param [in,out] storage: The previously declared error storage.
 *
 * @return Returns a pointer to the global error variable.
 */
cl_errno *cl_errno_storage(struct cl_error_storage *storage)
{
    cl_errno *error = NULL;

    /*
     * XXX: We don't check library initialization here since a call to this
     *      function is usually done in global variables.
     *
     *      Another important note is that one must declare the @storage
     *      argument using the library cl_error_storage_declare macro.
     *      Otherwise the init_routine to pthread_once won't exist.
     */

    pthread_once(&storage->once, storage->init_routine);
    error = pthread_getspecific(storage->key);

    if (!error) {
        error = calloc(1, storage->storage_size);
        pthread_setspecific(storage->key, error);
    }

    return error;
}

/*
 * Gets the last error code internally occurred.
 */
enum cl_error_code cl_get_last_error(void)
{
    struct error_storage *e = __cerrno;

    return e->error;
}

/*
 * Converts a numeric error code in a text message.
 */
const char *cl_strerror(enum cl_error_code error_code)
{
    if (error_code >= CL_MAX_ERROR_CODE)
        return __cunknown_error;

    return __cdescriptions[error_code];
}

