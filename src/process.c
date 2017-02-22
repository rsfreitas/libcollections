
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
#include <ctype.h>

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

__PUB_API__ void cprcs_daemon_start(void)
{
    __clib_function_init_ex2__(false, NULL, -1);

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

static char **cvt_cmd(const char *cmd)
{
    cstring_t *s = NULL, *ref;
    cstring_list_t *l = NULL;
    char **app_argv;
    unsigned int size, i;

    s = cstring_create("%s", cmd);
    l = cstring_split(s, " ");
    cstring_destroy(s);
    size = cstring_list_size(l);
    app_argv = calloc(size + 1, sizeof(char *));

    for (i = 0; i < size; i++) {
        ref = cstring_list_get(l, i);
        app_argv[i] = strdup(cstring_valueof(ref));
        cstring_unref(ref);
    }

    app_argv[size] = NULL;
    cstring_list_destroy(l);

    return app_argv;
}

/*
 * TODO: Needs to handle a pipe between commands.
 */
__PUB_API__ int csystem(bool close_parent_files, const char *fmt, ...)
{
    pid_t p;
    int child_status, i;
    char *cmd = NULL, **argv;
    va_list ap;

    __clib_function_init__(false, NULL, -1, -1);
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

__PUB_API__ void cmsleep(long mseconds)
{
    struct timespec tv;

    __clib_function_init_ex2__(false, NULL, -1);

    tv.tv_sec = (time_t)mseconds / 1000;
    tv.tv_nsec = (long)(mseconds % 1000) * 1000000;

    nanosleep(&tv,&tv);
}

__PUB_API__ int ctrap(int signum, void (*f)(int))
{
    struct sigaction s;

    /*
     * TODO: Save oldact for user somehow.
     *       Validate signumber.
     */

    __clib_function_init__(false, NULL, -1, -1);

    memset(&s, 0, sizeof(struct sigaction));
    s.sa_handler = f;

    if (sigaction(signum, &s, NULL) != 0)
        return -1;

    return 0;
}

/*
 * Gets the user-specific non-essential runtime directory.
 */
static char *get_xdg_dir(void)
{
    char *path = NULL, *env;

    env = getenv("XDG_RUNTIME_DIR");

    if (NULL == env)
        asprintf(&path, "/run/user/%d", getuid());
    else
        path = strdup(env);

    return path;
}

/*
 * Writes a file into a user directory with the current application as its name
 * and the PID as its content.
 */
static int write_instance_file(void)
{
    char *path, *pathname;
    FILE *f;

    path = get_xdg_dir();

    if (NULL == path)
        return -1;

    asprintf(&pathname, "%s/%s.pid", path, library_package_name());
    free(path);
    f = fopen(pathname, "w+");
    free(pathname);

    if (NULL == f)
        return -1;

    fprintf(f, "%d", getpid());
    fclose(f);

    return 0;
}

/*
 * Reads a PID from an application instance file.
 */
static pid_t read_instance_file(const char *name)
{
    char *path, *pathname;
    FILE *f;
    pid_t pid;

    path = get_xdg_dir();

    if (NULL == path)
        return 0;

    asprintf(&pathname, "%s/%s.pid", path, name);
    free(path);
    f = fopen(pathname, "r");
    free(pathname);

    if (NULL == f)
        return 0;

    fscanf(f, "%d", &pid);
    fclose(f);

    return pid;
}

static char *instance_name(pid_t pid)
{
    char *tmp = NULL, *cmd = NULL, *bname = NULL, *app_name = NULL;
    FILE *f;
    size_t size = 0;
    int offset = 0;

    asprintf(&tmp, "/proc/%d/cmdline", pid);
    f = fopen(tmp, "r");
    free(tmp);

    if (NULL == f)
        return NULL;

    if (getdelim(&tmp, &size, 0, f) == -1) {
        fclose(f);
        return NULL;
    }

    fclose(f);
    cmd = tmp;

    while (!(isalpha(*cmd)))
        offset++, cmd++;

    cmd = strdup(tmp + offset);
    bname = basename(cmd);
    free(tmp);
    app_name = strdup(bname);
    free(cmd);

    return app_name;
}

__PUB_API__ bool cinstance_active(const char *name)
{
    pid_t pid;
    char *we = NULL;
    bool b;

    __clib_function_init__(false, NULL, -1, -1);

    if (NULL == name)
        name = library_package_name();

    pid = read_instance_file(name);

    /* Instance file not found */
    if (pid == 0)
        return false;

    /* Are we really who we think we are? */
    we = instance_name(pid);

    if (NULL == we)
        return false;

    b = (strcmp(name, we) == 0) ? true : false;
    free(we);

    return b;
}

__PUB_API__ int cset_instance_as_active(void)
{
    __clib_function_init__(false, NULL, -1, -1);

    if (write_instance_file() < 0)
        return -1;

    return 0;
}

