
/*
 * Description: Example showing the 'chat' API from libcollections from the
 *              client side.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Jan 11 09:31:32 2016
 * Project: examples
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
#include <signal.h>

#include <limits.h>

#include <collections.h>

static bool __finish = false;

static void help(void)
{
    fprintf(stdout, "Usage: client [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections chat "
                    "API from the client side.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -t\t\t\tTCP mode (default).\n");
    fprintf(stdout, "  -u\t\t\tUDP mode.\n");
    fprintf(stdout, "  -p [port]\t\tServer port number.\n");
    fprintf(stdout, "  -i [IP]\t\tServer IP.\n");
    fprintf(stdout, "\n");
}

void signal_handler(int signum)
{
    switch (signum) {
        case SIGINT:
            __finish = true;
            break;
    }
}

int main(int argc, char **argv)
{
    const char *opt = "htup:i:\0";
    int option, port = 0, text_length;
    bool tcp = true;
    char *ip = NULL, *text = NULL, i = 0;
    cl_chat_t *c = NULL;
    struct sigaction sa_int;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 't':
                tcp = true;
                break;

            case 'u':
                tcp = false;
                break;

            case 'p':
                port = atoi(optarg);
                break;

            case 'i':
                ip = strdup(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if ((NULL == ip) || (port == 0)) {
        fprintf(stderr, "Error: The server IP and port must be informed.\n");
        return -1;
    }

    cl_init(NULL);
    memset(&sa_int, 0, sizeof(struct sigaction));
    sa_int.sa_handler = signal_handler;
    sigaction(SIGINT, &sa_int, NULL);

    c = cl_chat_create((tcp == true) ? CL_CHAT_DRV_RAW_TCP : CL_CHAT_DRV_RAW_UDP,
                    CL_CHAT_CLIENT, true);

    if (NULL == c) {
        fprintf(stderr, "Error (1): %s.\n", cl_strerror(cl_get_last_error()));
        return -1;
    }

    if (cl_chat_set_info(c, port, ip, NULL) < 0) {
        fprintf(stderr, "Error (2): %s.\n", cl_strerror(cl_get_last_error()));
        goto end_block;
    }

    if (cl_chat_client_start(c) < 0) {
        fprintf(stderr, "Error (3): %s.\n", cl_strerror(cl_get_last_error()));
        goto end_block;
    }

    while (__finish == false) {
        cl_msleep(1000);
        text_length = asprintf(&text, "An example... simple... just to test %03d", i);
        i++;

        if (cl_chat_send(c, text, text_length + 1) < 0)
            fprintf(stderr, "Error (4): %s.\n", cl_strerror(cl_get_last_error()));
        else
            fprintf(stdout, "sent...\n");

        free(text);

        if (i >= CHAR_MAX)
            i = 0;
    }

end_block:
    cl_chat_destroy(c);

    if (ip != NULL)
        free(ip);

    cl_uninit();
    cl_exit();

    return 0;
}

