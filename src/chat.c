
/*
 * Description: API for interprocess communication.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Mar  9 22:56:02 2015
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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "collections.h"
#include "chat/chat.h"

#define chat_s      cl_struct(chat_s)

/*
 * TODO: Save client info on the server side.
 *       Validate object in every exported function.
 */

/* IPCs methods */
static struct chat_ipc_methods_s __ipc_methods[] = {
    /* socket TCP */
    {
        .ipc_type = CHAT_IPC_TCP,
        .init = tcp_init,
        .uninit = tcp_uninit,
        .set_up = tcp_set_up,
        .connect = tcp_connect,
        .accept = tcp_accept,
        .send = tcp_send,
        .recv = tcp_recv,
        .stop = tcp_stop,
        .fd = tcp_fd,
    },

    /* socket UDP */
    {
        .ipc_type = CHAT_IPC_UDP,
        .init = udp_init,
        .uninit = udp_uninit,
        .set_up = udp_set_up,
        .connect = NULL,
        .accept = NULL,
        .send = udp_send,
        .recv = udp_recv,
        .stop = udp_stop,
        .fd = udp_fd,
    }
};

/* Protocols */
static struct chat_driver_info_s __drv_info[] = {
    {
        .driver_id = CHAT_DRV_RAW_TCP,
        .ipc_type = CHAT_IPC_TCP,
        .init = NULL,
        .uninit = NULL,
        .established_connection = NULL,
        .connection_received = NULL,
        .prepare_to_send = raw_socket_prepare_to_send,
        .process_recv_data = raw_socket_process_recv_data,
    },

    {
        .driver_id = CHAT_DRV_RAW_UDP,
        .ipc_type = CHAT_IPC_UDP,
        .init = NULL,
        .uninit = NULL,
        .established_connection = NULL,
        .connection_received = NULL,
        .prepare_to_send = raw_socket_prepare_to_send,
        .process_recv_data = raw_socket_process_recv_data,
    }
};

/*
 *
 * IPC functions
 *
 */

static struct chat_ipc_methods_s
        *get_chat_ipc_methods_s(enum chat_ipc_protocol cim)
{
    unsigned int i = 0, t = 0;

    t = sizeof(__ipc_methods) / sizeof(__ipc_methods[0]);

    for (i = 0; i < t; i++)
        if (__ipc_methods[i].ipc_type == cim)
            return &__ipc_methods[i];

    return NULL;
}

static struct chat_ipc_methods_s
        *get_chat_ipc_methods_s_from_chat_t(chat_s *c)
{
    struct chat_ipc_methods_s *cim;

    cim = c->ipc_methods;

    if (NULL == cim) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    return cim;
}

static ipc_data_t *chat_ipc_init(chat_s *c)
{
    struct chat_ipc_methods_s *cim = NULL;
    ipc_data_t *ipc_data = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return NULL;

    if (NULL == (cim->init)) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    ipc_data = (cim->init)();

    if (NULL == ipc_data) {
        cset_errno(CL_IPC_INIT_FAILED);
        return NULL;
    }

    return ipc_data;
}

static int chat_ipc_uninit(chat_s *c)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->uninit)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if ((cim->uninit)(c->ipc_data) < 0) {
        cset_errno(CL_IPC_UNINIT_FAILED);
        return -1;
    }

    return 0;
}

static int chat_ipc_set_up(chat_s *c, enum chat_mode mode, va_list ap)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->set_up)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if ((cim->set_up)(c->ipc_data, mode, ap) < 0) {
        cset_errno(CL_IPC_SET_UP_FAILED);
        return -1;
    }

    return 0;
}

static int chat_ipc_stop(chat_s *c)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->stop)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return (cim->stop)(c->ipc_data);
}

static int chat_ipc_send(chat_s *c, struct chat_data_s *data)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->send)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    if ((cim->send)(c->ipc_data, data) < 0) {
        cset_errno(CL_SEND_FAILED);
        return -1;
    }

    return 0;
}

static struct chat_data_s *chat_ipc_recv(chat_s *c,
    unsigned int recv_timeout)
{
    struct chat_ipc_methods_s *cim = NULL;
    struct chat_data_s *cd = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return NULL;

    if (NULL == (cim->recv)) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    cd = (cim->recv)(c->ipc_data, recv_timeout);

    if (NULL == cd) {
        cset_errno(CL_RECV_FAILED);
        return NULL;
    }

    return cd;
}

static int chat_ipc_connect(chat_s *c)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->connect))
        return 0; /* Optional method */

    if ((cim->connect)(c->ipc_data) < 0) {
        cset_errno(CL_CONNECT_FAILED);
        return -1;
    }

    return 0;
}

static ipc_data_t *chat_ipc_accept(chat_s *c, unsigned int accept_timeout)
{
    struct chat_ipc_methods_s *cim = NULL;
    ipc_data_t *ipc_data = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return NULL;

    /* Optional method, returns a pointer != NULL */
    if (NULL == (cim->accept))
        return (struct chat_data_s *)c;

    ipc_data = (cim->accept)(c->ipc_data, accept_timeout);

    if (NULL == ipc_data) {
        cset_errno(CL_ACCEPT_FAILED);
        return NULL;
    }

    return ipc_data;
}

static int chat_ipc_fd(chat_s *c)
{
    struct chat_ipc_methods_s *cim = NULL;

    cim = get_chat_ipc_methods_s_from_chat_t(c);

    if (NULL == cim)
        return -1;

    if (NULL == (cim->fd)) {
        cset_errno(CL_NULL_DATA);
        return -1;
    }

    return (cim->fd)(c->ipc_data);
}

/*
 *
 * Protocol driver functions.
 *
 */

static struct chat_driver_info_s *get_chat_driver_info_s(enum chat_driver cd)
{
    unsigned int i = 0, t = 0;

    t = sizeof(__drv_info) / sizeof(__drv_info[0]);

    for (i = 0; i < t; i++)
        if (__drv_info[i].driver_id == cd)
            return &__drv_info[i];

    return NULL;
}

static struct chat_driver_info_s
        *get_chat_driver_info_s_from_chat_t(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;

    cdi = c->driver_info;

    if (NULL == cdi) {
        cset_errno(CL_NULL_DATA);
        return NULL;
    }

    return cdi;
}

static drv_data_t *chat_drv_init(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;
    drv_data_t *drv_data = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return NULL;

    if (NULL == cdi->init)
        return (drv_data_t *)c;

    drv_data = (cdi->init)(c->ipc_data);

    if (NULL == drv_data) {
        cset_errno(CL_DRIVER_INIT_FAILED);
        return NULL;
    }

    return drv_data;
}

static int chat_drv_uninit(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return -1;

    if (NULL == cdi->uninit)
        return 0;

    if ((cdi->uninit)(c->drv_data) < 0) {
        cset_errno(CL_DRIVER_UNINIT_FAILED);
        return -1;
    }

    return 0;
}

static int chat_drv_established_connection(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return -1;

    if (NULL == cdi->established_connection)
        return 0;

    if ((cdi->established_connection)(c->drv_data) < 0) {
        cset_errno(CL_ESTABLISHED_CONNECTION_FAILED);
        return -1;
    }

    return 0;
}

static int chat_drv_connection_received(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return -1;

    if (NULL == cdi->connection_received)
        return 0;

    if ((cdi->connection_received)(c->drv_data) < 0) {
        cset_errno(CL_CONNECTION_RECEIVED_FAILED);
        return -1;
    }

    return 0;
}

static int chat_drv_connection_ended(chat_s *c)
{
    struct chat_driver_info_s *cdi = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return -1;

    return 0;
}

static struct chat_data_s *chat_drv_prepare_to_send(chat_s *c,
    void *data, unsigned int data_size)
{
    struct chat_driver_info_s *cdi = NULL;
    struct chat_data_s *cd = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return NULL;

    if (NULL == cdi->prepare_to_send)
        return (struct chat_data_s *)c;

    cd = (cdi->prepare_to_send)(c->drv_data, data, data_size);

    if (NULL == cd) {
        cset_errno(CL_PREPARE_TO_SEND_FAILED);
        return NULL;
    }

    return cd;
}

static void *chat_drv_process_recv_data(chat_s *c, struct chat_data_s *cd,
    unsigned int *data_size)
{
    struct chat_driver_info_s *cdi = NULL;
    void *data = NULL;

    cdi = get_chat_driver_info_s_from_chat_t(c);

    if (NULL == cdi)
        return NULL;

    data = (cdi->process_recv_data)(c->drv_data, cd, data_size);

    if (NULL == data) {
        cset_errno(CL_PROCESS_RECV_DATA_FAILED);
        return NULL;
    }

    return data;
}

/*
 *
 * Internal common functions
 *
 */

static bool validate_chat_driver(enum chat_driver scd)
{
    if ((scd == CHAT_DRV_RAW_TCP) ||
        (scd == CHAT_DRV_RAW_UDP))
    {
        return true;
    }

    return false;
}

static void blocks_sigpipe(chat_s *c)
{
    struct sigaction sa_pipe;

    memset(&sa_pipe, 0, sizeof(struct sigaction));
    sa_pipe.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa_pipe, &c->sa_pipe);
}

static void restores_sigpipe(chat_s *c)
{
    sigaction(SIGPIPE, &c->sa_pipe, NULL);
}

static void destroy_chat_s(chat_s *c)
{
    free(c);
}

static void __destroy_chat_s(const struct ref_s *ref)
{
    chat_s *c = container_of(ref, chat_s, ref);

    if (NULL == c)
        return;

    if (c->sigpipe == true)
        restores_sigpipe(c);

    /* Call driver closing function */
    if (chat_drv_uninit(c) < 0)
        return;

    /* Call IPC closing function */
    if (chat_ipc_uninit(c) < 0)
        return;

    destroy_chat_s(c);
}

static chat_s *new_chat_s(void)
{
    chat_s *s = NULL;

    s = calloc(1, sizeof(chat_s));

    if (NULL == s) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    set_typeof(CHAT, s);
    s->ref.count = 1;
    s->ref.free = __destroy_chat_s;

    return s;
}

static chat_s *dup_chat_s(chat_s *c)
{
    chat_s *s = NULL;

    s = new_chat_s();

    if (NULL == s)
        return NULL;

    s->driver_id = c->driver_id;
    s->driver_info = c->driver_info;
    s->ipc_methods = c->ipc_methods;
    s->mode = c->mode;

    return s;
}

/*
 *
 * Exported chat API functions.
 *
 */

chat_t LIBEXPORT *chat_create(enum chat_driver cd, enum chat_mode mode,
    bool sigpipe_block)
{
    chat_s *c = NULL;
    struct chat_driver_info_s *cdi;
    struct chat_ipc_methods_s *cim;

    __clib_function_init__(false, NULL, -1, NULL);

    if (validate_chat_driver(cd) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    cdi = get_chat_driver_info_s(cd);

    if (NULL == cdi) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    cim = get_chat_ipc_methods_s(cdi->ipc_type);

    if (NULL == cim) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    c = new_chat_s();

    if (NULL == c)
        return NULL;

    c->driver_id = cd;
    c->driver_info = cdi;
    c->ipc_methods = cim;
    c->mode = mode;
    c->sigpipe = sigpipe_block;

    /* Call IPC initialization function */
    c->ipc_data = chat_ipc_init(c);

    if (NULL == c->ipc_data)
        goto error_block;

    /* Call driver initialization function */
    c->drv_data = chat_drv_init(c);

    if (NULL == c->drv_data)
        goto error_block;

    if (sigpipe_block == true)
        blocks_sigpipe(c);

    return c;

error_block:
    destroy_chat_s(c);
    return NULL;
}

int LIBEXPORT chat_destroy(chat_t *chat)
{
    return chat_unref(chat);
}

int LIBEXPORT chat_set_info(chat_t *chat, ...)
{
    chat_s *c = (chat_s *)chat;
    va_list ap;

    __clib_function_init__(true, chat, CHAT, -1);
    va_start(ap, NULL);

    /* Call function to set up some socket details */
    if (chat_ipc_set_up(c, c->mode, ap) < 0)
        return -1;

    va_end(ap);

    return 0;
}

int LIBEXPORT chat_client_start(chat_t *chat)
{
    chat_s *c = (chat_s *)chat;

    __clib_function_init__(true, chat, CHAT, -1);

    /* Call function to establish connection */
    if (chat_ipc_connect(c) < 0)
        return -1;

    /* Call driver event function after a successfully established connection */
    if (chat_drv_established_connection(c) < 0)
        return -1;

    return 0;
}

chat_t LIBEXPORT *chat_server_start(chat_t *chat, unsigned int accept_timeout)
{
    chat_s *c = (chat_s *)chat;
    chat_s *client_c = NULL;
    ipc_data_t *d = NULL;

    __clib_function_init__(true, chat, CHAT, NULL);

    /* Call function to wait connections */
    d = chat_ipc_accept(c, accept_timeout);

    if (NULL == d)
        return NULL;

    /* Call driver event function after a successfully receveid connection */
    if (chat_drv_connection_received(c) < 0)
        goto error_block;

    /* Set up a new struct for the connected client */
    client_c = dup_chat_s(c);

    if (NULL == client_c)
        goto error_block;

    client_c->mode = CHAT_CLIENT;
    client_c->ipc_data = d;
    client_c->sigpipe = false;
    client_c->drv_data = c->drv_data;

    return client_c;

error_block:
    chat_ipc_uninit(d);
    return NULL;
}

int LIBEXPORT chat_send(chat_t *chat, void *data, unsigned int data_size)
{
    chat_s *c = (chat_s *)chat;
    struct chat_data_s *cd = NULL;
    int ret = 0;

    __clib_function_init__(true, chat, CHAT, -1);

    /* Prepare data to be sent */
    cd = chat_drv_prepare_to_send(c, data, data_size);

    if (NULL == cd)
        return -1;

    /* Call sending function */
    if (chat_ipc_send(c, cd) < 0) {
        ret = -1;
        goto end_block;
    }

    ret = 0;

end_block:
    destroy_chat_data_s(cd);
    return ret;
}

void LIBEXPORT *chat_recv(chat_t *chat, unsigned int recv_timeout,
    unsigned int *data_size)
{
    chat_s *c = (chat_s *)chat;
    struct chat_data_s *cd = NULL;
    void *data = NULL;

    __clib_function_init__(true, chat, CHAT, NULL);

    /* Call receive function */
    cd = chat_ipc_recv(c, recv_timeout);

    if (NULL == cd)
        return NULL;

    /* Handle received data */
    data = chat_drv_process_recv_data(c, cd, data_size);

    if (NULL == data)
        goto end_block;

end_block:
    destroy_chat_data_s(cd);
    return data;
}

int LIBEXPORT chat_stop(chat_t *chat)
{
    __clib_function_init__(true, chat, CHAT, -1);

    /* Call IPC stop function */
    if (chat_ipc_stop(chat) < 0)
        return -1;

    /* And call driver event function after a connection closed */
    if (chat_drv_connection_ended(chat) < 0)
        return -1;

    return 0;
}

int LIBEXPORT chat_fd(chat_t *chat)
{
    __clib_function_init__(true, chat, CHAT, -1);
    chat_ref(chat);

    return chat_ipc_fd(chat);
}

chat_t LIBEXPORT *chat_ref(chat_t *chat)
{
    chat_s *c = (chat_s *)chat;

    __clib_function_init__(true, chat, CHAT, NULL);
    ref_inc(&c->ref);

    return chat;
}

int LIBEXPORT chat_unref(chat_t *chat)
{
    chat_s *c = (chat_s *)chat;

    __clib_function_init__(true, chat, CHAT, -1);
    ref_dec(&c->ref);

    return 0;
}

