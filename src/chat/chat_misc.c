
/*
 * Description: Common functions for chat functions internal API.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Mar  9 22:54:40 2015
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
#include <signal.h>
#include <string.h>

#include "collections.h"
#include "chat.h"

struct chat_data_s *new_chat_data_s(unsigned int data_size)
{
    struct chat_data_s *d = NULL;

    d = calloc(1, sizeof(struct chat_data_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    d->data = calloc(1, (data_size + 1) * sizeof(char));

    if (NULL == d->data) {
        cset_errno(CL_NO_MEM);
        free(d);
        return NULL;
    }

    d->data_size = data_size;

    return d;
}

void destroy_chat_data_s(struct chat_data_s *data)
{
    if (data->data != NULL)
        free(data->data);

    free(data);
}

/*
 * Checks if there are any information available on a file descriptor.
 *
 * If don't, the function will set the internal library error to
 * CL_DATA_TIMEOUT in case the pselect timeout expires or CL_PSELECT_FAILED
 * in case pselect detect any internal error.
 */
cbool_t has_data_to_receive(int fd, int seconds)
{
    struct timespec ts;
    int ndfs = 0, ret;
    fd_set rdfs;
    sigset_t tmask;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);
    sigemptyset(&tmask);

    ndfs = (ndfs > fd) ? ndfs : fd;
    ts.tv_sec = seconds;
    ts.tv_nsec = 0;

    ret = pselect(ndfs + 1, &rdfs, NULL, NULL, &ts, &tmask);

    if (ret > 0)
        return CL_TRUE; /* Has data available to be read */
    else if (ret == -1)
        cset_errno(CL_PSELECT_FAILED);
    else
        cset_errno(CL_ENDED_WITH_TIMEOUT);

    /* Error or timeout */
    return CL_FALSE;
}

