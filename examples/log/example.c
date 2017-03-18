
/*
 * Description: Example showing the 'log' API from libcolelctions.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon May 30 13:37:27 2016
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

#include <collections.h>

static void help(void)
{
    fprintf(stdout, "Usage: log [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "log file API.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -f [file name]\tIndicate the log file to handle.\n");

    fprintf(stdout, "\n");
}

static void write_binary_data(cl_log_t *log)
{
    void *p;
    int psize = 255;

    p = calloc(psize, sizeof(char));

    if (NULL == p)
        return;

    cl_log_set_log_level(log, CL_LOG_DEBUG);
    cl_log_bprint(log, CL_LOG_DEBUG, p, psize);
    free(p);
}

int main(int argc, char **argv)
{
    const char *opt = "f:h";
    char *filename = NULL;
    int option, ret;
    cl_log_t *log;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'f':
                filename = strdup(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == filename) {
        fprintf(stderr, "There must be a valid file name.\n");
        return -1;
    }

    cl_init(NULL);
    log = cl_log_open(filename, CL_LOG_SYNC_ALL_MSGS, CL_LOG_INFO, 30);

    if (NULL == log) {
        printf("%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));
        return -1;
    }

    ret = cl_log_printf(log, CL_LOG_ERROR, "An error message");

    if (ret < 0)
        printf("%s: %s\n", __FUNCTION__, cl_strerror(cl_get_last_error()));

    cl_log_printf(log, CL_LOG_INFO, "An information message");
    write_binary_data(log);

    cl_log_close(log);

    if (filename != NULL)
        free(filename);

    cl_uninit();
    cl_exit();

    return 0;
}

