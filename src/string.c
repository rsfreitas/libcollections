
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
    uint32_t        size;
    char            *str;

    /* reference count */
    struct ref_s    ref;
};

static void destroy_string(const struct ref_s *ref)
{
    struct cstring_s *string = container_of(ref, struct cstring_s, ref);

    if (NULL == string)
        return;

    if (string->str != NULL)
        free(string->str);

    free(string);
}

static struct cstring_s *cstring_new_empty(void)
{
    struct cstring_s *p = NULL;

    p = calloc(1, sizeof(struct cstring_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->str = NULL;

    /* reference count initialization */
    p->ref.free = destroy_string;
    p->ref.count = 1;

    return p;
}

cstring_t LIBEXPORT *cstring_ref(cstring_t *string)
{
    struct cstring_s *p = (struct cstring_s *)string;

    cerrno_clear();

    if (NULL == string) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ref_inc(&p->ref);

    return string;
}

int LIBEXPORT cstring_unref(cstring_t *string)
{
    struct cstring_s *p = (struct cstring_s *)string;

    cerrno_clear();

    if (NULL == string) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ref_dec(&p->ref);

    return 0;
}

/*
 * Frees a cstring_t object from memory.
 */
int LIBEXPORT cstring_free(cstring_t *string)
{
    return cstring_unref(string);
}

/*
 * Creates a new cstring_t object.
 */
cstring_t LIBEXPORT *cstring_new(const char *fmt, ...)
{
    struct cstring_s *string = NULL;
    va_list ap;

    cerrno_clear();
    string = cstring_new_empty();

    if (NULL == string)
        return NULL;

    if (NULL == fmt)
        return string;

    va_start(ap, fmt);
    string->size = vasprintf(&string->str, fmt, ap);
    va_end(ap);

    return string;
}

cstring_t LIBEXPORT *cstring_new_ex(unsigned int size)
{
    struct cstring_s *string = NULL;

    cerrno_clear();

    if (size == 0) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    string = cstring_new_empty();

    if (NULL == string)
        return NULL;

    string->str = calloc(size, sizeof(char));

    if (NULL == string->str) {
        cset_errno(CL_NO_MEM);
        cstring_free(string);
        return NULL;
    }

    return string;
}

/*
 * Creates a cstring_t object containing random letters.
 */
cstring_t LIBEXPORT *cstring_new_random(unsigned int size)
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
 * Gets the length of a cstring_t object.
 */
int LIBEXPORT cstring_length(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    int l = -1;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    l = p->size;
    cstring_unref(p);

    return l;
}

/*
 * Gets the value of a cstring_t object.
 */
const char LIBEXPORT *cstring_valueof(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *ptr = NULL;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ptr = p->str;
    cstring_unref(p);

    return ptr;
}

/*
 * Returns the char value at the specified index.
 */
char LIBEXPORT cstring_at(const cstring_t *string, unsigned int index)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char cnt = -1;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (index >= (unsigned int)cstring_length(p)) {
        cstring_unref(p);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    cnt = p->str[index];
    cstring_unref(p);

    return cnt;
}

/*
 * Changes the value of a specified index to a new value.
 */
int LIBEXPORT cstring_set(cstring_t *string, char c, unsigned int index)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (index >= (unsigned int)cstring_length(p)) {
        cstring_unref(p);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    p->str[index] = c;
    cstring_unref(p);

    return 0;
}

/*
 * Concatenate two strings.
 */
int LIBEXPORT cstring_cat(cstring_t *string, const char *fmt, ...)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    va_list ap;
    char *buff = NULL;
    int l=0;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    l = vasprintf(&buff, fmt, ap);
    va_end(ap);

    if (NULL == p->str) {
        p->str = buff;
        p->size = l;
        goto end_block;
    }

    p->str = realloc(p->str, p->size + l + 1);
    memcpy(&p->str[p->size], buff, l);
    p->size += l;
    p->str[p->size] = '\0';

    if (buff != NULL)
        free(buff);

end_block:
    cstring_unref(p);
    return 0;
}

/*
 * Compare two cstring_t objects.
 */
int LIBEXPORT cstring_cmp(const cstring_t *s1, const cstring_t *s2)
{
    struct cstring_s *p1 = cstring_ref((cstring_t *)s1);
    struct cstring_s *p2 = cstring_ref((cstring_t *)s2);
    int ret = -1;

    cerrno_clear();

    if ((NULL == s1) || (NULL == s2)) {
        cstring_unref(p2);
        cstring_unref(p1);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ret = strcmp(p1->str, p2->str);

    cstring_unref(p2);
    cstring_unref(p1);

    return ret;
}

/*
 * Compare two cstring_t objects.
 */
int LIBEXPORT cstring_ncmp(const cstring_t *s1, const cstring_t *s2, size_t n)
{
    struct cstring_s *p1 = cstring_ref((cstring_t *)s1);
    struct cstring_s *p2 = cstring_ref((cstring_t *)s2);
    int ret = -1;

    cerrno_clear();

    if ((NULL == s1) || (NULL == s2)) {
        cstring_unref(p2);
        cstring_unref(p1);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    ret = strncmp(p1->str, p2->str, n);

    cstring_unref(p2);
    cstring_unref(p1);

    return ret;
}

cstring_t LIBEXPORT *cstring_dup(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    cstring_t *d = NULL;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    d = cstring_new("%s", p->str);
    cstring_unref(p);

    return d;
}

cstring_t LIBEXPORT *cstring_upper(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;
    unsigned int i;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);
    cstring_unref(p);

    if (o != NULL)
        for (i = 0; i < o->size; i++)
            if (isascii(o->str[i]))
                o->str[i] = toupper(o->str[i]);

    return o;
}

cstring_t LIBEXPORT *cstring_lower(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;
    unsigned int i;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);
    cstring_unref(p);

    if (o != NULL)
        for (i = 0; i < o->size; i++)
            if (isascii(o->str[i]))
                o->str[i] = tolower(o->str[i]);

    return o;
}

cstring_t LIBEXPORT *cstring_capitalize(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);
    cstring_unref(p);

    if (o != NULL)
        o->str[0] = toupper(o->str[0]);

    return o;
}

int LIBEXPORT cstring_find(const cstring_t *string, char c)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    int i, idx = -1 /* character not found */;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cstring_unref(p);

    return idx;
}

int LIBEXPORT cstring_rfind(const cstring_t *string, char c)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    int i, idx = 1 /* character not found */;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = cstring_length(p) - 1; i >= 0; i--)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cstring_unref(p);

    return idx;
}

int LIBEXPORT cstring_cchr(const cstring_t *string, char c)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    int i, match = 0;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c)
            match++;

    cstring_unref(p);

    return match;
}

cstring_t LIBEXPORT *cstring_ltrim(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;
    int size, i;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);
    cstring_unref(p);

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

cstring_t LIBEXPORT *cstring_rtrim(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;
    int size, i;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_dup(p);
    cstring_unref(p);

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

cstring_t LIBEXPORT *cstring_alltrim(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    o = cstring_ltrim(p);
    cstring_unref(p);

    if (NULL == o)
        return NULL;

    p = cstring_rtrim(o);
    cstring_free(o);

    if (NULL == p)
        return NULL;

    return p;
}

cstring_t LIBEXPORT *cstring_substr(const cstring_t *string, const char *needle)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    struct cstring_s *o = NULL;
    char *ptr = NULL;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ptr = strstr(p->str, needle);

    if (NULL == ptr) {
        cstring_unref(p);
        return NULL;
    }

    o = cstring_new("%s", ptr);
    cstring_unref(p);

    if (NULL == o)
        return NULL;

    return o;
}

int LIBEXPORT cstring_rplchr(cstring_t *string, char c1, char c2)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    int i, c = 0, index;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    c = cstring_cchr(p, c1);

    if (c <= 0) {
        cstring_unref(p);
        return 0;
    }

    for (i = 0; i < c; i++) {
        index = cstring_find(p, c1);

        if (index < 0)
            break;

        cstring_set(p, c2, index);
    }

    cstring_unref(p);

    return 0;
}

int LIBEXPORT cstring_rplsubstr(cstring_t *string, const char *old,
    const char *new_)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    size_t l_old, l_new, l;
    char *s_in1, *s_in2, *s_out, *n;
    int n_old = 0;

    cerrno_clear();

    if ((NULL == string) || (NULL == old) || (NULL == new_)) {
        cstring_unref(p);
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
        cstring_unref(p);
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
    cstring_unref(p);

    return 0;
}

/*
 * Checks if a cstring_t object has any valid data.
 */
bool LIBEXPORT cstring_isempty(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    bool b;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return false;
    }

    b = (cstring_length(p) > 0) ? true : false;
    cstring_unref(p);

    return b;
}

/*
 * Clears the content of a cstring_t object.
 */
int LIBEXPORT cstring_clear(cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (p->str != NULL) {
        free(p->str);
        p->str = NULL;
    }

    p->size = 0;
    cstring_unref(p);

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
 * Gets a substring of @string until the first token is found.
 */
static char *token_strcpy(const char *string, char token)
{
    int i=0;
    char *tmp, *p;

    tmp = strdup(string);

    while (*string) {
        if (*string++ == token) {
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
 * @param [in] string: Original string.
 * @param [in] delim: String containing token list of chars.
 * @param [out] next_s: Pointer to store the remaining string.
 *
 * @return Returns NULL when no substring is found or a new substring that
 *         must be freed by the user.
 */
static char *__strtok(const char *string, const char *delim, char **next_s)
{
    char *str, c_delim, *st, *sc;

    if (string)
        st = (char *)string;
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
cstring_list_t LIBEXPORT *cstring_split(const cstring_t *string,
    const char *delim)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    cstring_list_t *l = NULL;
    char *t = NULL, *tmp = NULL;
    cstring_t *data;

    cerrno_clear();

    if ((NULL == string) || (NULL == delim)) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    l = cstring_list_new();

    if (NULL == l) {
        cstring_unref(p);
        return NULL;
    }

    t = __strtok(p->str, delim, &tmp);

    if (NULL == t) {
        cstring_unref(p);
        return l;
    }

    data = cstring_new(t);
    cstring_list_add(l, data);
    cstring_unref(data);
    free(t);

    while ((t = __strtok(NULL, delim, &tmp)) != NULL) {
        data = cstring_new(t);
        cstring_list_add(l, data);
        cstring_unref(data);
        free(t);
    }

    cstring_unref(p);

    return l;
}

int LIBEXPORT cstring_value_as_int(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *endptr = NULL;
    int v;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LONG_MAX) || (v == LONG_MIN))) {
        cstring_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cstring_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cstring_unref(p);

    return v;
}

long LIBEXPORT cstring_value_as_long(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *endptr = NULL;
    long v;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LONG_MAX) || (v == LONG_MIN))) {
        cstring_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cstring_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cstring_unref(p);

    return v;
}

long long LIBEXPORT cstring_value_as_long_long(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *endptr = NULL;
    long long v;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtoll(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LLONG_MAX) || (v == LLONG_MIN))) {
        cstring_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cstring_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cstring_unref(p);

    return v;
}

float LIBEXPORT cstring_value_as_float(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *endptr = NULL;
    float v;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtof(p->str, &endptr);

    if (errno == ERANGE) {
        cstring_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cstring_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cstring_unref(p);

    return v;
}

double LIBEXPORT cstring_value_as_double(const cstring_t *string)
{
    struct cstring_s *p = cstring_ref((cstring_t *)string);
    char *endptr = NULL;
    double v;

    cerrno_clear();

    if (NULL == string) {
        cstring_unref(p);
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    errno = 0;
    v = strtod(p->str, &endptr);

    if (errno == ERANGE) {
        cstring_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cstring_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cstring_unref(p);

    return v;
}

bool LIBEXPORT cstring_is_number(const cstring_t *string)
{
    cstring_t *value = cstring_ref((cstring_t *)string);
    int i, l;
    bool ret = true;

    cerrno_clear();

    if (NULL == value) {
        cstring_unref(value);
        cset_errno(CL_NULL_ARG);
        return false;
    }

    l = cstring_length(value);

    for (i = 0; i < l; i++)
        if (isdigit(cstring_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cstring_unref(value);

    return ret;
}

bool LIBEXPORT cstring_is_float_number(const cstring_t *string)
{
    cstring_t *value = cstring_ref((cstring_t *)string);
    int i, l;
    bool ret = true;

    cerrno_clear();

    if (NULL == value) {
        cstring_unref(value);
        cset_errno(CL_NULL_ARG);
        return false;
    }

    l = cstring_length(value);

    for (i = 0; i < l; i++) {
        if ((isdigit(cstring_at(value, i)) == 0) ||
            (cstring_at(value, i) != '.'))
        {
            ret = false;
            break;
        }
    }

    cstring_unref(value);

    return ret;
}

bool LIBEXPORT cstring_is_alphanumeric(const cstring_t *string)
{
    cstring_t *value = cstring_ref((cstring_t *)string);
    int i, l;
    bool ret = true;

    cerrno_clear();

    if (NULL == value) {
        cstring_unref(value);
        cset_errno(CL_NULL_ARG);
        return false;
    }

    l = cstring_length(value);

    for (i = 0; i < l; i++)
        if (isalnum(cstring_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cstring_unref(value);

    return ret;
}

