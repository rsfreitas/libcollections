
/*
 * Description: TCP IPC socket functions.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Mar  9 22:56:04 2015
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
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "collections.h"
#include "chat.h"

#define LISTEN_BACKLOG                  30

struct tcp_data_s {
    int             fd;
    char            *ip;
    unsigned int    port;
};

static struct tcp_data_s *new_tcp_data_s(void)
{
    struct tcp_data_s *d = NULL;

    d = calloc(1, sizeof(struct tcp_data_s));

    if (NULL == d) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    d->ip = NULL;

    return d;
}

static void destroy_tcp_data_s(struct tcp_data_s *d)
{
    if (NULL == d)
        return;

    if (d->ip != NULL)
        free(d->ip);

    free(d);
}

ipc_data_t *tcp_init(void)
{
    struct tcp_data_s *d = NULL;
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
        return NULL;

    d = new_tcp_data_s();

    if (NULL == d)
        return NULL;

    d->fd = fd;

    return d;

}

int tcp_uninit(ipc_data_t *ipc_data)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;

    destroy_tcp_data_s(d);

    return 0;
}

int tcp_set_up(ipc_data_t *ipc_data, enum chat_mode mode, va_list ap)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;
    struct sockaddr_in sc_in;

    if (mode == CHAT_SERVER) {
        /* Receives server mode parameters */
        d->port = va_arg(ap, int);

        /* bind */
        sc_in.sin_family = AF_INET;
        sc_in.sin_port = htons(d->port);
        sc_in.sin_addr.s_addr = INADDR_ANY;

        memset(sc_in.sin_zero, 0, 8);

        if (bind(d->fd, (struct sockaddr *)&sc_in,
                 sizeof(struct sockaddr)) == -1)
        {
            return -1;
        }

        /* listen */
        if (listen(d->fd, LISTEN_BACKLOG) < 0)
            return -1;
    } else {
        /* Receives client mode parameters */
        d->port = va_arg(ap, int);
        d->ip = strdup(va_arg(ap, char *));
    }

    return 0;
}

int tcp_connect(ipc_data_t *ipc_data)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;
    struct sockaddr_in sc_in;

    sc_in.sin_family = AF_INET;
    sc_in.sin_port = htons(d->port);
    sc_in.sin_addr.s_addr = inet_addr(d->ip);

    return connect(d->fd, (struct sockaddr *)&sc_in, sizeof(struct sockaddr));
}

ipc_data_t *tcp_accept(ipc_data_t *ipc_data, unsigned int accept_timeout)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;
    struct tcp_data_s *client_d = NULL;
    struct sockaddr_in sc_in;
    int cfd;
    socklen_t len;

    /* Awaits the data becomes available */
    if (accept_timeout > 0)
        if (has_data_to_receive(d->fd, accept_timeout) == false)
            return NULL;

    /* accept */
    len = sizeof(struct sockaddr_in);
    cfd = accept(d->fd, (struct sockaddr *)&sc_in, &len);

    if (cfd < 0) {
        cset_errno(CL_ACCEPT_FAILED);
        return NULL;
    }

    client_d = new_tcp_data_s();

    if (NULL == client_d)
        return NULL;

    client_d->fd = cfd;

    return client_d;
}

int tcp_send(ipc_data_t *ipc_data, struct chat_data_s *dt_send)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;

    if (NULL == dt_send)
        return -1;

    return send(d->fd, dt_send->data, dt_send->data_size, 0);
}

struct chat_data_s *tcp_recv(ipc_data_t *ipc_data, unsigned int recv_timeout)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;
    struct chat_data_s *cd = NULL;
    int ret;

    /* Awaits the data becomes available */
    if (recv_timeout > 0)
        if (has_data_to_receive(d->fd, recv_timeout) == false)
            return NULL;

    cd = new_chat_data_s(DEFAULT_RECV_DATA_SIZE);

    if (NULL == cd)
        return NULL;

    ret = recv(d->fd, cd->data, cd->data_size, 0);

    if (ret == -1) {
        cset_errno(CL_RECV_FAILED);
        destroy_chat_data_s(cd);
        return NULL;
    } else if (ret > 0)
        cd->data_size = ret;

    return cd;
}

int tcp_stop(ipc_data_t *ipc_data)
{
    struct tcp_data_s *d = (struct tcp_data_s *)ipc_data;

    return close(d->fd);
}

