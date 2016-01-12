
/*
 * Description: Example showing the 'chat' API from libcollections from the
 *              server side.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Jan 11 09:31:32 2016
 * Project: examples
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

#include <collections.h>

static cbool_t __finish = CL_FALSE;

static void help(void)
{
    fprintf(stdout, "Usage: server [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections chat "
                    "API from the server side.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -t\t\t\tTCP mode (default).\n");
    fprintf(stdout, "  -u\t\t\tUDP mode.\n");
    fprintf(stdout, "  -p [port]\t\tServer port number.\n");
    fprintf(stdout, "\n");
}

void signal_handler(int signum)
{
    switch (signum) {
        case SIGINT:
            __finish = CL_TRUE;
            break;
    }
}

int main(int argc, char **argv)
{
    const char *opt = "htup:\0";
    int option, port = 0;
    cbool_t tcp = CL_TRUE;
    chat_t *c = NULL, *n = NULL;
    char *ptr = NULL;
    unsigned int length = 0;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 't':
                tcp = CL_TRUE;
                break;

            case 'u':
                tcp = CL_FALSE;
                break;

            case 'p':
                port = atoi(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if ((port == 0)) {
        fprintf(stderr, "Error: The server port must be informed.\n");
        return -1;
    }

    c = chat_new((tcp == CL_TRUE) ? CHAT_DRV_RAW_TCP : CHAT_DRV_RAW_UDP,
                 CHAT_SERVER, CL_TRUE);

    if (NULL == c) {
        fprintf(stderr, "Error (1): %s.\n", cstrerror(cget_last_error()));
        return -1;
    }

    if (chat_set_info(c, port, NULL) < 0) {
        fprintf(stderr, "Error (2): %s.\n", cstrerror(cget_last_error()));
        goto end_block;
    }

    if (tcp == CL_TRUE)
        n = chat_server_start(c, 0);
    else
        n = c;

    if (NULL == n) {
        fprintf(stderr, "Error (3): %s.\n", cstrerror(cget_last_error()));
        goto end_block;
    }

    fprintf(stdout, "Client connected.\n");

    while (__finish == CL_FALSE) {
        cmsleep(1);
        ptr = (char *)chat_recv(n, 0, &length);

        if (ptr != NULL)
            fprintf(stdout, "Recv: '%s' - %d\n", ptr, length);
    }

end_block:
    if ((n != NULL) && (tcp == CL_TRUE))
        chat_free(n);

    chat_free(c);
    cexit();

    return 0;
}

