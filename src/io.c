
/*
 * Description: Standard input handle functions.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "collections.h"

#define ESC_KEY                 0x1B

/* Special keys */
static char *extended_keys[] = {
    "\x1b[A",               // key up
    "\x1b[B",               // key down
    "\x1b[C",               // key right
    "\x1b[D",               // key left
    "\x1b\x5b\x5b\x41",     // F1
    "\x1b\x5b\x5b\x42",     // F2
    "\x1b\x5b\x5b\x43",     // F3
    "\x1b\x5b\x5b\x44",     // F4
    "\x1b\x5b\x5b\x45",     // F5
    "\x1b\x5b\x31\x37\x7e", // F6
    "\x1b\x5b\x31\x38\x7e", // F7
    "\x1b\x5b\x31\x39\x7e", // F8
    "\x1b\x5b\x32\x30\x7e", // F9
    "\x1b\x5b\x32\x31\x7e", // F10
    "\x1b\x5b\x32\x33\x7e", // F11
    "\x1b\x5b\x32\x34\x7e", // F12
    "\x1b\x5b\x36\x7e",     // page down
    "\x1b\x5b\x35\x7e",     // page up
    "\x1b\x5b\x48",         // home
    "\x1b\x5b\x32\x7e",     // insert
    "\x1b\x5b\x46",         //end
    "\x1b\x5b\x33\x7e",     // delete
    "\x7f",                 // backspace
    NULL
};

/* SSH special keys */
static char *extended_keys_xterm[] = {
    "",
    "",
    "",
    "",
    "\x1b\x4f\x50", // F1
    "\x1b\x4f\x51", // F2
    "\x1b\x4f\x52", // F3
    "\x1b\x4f\x53", // F4
    "\x1b\x5b\x31\x35\x7e", // F5
    NULL
};

cstring_t LIBEXPORT *cstdin_getline(void)
{
    char *p;
    cstring_t *s;

    cerrno_clear();

    if (library_initialized() == false)
        return NULL;

    p = cfreadline(stdin);

    if (NULL == p)
        return NULL;

    s = cstring_create("%s", p);
    free(p);

    return s;
}

static int __stdin_timeout(void)
{
    struct timeval tv;
    fd_set rdfs;
    int n;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (!isatty(STDIN_FILENO))
        return -1;

    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);

    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    n = select(1, &rdfs, NULL, NULL, &tv);

    if (n < 0)
        return -1;

    return n;
}

bool LIBEXPORT cstdin_select(void)
{
    return (__stdin_timeout() <= 0 ? false : true);
}

static int __search_ext_key(char *k)
{
    int i, p=-1;
    char *term=NULL;

    term = getenv("TERM");

    for (i = 0; extended_keys[i] != NULL; i++)
        if (!strcmp(k, extended_keys[i]))
            p = i;

    if ((p == -1) && (term != NULL) && strcmp(term, "linux"))
        for (i = 0; extended_keys_xterm[i] != NULL; i++)
            if (!strcmp(k, extended_keys_xterm[i]))
                p = i;

    return p;
}

int LIBEXPORT cgetkey(bool block)
{
    unsigned char ch;
    char keys[32];
    int n, i, retval = 1000, e_key;
    struct termios oattr, attr;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (tcgetattr(STDIN_FILENO, &oattr) < 0) {
        cset_errno(CL_TTY_GET_ATTR);
        return -1;
    }

    attr = oattr;
    attr.c_lflag &= ~(ICANON | ECHO);
    attr.c_cc[VMIN] = 1;
    attr.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH,&attr) < 0) {
        cset_errno(CL_TTY_SET_ATTR_TCSAFLUSH);
        return -1;
    }

    if (block == false) {
        if ((n = __stdin_timeout()) < 0) {
            retval = -3;
            goto end_block;
        }

        if (!n)
            retval = CL_NO_KEY;
    }

    if (retval == 1000) {
        for (i = 0, n = 1; n; ++i) {
            if (read(STDIN_FILENO, &ch, 1) != 1) {
                retval = -4;
                goto end_block;
            }

            keys[i] = ch;

            if ((n = __stdin_timeout()) < 0) {
                retval = -5;
                goto end_block;
            }
        }

        keys[i] = 0;

        if (i == 1 || (i > 1 && keys[0] != ESC_KEY))
            retval = keys[0];
        else {
            /* special keys */
            e_key = __search_ext_key(keys);
            retval = e_key;

            if (e_key >= 21)
                retval = (e_key - 21) + 508;
            else if (e_key >= 16)
                retval = (e_key - 16) + 500;
            else if (e_key >= 4)
                retval = (e_key - 4) + 400;
            else
                retval += 300;
        }
    }

end_block:
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oattr) < 0) {
        cset_errno(CL_TTY_SET_ATTR_TCSAFLUSH);
        return -1;
    }

    return retval;
}

int LIBEXPORT cdisable_echo(void)
{
    struct termios attr;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (!isatty(STDIN_FILENO)) {
        cset_errno(CL_NO_TTY);
        return -1;
    }

    if (tcgetattr(STDIN_FILENO, &attr) < 0) {
        cset_errno(CL_TTY_GET_ATTR);
        return -1;
    }

    attr.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr) < 0) {
        cset_errno(CL_TTY_SET_ATTR_TCSAFLUSH);
        return -1;
    }

    return 0;
}

int LIBEXPORT cenable_echo(void)
{
    struct termios attr;

    cerrno_clear();

    if (library_initialized() == false)
        return -1;

    if (!isatty(STDIN_FILENO)) {
        cset_errno(CL_NO_TTY);
        return -1;
    }

    if (tcgetattr(STDIN_FILENO, &attr) < 0) {
        cset_errno(CL_TTY_GET_ATTR);
        return -1;
    }

    attr.c_lflag |= ECHO;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr) < 0) {
        cset_errno(CL_TTY_SET_ATTR_TCSAFLUSH);
        return -1;
    }

    return 0;
}

