
/*
 * Description: API to handle strings using a cstring_t object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
 * Project: libcollections
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

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "collections.h"

struct cstring_s {
    uint32_t    size;
    char        *str;
};

static struct cstring_s *cstring_new_empty(void)
{
    struct cstring_s *p = NULL;

    p = calloc(1, sizeof(struct cstring_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->str = NULL;

    return p;
}

/*
 * Creates a new cstring_t object.
 */
cstring_t LIBEXPORT *cstring_new(const char *fmt, ...)
{
    struct cstring_s *s = NULL;
    va_list ap;

    cerrno_clear();
    s = cstring_new_empty();

    if (NULL == s)
        return NULL;

    if (NULL == fmt)
        return s;

    va_start(ap, fmt);
    s->size = vasprintf(&s->str, fmt, ap);
    va_end(ap);

    return s;
}

cstring_t LIBEXPORT *cstring_new_ex(unsigned int size)
{
    struct cstring_s *s = NULL;

    cerrno_clear();

    if (size == 0) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    s = cstring_new_empty();

    if (NULL == s)
        return NULL;

    s->str = calloc(size, sizeof(char));

    if (NULL == s->str) {
        cset_errno(CL_NO_MEM);
        cstring_destroy(s);
        return NULL;
    }

    return s;
}

/*
 * Creates a cstring_t object containing random letters.
 */
cstring_t LIBEXPORT *cstring_random(unsigned int size)
{
    struct cstring_s *p = NULL;
    unsigned int i;
    int n;

    cerrno_clear();
    p = cstring_new_empty();

    if (NULL == p)
        return NULL;

    for (i = 0; i < size; i++) {
        n = (int)(26.0 * (rand() / (RAND_MAX + 1.0)));
        n += 'a';
        cstring_cat(p, "%c", (char)n);
    }

    return p;
}

/*
 * Frees a cstring_t object from memory.
 */
int LIBEXPORT cstring_destroy(cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (p->str != NULL)
        free(p->str);

    free(p);
    p = NULL;

    return 0;
}

/*
 * Gets the length of a cstring_t object.
 */
int LIBEXPORT cstring_length(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return p->size;
}

/*
 * Gets the value of a cstring_t object.
 */
const char LIBEXPORT *cstring_valueof(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return p->str;
}

/*
 * Returns the char value at the specified index.
 */
char LIBEXPORT cstring_at(const cstring_t *s, unsigned int index)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (index >= (unsigned int)cstring_length(p)) {
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    return p->str[index];
}

/*
 * Changes the value of a specified index to a new value.
 */
int LIBEXPORT cstring_set(cstring_t *s, char c, unsigned int index)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (index >= (unsigned int)cstring_length(p)) {
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    p->str[index] = c;

    return 0;
}

/*
 * Concatenate two strings.
 */
int LIBEXPORT cstring_cat(cstring_t *s, const char *fmt, ...)
{
    struct cstring_s *p = (struct cstring_s *)s;
    va_list ap;
    char *buff = NULL;
    int l=0;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    l = vasprintf(&buff, fmt, ap);
    va_end(ap);

    if (NULL == p->str) {
        p->str = buff;
        p->size = l;
        return 0;
    }

    p->str = realloc(p->str, p->size + l + 1);
    memcpy(&p->str[p->size], buff, l);
    p->size += l;
    p->str[p->size] = '\0';

    if (buff != NULL)
        free(buff);

    return 0;
}

/*
 * Compare two cstring_t objects.
 */
int LIBEXPORT cstring_cmp(const cstring_t *s1, const cstring_t *s2)
{
    struct cstring_s *p1 = (struct cstring_s *)s1;
    struct cstring_s *p2 = (struct cstring_s *)s2;

    cerrno_clear();

    if ((NULL == s1) || (NULL == s2)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return strcmp(p1->str, p2->str);
}

/*
 * Compare two cstring_t objects.
 */
int LIBEXPORT cstring_ncmp(const cstring_t *s1, const cstring_t *s2, size_t n)
{
    struct cstring_s *p1 = (struct cstring_s *)s1;
    struct cstring_s *p2 = (struct cstring_s *)s2;

    cerrno_clear();

    if ((NULL == s1) || (NULL == s2)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return strncmp(p1->str, p2->str, n);
}

cstring_t LIBEXPORT *cstring_dup(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cstring_new("%s", p->str);
}

cstring_t LIBEXPORT *cstring_upper(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;
    unsigned int i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);

    if (NULL == o)
        return NULL;

    for (i = 0; i < o->size; i++)
        if (isascii(o->str[i]))
            o->str[i] = toupper(o->str[i]);

    return o;
}

cstring_t LIBEXPORT *cstring_lower(const const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;
    unsigned int i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);

    if (NULL == o)
        return NULL;

    for (i = 0; i < o->size; i++)
        if (isascii(o->str[i]))
            o->str[i] = tolower(o->str[i]);

    return o;
}

cstring_t LIBEXPORT *cstring_capitalize(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);

    if (NULL == o)
        return NULL;

    o->str[0] = toupper(o->str[0]);

    return o;
}

int LIBEXPORT cstring_find(const cstring_t *s, char c)
{
    struct cstring_s *p = (struct cstring_s *)s;
    int i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c)
            return i;

    return -1; /* character not found */
}

int LIBEXPORT cstring_rfind(const cstring_t *s, char c)
{
    struct cstring_s *p = (struct cstring_s *)s;
    int i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = cstring_length(p) - 1; i >= 0; i--)
        if (p->str[i] == c)
            return i;

    return -1; /* character not found */
}

int LIBEXPORT cstring_cchr(const cstring_t *s, char c)
{
    struct cstring_s *p = (struct cstring_s *)s;
    int i, match = 0;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c)
            match++;

    return match;
}

cstring_t LIBEXPORT *cstring_ltrim(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;
    int size, i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);

    if (NULL == o)
        return NULL;

    size = cstring_length(o);

    while (o->str[0] == ' ') {
        for (i = 0; i < size; i++)
            o->str[i] = o->str[i + 1];

        size--;
        o->str[size] = '\0';
    }

    o->size = size;

    return o;
}

cstring_t LIBEXPORT *cstring_rtrim(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;
    int size, i;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);

    if (NULL == o)
        return NULL;

    size = cstring_length(o);
    i = size - 1;

    while ((i >= 0) && (o->str[i] == ' ')) {
        o->str[i] = '\0';
        i--;
        size--;
    }

    o->size = size;

    return o;
}

cstring_t LIBEXPORT *cstring_alltrim(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_ltrim(p);

    if (NULL == o)
        return NULL;

    p = cstring_rtrim(o);
    cstring_destroy(o);

    if (NULL == p)
        return NULL;

    return p;
}

cstring_t LIBEXPORT *cstring_substr(const cstring_t *s, const char *needle)
{
    struct cstring_s *p = (struct cstring_s *)s;
    struct cstring_s *o = NULL;
    char *ptr = NULL;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ptr = strstr(p->str, needle);

    if (NULL == ptr)
        return NULL;

    o = cstring_new("%s", ptr);

    if (NULL == o)
        return NULL;

    return o;
}

int LIBEXPORT cstring_rplchr(cstring_t *s, char c1, char c2)
{
    struct cstring_s *p = (struct cstring_s *)s;
    int i, c = 0, index;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    c = cstring_cchr(p, c1);

    if (c <= 0)
        return 0;

    for (i = 0; i < c; i++) {
        index = cstring_find(s, c1);

        if (index < 0)
            break;

        cstring_set(s, c2, index);
    }

    return 0;
}

int LIBEXPORT cstring_rplsubstr(cstring_t *s, const char *old, const char *new_)
{
    struct cstring_s *p = (struct cstring_s *)s;
    size_t l_old, l_new, l;
    char *s_in1, *s_in2, *s_out, *n;
    int n_old = 0;

    cerrno_clear();

    if ((NULL == s) || (NULL == old) || (NULL == new_)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    l_old = strlen(old);
    l_new = strlen(new_);
    s_in1 = p->str;

    /* counts the number of @old occurrences */
    if (l_old) {
        while ((s_in1 = strstr(s_in1, old))) {
            n_old++;
            s_in1 += l_old;
        }
    }

    l = p->size - n_old * l_old + n_old * l_new + 1;
    n = calloc(l, sizeof(char));

    if (NULL == n) {
        cset_errno(CL_NO_MEM);
        return -1;
    }

    s_in1 = p->str;
    s_in2 = p->str;
    s_out = n;

    if (l_old) {
        while ((s_in1 = strstr(s_in1, old))) {
            strncpy(s_out, s_in2, s_in1 - s_in2);
            s_out += s_in1 - s_in2;

            strcpy(s_out, new_);
            s_out += l_new;

            s_in1 += l_old;
            s_in2 = s_in1;
        }
    }

    strcat(s_out, s_in2);

    /* Replace on cstring_t object */
    free(p->str);
    p->str = n;
    p->size = l;

    return 0;
}

/*
 * Checks if a cstring_t object has any valid data.
 */
cbool_t LIBEXPORT cstring_isempty(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return CL_FALSE;
    }

    return (cstring_length(p) > 0) ? CL_TRUE : CL_FALSE;
}

/*
 * Clears the content of a cstring_t object.
 */
int LIBEXPORT cstring_clear(cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (p->str != NULL) {
        free(p->str);
        p->str = NULL;
    }

    p->size = 0;

    return 0;
}

/*
 * Checks if @delim is a valid delimiter character.
 */
static char is_delimiter_char(char c, const char *delim)
{
    while (*delim)
        if (c == *delim++)
            return c;

    return 0;
}

/*
 * Gets a substring of @s until the first token is found.
 */
static char *token_strcpy(const char *s, char token)
{
    int i=0;
    char *tmp, *p;

    tmp = strdup(s);

    while (*s) {
        if (*s++ == token) {
            p = strndup(tmp, i);
            free(tmp);
            return p;
        } else
            i++;
    }

    free(tmp);

    return NULL;
}

/*
 * Extracts substrings around matches of given tokens.  It separates the
 * original string from a series of tokens. Instead of strtok C function,
 * this one does not changes the original string.
 *
 * @param [in] s: Original string.
 * @param [in] delim: String containing token list of chars.
 * @param [out] next_s: Pointer to store the remaining string.
 *
 * @return Returns NULL when no substring is found or a new substring that
 *         must be freed by the user.
 */
static char *__strtok(const char *s, const char *delim, char **next_s)
{
    char *str, c_delim, *st, *sc;

    if (s)
        st = (char *)s;
    else if (*next_s && (strlen(*next_s) > 0))
        st = *next_s;
    else {
        /* End of susbtrings */
        return NULL;
    }

    sc = strdup(st);

    while (*st) {
        c_delim = is_delimiter_char(*st++, delim);

        if (c_delim) {
            str = token_strcpy(sc, c_delim);

            if (NULL == str) {
                /* Error */
                return NULL;
            }

            *next_s = (char *)st;
            free(sc);

            return str;
        }
    }

    /*
     * No token was found and has data. Sets NULL to @next_s to returns NULL
     * on next call.
     */
    st = *next_s;
    *next_s = NULL;

    return sc;
}

/*
 * Splits the cstring_t object around matches of the given tokens.
 */
cstring_list_t LIBEXPORT *cstring_split(const cstring_t *s, const char *delim)
{
    struct cstring_s *p = (struct cstring_s *)s;
    cstring_list_t *l = NULL;
    char *t = NULL, *tmp = NULL;

    cerrno_clear();

    if ((NULL == s) || (NULL == delim)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    l = cstring_list_new();

    if (NULL == l)
        return NULL;

    t = __strtok(p->str, delim, &tmp);

    if (NULL == t)
        return l;

    cstring_list_add(l, cstring_new(t));
    free(t);

    while ((t = __strtok(NULL, delim, &tmp)) != NULL) {
        cstring_list_add(l, cstring_new(t));
        free(t);
    }

    return l;
}

int LIBEXPORT cstring_value_as_int(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    char *endptr = NULL;
    int v;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LONG_MAX) || (v == LONG_MIN))) {
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    return v;
}

long LIBEXPORT cstring_value_as_long(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    char *endptr = NULL;
    long v;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LONG_MAX) || (v == LONG_MIN))) {
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    return v;
}

long long LIBEXPORT cstring_value_as_long_long(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    char *endptr = NULL;
    long long v;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtoll(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LLONG_MAX) || (v == LLONG_MIN))) {
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    return v;
}

float LIBEXPORT cstring_value_as_float(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    char *endptr = NULL;
    float v;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtof(p->str, &endptr);

    if (errno == ERANGE) {
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    return v;
}

double LIBEXPORT cstring_value_as_double(const cstring_t *s)
{
    struct cstring_s *p = (struct cstring_s *)s;
    char *endptr = NULL;
    double v;

    cerrno_clear();

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtod(p->str, &endptr);

    if (errno == ERANGE) {
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    return v;
}

