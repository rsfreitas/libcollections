
/*
 * Description: Internal chat declarations for libcollections.
 *
 * Author: Rodrigo Freitas
 * Date: Thu Dec 10 14:38:55 2015
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

#ifndef _COLLECTIONS_CHAT_H
#define _COLLECTIONS_CHAT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <chat.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _SIGNAL_H
# include <signal.h>
#endif

#ifndef _STDARG_H
# include <stdarg.h>
#endif

/* Internal chat types */
typedef void                    drv_data_t;
typedef void                    ipc_data_t;

/* Default size of a received package */
#define DEFAULT_RECV_DATA_SIZE  1024

/* Tipos de IPC suportados */
enum chat_ipc_protocol {
    CHAT_IPC_TCP,
    CHAT_IPC_UDP
};

/* Stores data to be sent or received */
struct chat_data_s {
    void            *data;
    unsigned int    data_size;
};

/* Methods of an IPC manipulation */
struct chat_ipc_methods_s {
    enum chat_ipc_protocol  ipc_type;

    /* Mandatory IPC manipulation methods */
    ipc_data_t              *(*init)(void);
    int                     (*uninit)(ipc_data_t *);
    int                     (*set_up)(ipc_data_t *, enum chat_mode, va_list);
    int                     (*stop)(ipc_data_t *);
    int                     (*send)(ipc_data_t *, struct chat_data_s *);
    struct chat_data_s      *(*recv)(ipc_data_t *, unsigned int);
    int                     (*fd)(ipc_data_t *);

    /* Optional IPC manipulation methods */
    int                     (*connect)(ipc_data_t *);
    ipc_data_t              *(*accept)(ipc_data_t *, unsigned int);
};

/* Protocol informations */
struct chat_driver_info_s {
    /* Configurations */
    enum chat_driver        driver_id;
    enum chat_ipc_protocol  ipc_type;

    /* Driver events and dealing functions */
    drv_data_t              *(*init)(ipc_data_t *);
    int                     (*uninit)(drv_data_t *);
    int                     (*established_connection)(drv_data_t *);
    int                     (*connection_received)(drv_data_t *);
    int                     (*connection_ended)(drv_data_t *);
    struct chat_data_s      *(*prepare_to_send)(drv_data_t *, void *,
                                                unsigned int);

    void                    *(*process_recv_data)(drv_data_t *,
                                                  struct chat_data_s *,
                                                  unsigned int *);
};

struct chat_s {
    /* Configurations */
    enum chat_mode              mode;
    enum chat_driver            driver_id;
    struct chat_driver_info_s   *driver_info;
    struct chat_ipc_methods_s   *ipc_methods;
    bool                        sigpipe;
    struct sigaction            sa_pipe;

    /* IPC internal informations */
    ipc_data_t                  *ipc_data;

    /* Internal Protocol informations */
    drv_data_t                  *drv_data;
};

/* chat_misc.c */
struct chat_data_s *new_chat_data_s(unsigned int data_size);
void destroy_chat_data_s(struct chat_data_s *data);
bool has_data_to_receive(int fd, int seconds);

/** Communication driver */
/* drv_raw_socket.c */
struct chat_data_s *raw_socket_prepare_to_send(drv_data_t *drv_data, void *data,
                                               unsigned int data_size);

void *raw_socket_process_recv_data(drv_data_t *drv_data,
                                   struct chat_data_s *scd,
                                   unsigned int *data_size);

/** IPC drivers */
/* TCP */
ipc_data_t *tcp_init(void);
int tcp_uninit(ipc_data_t *ipc_data);
int tcp_set_up(ipc_data_t *ipc_data, enum chat_mode, va_list ap);
int tcp_connect(ipc_data_t *ipc_data);
ipc_data_t *tcp_accept(ipc_data_t *ipc_data, unsigned int accept_timeout);
int tcp_send(ipc_data_t *ipc_data, struct chat_data_s *dt_send);
struct chat_data_s *tcp_recv(ipc_data_t *ipc_data, unsigned int recv_timeout);
int tcp_stop(ipc_data_t *ipc_data);
int tcp_fd(ipc_data_t *ipc_data);

/* UDP */
ipc_data_t *udp_init(void);
int udp_uninit(ipc_data_t *ipc_data);
int udp_set_up(ipc_data_t *ipc_data, enum chat_mode, va_list ap);
int udp_connect(ipc_data_t *ipc_data);
ipc_data_t *udp_accept(ipc_data_t *ipc_data, unsigned int accept_timeout);
int udp_send(ipc_data_t *ipc_data, struct chat_data_s *dt_send);
struct chat_data_s *udp_recv(ipc_data_t *ipc_data, unsigned int recv_timeout);
int udp_stop(ipc_data_t *ipc_data);
int udp_fd(ipc_data_t *ipc_data);

#endif

