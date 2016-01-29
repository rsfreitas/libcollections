
/*
 * Description: Functions to handle process, like daemons.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
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

#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "collections.h"

static int get_maxfd(void)
{
    int maxfd = 0;

#ifdef HAVE_SYSCONF
    maxfd = sysconf(_SC_OPEN_MAX);
#else
# ifdef _POSIX_OPEN_MAX
    maxfd = _POSIX_OPEN_MAX;
# else
    maxfd = 1024;
# endif
#endif

    return maxfd;
}

static void close_all_opened_files(void)
{
    int maxfd, fd;

    maxfd = get_maxfd();

    for (fd = 0; fd < maxfd; fd++)
        close(fd);
}

void LIBEXPORT cprcs_daemon_start(void)
{
    if (fork())
        exit(0);

    setsid();

    if (fork())
        exit(0);

    chdir("/");
    umask(0);
    close_all_opened_files();

    open("/dev/null", 0);
    dup(0);
    dup(0);
}

static char *__fixed_args[] = {
    "/bin/sh",
    "-c"
};

#define FIXED_ARGS              \
    (sizeof(__fixed_args) / sizeof(__fixed_args[0]))

static char **cvt_cmd(const char *cmd)
{
    cstring_t *s = NULL, *ref;
    cstring_list_t *l = NULL;
    char **app_argv;
    unsigned int size, i;

    s = cstring_new("%s", cmd);
    l = cstring_split(s, " ");
    cstring_free(s);
    size = cstring_list_size(l);
    app_argv = calloc(size + FIXED_ARGS + 1, sizeof(char *));

    for (i = 0; i < FIXED_ARGS; i++)
        app_argv[i] = strdup(__fixed_args[i]);

    for (i = 0; i < size; i++) {
        ref = cstring_list_get(l, i);
        app_argv[FIXED_ARGS + i] = strdup(cstring_valueof(ref));
        cstring_unref(ref);
    }

    app_argv[size + FIXED_ARGS + 1] = NULL;
    cstring_list_free(l);

    return app_argv;
}

int LIBEXPORT csystem(bool close_parent_files, const char *fmt, ...)
{
    pid_t p;
    int child_status, i;
    char *cmd = NULL, **argv;
    va_list ap;

    cerrno_clear();
    p = fork();

    if (p < 0) {
        cset_errno(CL_FORK_FAILED);
        return -1;
    } else if (p == 0) {
        if (close_parent_files == true)
            close_all_opened_files();

        va_start(ap, fmt);
        vasprintf(&cmd, fmt, ap);
        va_end(ap);

        argv = cvt_cmd(cmd);
        free(cmd);

        execvp(argv[0], argv);

        for (i = 0; argv[i] != NULL; i++)
            free(argv[i]);

        free(argv);
        _exit(0);
    } else {
        waitpid(p, &child_status, WUNTRACED);
        return child_status;
    }

    return 0;
}

void LIBEXPORT cmsleep(long mseconds)
{
    struct timespec tv;

    tv.tv_sec = (time_t)mseconds / 1000;
    tv.tv_nsec = (long)(mseconds % 1000) * 1000000;

    nanosleep(&tv,&tv);
}

int LIBEXPORT ctrap(int signum, void (*f)(int))
{
    struct sigaction s;

    /*
     * TODO: Save oldact for user somehow.
     *       Validate signumber.
     */

    memset(&s, 0, sizeof(struct sigaction));
    s.sa_handler = f;

    if (sigaction(signum, &s, NULL) != 0)
        return -1;

    return 0;
}

