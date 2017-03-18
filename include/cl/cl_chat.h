
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Dec 10 14:40:57 2015
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

#ifndef _COLLECTIONS_CL_CHAT_H
#define _COLLECTIONS_CL_CHAT_H     1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_chat.h> directly; include <collections.h> instead."
# endif
#endif

/** Working modes */
enum cl_chat_mode {
    CL_CHAT_CLIENT,
    CL_CHAT_SERVER
};

/** Protocols */
enum cl_chat_driver {
    CL_CHAT_DRV_RAW_TCP,
    CL_CHAT_DRV_RAW_UDP
};

/**
 * @name cl_chat_create
 * @brief Initialize IPC environment for client/server communication.
 *
 * @param [in] cd: Protocol type.
 * @param [in] mode: Working mode.
 * @param [in] sigpipe_block: Boolean flag informing if SIGPIPE will be internally
 *                            blocked or not.
 *
 * @return On success returns a cl_chat_t object or NULL otherwise.
 */
cl_chat_t *cl_chat_create(enum cl_chat_driver cd, enum cl_chat_mode mode,
                          bool sigpipe_block);

/**
 * @name cl_chat_destroy
 * @brief Ends IPC environment for client/server communication.
 *
 * @param [in] chat: The cl_chat_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_destroy(cl_chat_t *chat);

/**
 * @name cl_chat_set_info
 * @brief Set up required informations before making the client/server
 *        connection.
 *
 * The variadic arguments changes according \a chat work mode. If it is
 * CHAT_SERVER must inform the port (an int). If it is CHAT_CLIENT must inform
 * port number (an int) and the IP (a char *).
 *
 * @param [in] chat: The cl_chat_t object.
 * @param [in] ...: Informations that will be assigned to the object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_set_info(cl_chat_t *chat, ...);

/**
 * @name cl_chat_client_start
 * @brief Try to establish a connection with a server.
 *
 * The server IP address must have been previously configured with a
 * call to 'cl_chat_set_info' function.
 *
 * @param [in] chat: The client cl_chat_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_client_start(cl_chat_t *chat);

/**
 * @name cl_chat_server_start
 * @brief Puts a server side to wait for connections.
 *
 * If the argument \a accept_timeout is 0 this function will block until a
 * connection is made. Otherwise, it will be used as a timeout to wait for
 * new connections.
 *
 * @param [in] chat: The server cl_chat_t object.
 * @param [in] accept_timeout: Timeout to wait for new connecions (in seconds).
 *
 * @return On success returns a new 'cl_chat_t' object pointing to the established
 *         connection or NULL otherwise.
 */
cl_chat_t *cl_chat_server_start(cl_chat_t *chat, unsigned int accept_timeout);

/**
 * @name cl_chat_send
 * @brief Sends data to another connected node.
 *
 * @param [in] chat: The cl_chat_t object.
 * @param [in] data: Pointer to the data which will be sent.
 * @param [in] data_size: Size of this data.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_send(cl_chat_t *chat, void *data, unsigned int data_size);

/**
 * @name cl_chat_recv
 * @brief Gets data from another connected node.
 *
 * If the argument \a recv_timeout is 0 this function will block until some
 * data is available to read. Otherwise, it will be used as a timeout to
 * wait for new data.
 *
 * @param [in] chat: The cl_chat_t object.
 * @param [in] recv_timeout: Timeout to wait for new data (in seconds).
 * @param [out] data_size: Size of the received data.
 *
 * @return On success returns a 'void *' with the received data (that should
 *         be freed by the user) or NULL otherwise.
 */
void *cl_chat_recv(cl_chat_t *chat, unsigned int recv_timeout,
                   unsigned int *data_size);

/**
 * @name cl_chat_fd
 * @brief Gets the IPC file descriptor.
 *
 * Creates a new reference for \a chat, so one must unref after using it.
 *
 * @param [in] chat: The cl_chat_t object.
 *
 * @return On success returns the IPC file descriptor or NULL otherwise.
 */
int cl_chat_fd(cl_chat_t *chat);

/**
 * @name cl_chat_stop
 * @brief Closes a client/server communication.
 *
 * @param [in] chat: The cl_chat_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_stop(cl_chat_t *chat);

/**
 * @name cl_chat_ref
 * @brief Increases the reference count for a cl_chat_t item.
 *
 * @param [in,out] chat: The cl_chat_t object.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_chat_t *cl_chat_ref(cl_chat_t *chat);

/**
 * @name cl_chat_unref
 * @brief Decreases the reference count for a cl_chat_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] chat: The cl_chat_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_chat_unref(cl_chat_t *chat);

#endif

