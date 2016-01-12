
/*
 * Description: UDP IPC socket functions.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Mar  9 22:56:07 2015
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
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "collections.h"
#include "chat.h"

struct udp_data_s {
    int                 fd;
    unsigned int        port;
    struct sockaddr_in  sc_in;
};

static struct udp_data_s *new_udp_data_s(void)
{
    struct udp_data_s *d = NULL;

    d = calloc(1, sizeof(struct udp_data_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    return d;
}

static void destroy_udp_data_s(struct udp_data_s *d)
{
    if (NULL == d)
        return;

    free(d);
}

ipc_data_t *udp_init(void)
{
    struct udp_data_s *d = NULL;
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd == -1)
        return NULL;

    d = new_udp_data_s();

    if (NULL == d)
        return NULL;

    d->fd = fd;

    return d;
}

int udp_uninit(ipc_data_t *ipc_data)
{
    struct udp_data_s *d = (struct udp_data_s *)ipc_data;

    destroy_udp_data_s(d);

    return 0;
}

int udp_set_up(ipc_data_t *ipc_data, enum chat_mode mode, va_list ap)
{
    struct udp_data_s *d = (struct udp_data_s *)ipc_data;
    struct sockaddr_in sc_in;

    /* Receives server mode parameters */
    d->port = va_arg(ap, int);

    memset(&sc_in, 0, sizeof(sc_in));
    sc_in.sin_family = AF_INET;
    sc_in.sin_port = htons(d->port);

    if (mode == CHAT_SERVER) {
        sc_in.sin_addr.s_addr = htonl(INADDR_ANY);

        /* bind */
        if (bind(d->fd, (struct sockaddr *)&sc_in, sizeof(sc_in)) == -1)
            return -1;
    } else {
        inet_aton(va_arg(ap, char *), &sc_in.sin_addr);
        memcpy(&d->sc_in, &sc_in, sizeof(struct sockaddr_in));
    }

    return 0;
}

int udp_send(ipc_data_t *ipc_data, struct chat_data_s *dt_send)
{
    struct udp_data_s *d = (struct udp_data_s *)ipc_data;

    if (NULL == dt_send)
        return -1;

    return sendto(d->fd, dt_send->data, dt_send->data_size, 0,
                  (struct sockaddr *)&d->sc_in, sizeof(d->sc_in));
}

struct chat_data_s *udp_recv(ipc_data_t *ipc_data, unsigned int recv_timeout)
{
    struct udp_data_s *d = (struct udp_data_s *)ipc_data;
    struct chat_data_s *cd = NULL;
    socklen_t size = sizeof(struct sockaddr_in);
    int ret;

    /* Awaits the data becomes available */
    if (recv_timeout > 0)
        if (has_data_to_receive(d->fd, recv_timeout) == CL_FALSE)
            return NULL;

    cd = new_chat_data_s(DEFAULT_RECV_DATA_SIZE);

    if (NULL == cd)
        return NULL;

    ret = recvfrom(d->fd, cd->data, cd->data_size, 0,
                   (struct sockaddr *)&d->sc_in, &size);

    if (ret < 0) {
        cset_errno(CL_RECV_FAILED);
        destroy_chat_data_s(cd);
        return NULL;
    } else if (ret > 0)
        cd->data_size = ret;

    return cd;
}

int udp_stop(ipc_data_t *ipc_data)
{
    struct udp_data_s *d = (struct udp_data_s *)ipc_data;

    return close(d->fd);
}

