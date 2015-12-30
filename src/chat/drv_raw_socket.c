
/*
 * Description: Internal API for 'raw socket' protocol.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 10 11:02:53 2015
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

#include <string.h>

#include "collections.h"
#include "chat.h"

/*
 * Makes the insertion of raw data from arguments @data (and @data_size) in
 * a 'struct chat_data_s' structure, for sending it through an internal IPC
 * protocol.
 */
struct chat_data_s
    *raw_socket_prepare_to_send(drv_data_t *drv_data __attribute__((unused)),
                void *data, unsigned int data_size)
{
    struct chat_data_s *scd = NULL;

    scd = new_chat_data_s(data_size);

    if (NULL == scd)
        return NULL;

    memcpy(scd->data, data, data_size);

    return scd;
}

/*
 * Breaks an information received from an IPC protocol in a 'struct chat_data_s'
 * structure (raw data).
 */
void *raw_socket_process_recv_data(drv_data_t *drv_data __attribute__((unused)),
    struct chat_data_s *scd, unsigned int *data_size)
{
    void *d;

    /*
     * Makes a copy of the current data, because @scd will be freed before
     * the function returns to the user.
     */
    *data_size = scd->data_size;
    d = cmemdup(scd->data, scd->data_size);

    return d;
}

