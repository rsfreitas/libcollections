
/*
 * Description: API to handle strings using a cstring_t object.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:46:43 2015
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

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "collections.h"

#define cstring_members                 \
    cl_struct_member(uint32_t, size)    \
    cl_struct_member(char *, str)       \
    cl_struct_member(struct ref_s, ref)

cl_struct_declare(cstring_s, cstring_members);

#define cstring_s           cl_struct(cstring_s)

static void destroy_string(const struct ref_s *ref)
{
    cstring_s *string = container_of(ref, cstring_s, ref);

    if (NULL == string)
        return;

    if (string->str != NULL)
        free(string->str);

    free(string);
}

static cstring_s *new_cstring(void)
{
    cstring_s *p = NULL;

    p = calloc(1, sizeof(cstring_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->str = NULL;
    set_typeof(CSTRING, p);

    /* reference count initialization */
    p->ref.free = destroy_string;
    p->ref.count = 1;

    return p;
}

__PUB_API__ cstring_t *cstring_ref(cstring_t *string)
{
    cstring_s *p = (cstring_s *)string;

    __clib_function_init__(true, string, CSTRING, NULL);
    ref_inc(&p->ref);

    return string;
}

__PUB_API__ int cstring_unref(cstring_t *string)
{
    cstring_s *p = (cstring_s *)string;

    __clib_function_init__(true, string, CSTRING, -1);
    ref_dec(&p->ref);

    return 0;
}

/*
 * Frees a cstring_t object from memory.
 */
__PUB_API__ int cstring_destroy(cstring_t *string)
{
    return cstring_unref(string);
}

/*
 * Creates a new cstring_t object.
 */
__PUB_API__ cstring_t *cstring_create(const char *fmt, ...)
{
    cstring_s *string = NULL;
    va_list ap;

    __clib_function_init__(false, NULL, -1, NULL);
    string = new_cstring();

    if (NULL == string)
        return NULL;

    if (NULL == fmt)
        return string;

    va_start(ap, fmt);
    string->size = vasprintf(&string->str, fmt, ap);
    va_end(ap);

    return string;
}

__PUB_API__ cstring_t *cstring_create_empty(unsigned int size)
{
    cstring_s *string = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    string = new_cstring();

    if (NULL == string)
        return NULL;

    if (size == 0)
        return string;

    string->str = calloc(size, sizeof(char));

    if (NULL == string->str) {
        cset_errno(CL_NO_MEM);
        cstring_destroy(string);
        return NULL;
    }

    return string;
}

/*
 * Creates a cstring_t object containing random letters.
 */
__PUB_API__ cstring_t *cstring_create_random(unsigned int size)
{
    cstring_s *p = NULL;
    unsigned int i;
    int n;

    __clib_function_init__(false, NULL, -1, NULL);
    p = new_cstring();

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
__PUB_API__ int cstring_length(const cstring_t *string)
{
    cstring_s *p;
    int l = -1;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
    l = p->size;
    cstring_unref(p);

    return l;
}

/*
 * Gets the value of a cstring_t object.
 */
__PUB_API__ const char *cstring_valueof(const cstring_t *string)
{
    cstring_s *p;
    char *ptr = NULL;

    __clib_function_init__(true, string, CSTRING, NULL);

    p = cstring_ref((cstring_t *)string);
    ptr = p->str;
    cstring_unref(p);

    return ptr;
}

/*
 * Returns the char value at the specified index.
 */
__PUB_API__ char cstring_at(const cstring_t *string, unsigned int index)
{
    cstring_s *p;
    char cnt = -1;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

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
__PUB_API__ int cstring_set(cstring_t *string, char c, unsigned int index)
{
    cstring_s *p;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

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
__PUB_API__ int cstring_cat(cstring_t *string, const char *fmt, ...)
{
    cstring_s *p;
    va_list ap;
    char *buff = NULL;
    int l=0;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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
__PUB_API__ int cstring_cmp(const cstring_t *s1, const cstring_t *s2)
{
    cstring_s *p1, *p2;
    int ret = -1;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(s1, CSTRING) == false) ||
        (validate_object(s2, CSTRING) == false))
    {
        return -1;
    }

    p1 = cstring_ref((cstring_t *)s1);
    p2 = cstring_ref((cstring_t *)s2);

    ret = strcmp(p1->str, p2->str);

    cstring_unref(p2);
    cstring_unref(p1);

    return ret;
}

/*
 * Compare two cstring_t objects.
 */
__PUB_API__ int cstring_ncmp(const cstring_t *s1, const cstring_t *s2, size_t n)
{
    cstring_s *p1, *p2;
    int ret = -1;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(s1, CSTRING) == false) ||
        (validate_object(s2, CSTRING) == false))
    {
        return -1;
    }

    p1 = cstring_ref((cstring_t *)s1);
    p2 = cstring_ref((cstring_t *)s2);

    ret = strncmp(p1->str, p2->str, n);

    cstring_unref(p2);
    cstring_unref(p1);

    return ret;
}

__PUB_API__ cstring_t *cstring_dup(const cstring_t *string)
{
    cstring_s *p;
    cstring_t *d = NULL;

    __clib_function_init__(true, string, CSTRING, NULL);

    p = cstring_ref((cstring_t *)string);
    d = cstring_create("%s", p->str);
    cstring_unref(p);

    return d;
}

__PUB_API__ int cstring_upper(cstring_t *string)
{
    cstring_s *p;
    unsigned int i;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref(string);

    for (i = 0; i < p->size; i++)
        if (isascii(p->str[i]))
            p->str[i] = toupper(p->str[i]);

    cstring_unref(p);

    return 0;
}

__PUB_API__ int cstring_lower(cstring_t *string)
{
    cstring_s *p;
    unsigned int i;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref(string);

    for (i = 0; i < p->size; i++)
        if (isascii(p->str[i]))
            p->str[i] = tolower(p->str[i]);

    cstring_unref(p);

    return 0;
}

__PUB_API__ int cstring_capitalize(cstring_t *string)
{
    cstring_s *p;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref(string);
    p->str[0] = toupper(p->str[0]);
    cstring_unref(p);

    return 0;
}

__PUB_API__ int cstring_find(const cstring_t *string, char c)
{
    cstring_s *p;
    int i, idx = -1 /* character not found */;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cstring_unref(p);

    return idx;
}

__PUB_API__ int cstring_rfind(const cstring_t *string, char c)
{
    cstring_s *p;
    int i, idx = 1 /* character not found */;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

    for (i = cstring_length(p) - 1; i >= 0; i--)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cstring_unref(p);

    return idx;
}

__PUB_API__ int cstring_cchr(const cstring_t *string, char c)
{
    cstring_s *p;
    int i, match = 0;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

    for (i = 0; i < cstring_length(p); i++)
        if (p->str[i] == c)
            match++;

    cstring_unref(p);

    return match;
}

__PUB_API__ int cstring_ltrim(cstring_t *string)
{
    cstring_s *p;
    int size, i;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref(string);
    size = cstring_length(p);

    while (p->str[0] == ' ') {
        for (i = 0; i < size; i++)
            p->str[i] = p->str[i + 1];

        size--;
        p->str[size] = '\0';
    }

    p->size = size;
    cstring_unref(p);

    return 0;
}

__PUB_API__ int cstring_rtrim(cstring_t *string)
{
    cstring_s *p;
    int size, i;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref(string);
    size = cstring_length(p);
    i = size - 1;

    while ((i >= 0) && (p->str[i] == ' ')) {
        p->str[i] = '\0';
        i--;
        size--;
    }

    p->size = size;
    cstring_unref(p);

    return 0;
}

__PUB_API__ int cstring_alltrim(cstring_t *string)
{
    cstring_s *p;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref(string);

    cstring_ltrim(p);
    cstring_rtrim(p);

    cstring_unref(p);

    return 0;
}

__PUB_API__ cstring_t *cstring_substr(const cstring_t *string, const char *needle)
{
    cstring_s *p, *o;
    char *ptr = NULL;

    __clib_function_init__(true, string, CSTRING, NULL);

    p = cstring_ref((cstring_t *)string);
    ptr = strstr(p->str, needle);

    if (NULL == ptr) {
        cstring_unref(p);
        return NULL;
    }

    o = cstring_create("%s", ptr);
    cstring_unref(p);

    if (NULL == o)
        return NULL;

    return o;
}

__PUB_API__ int cstring_rplchr(cstring_t *string, char c1, char c2)
{
    cstring_s *p;
    int i, c = 0, index;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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

__PUB_API__ int cstring_rplsubstr(cstring_t *string, const char *old,
    const char *new_)
{
    cstring_s *p;
    size_t l_old, l_new, l;
    char *s_in1, *s_in2, *s_out, *n;
    int n_old = 0;

    __clib_function_init__(true, string, CSTRING, -1);

    if ((NULL == old) || (NULL == new_)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    p = cstring_ref(string);
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
__PUB_API__ bool cstring_isempty(const cstring_t *string)
{
    cstring_s *p;
    bool b;

    __clib_function_init__(true, string, CSTRING, false);

    p = cstring_ref((cstring_t *)string);
    b = (cstring_length(p) > 0) ? true : false;
    cstring_unref(p);

    return b;
}

/*
 * Clears the content of a cstring_t object.
 */
__PUB_API__ int cstring_clear(cstring_t *string)
{
    cstring_s *p;

    __clib_function_init__(true, string, CSTRING, -1);
    p = cstring_ref((cstring_t *)string);

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
__PUB_API__ cstring_list_t *cstring_split(const cstring_t *string,
    const char *delim)
{
    cstring_s *p;
    cstring_list_t *l = NULL;
    char *t = NULL, *tmp = NULL;
    cstring_t *data;

    __clib_function_init__(true, string, CSTRING, NULL);

    if (NULL == delim) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cstring_ref((cstring_t *)string);
    l = cstring_list_create();

    if (NULL == l) {
        cstring_unref(p);
        return NULL;
    }

    t = __strtok(p->str, delim, &tmp);

    if (NULL == t) {
        cstring_unref(p);
        return l;
    }

    data = cstring_create("%s", t);
    cstring_list_add(l, data);
    cstring_unref(data);
    free(t);

    while ((t = __strtok(NULL, delim, &tmp)) != NULL) {
        data = cstring_create("%s", t);
        cstring_list_add(l, data);
        cstring_unref(data);
        free(t);
    }

    cstring_unref(p);

    return l;
}

__PUB_API__ int cstring_to_int(const cstring_t *string)
{
    cstring_s *p;
    char *endptr = NULL;
    int v;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == INT_MAX) || (v == INT_MIN))) {
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

__PUB_API__ long cstring_to_long(const cstring_t *string)
{
    cstring_s *p;
    char *endptr = NULL;
    long v;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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

__PUB_API__ long long cstring_to_long_long(const cstring_t *string)
{
    cstring_s *p;
    char *endptr = NULL;
    long long v;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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

__PUB_API__ float cstring_to_float(const cstring_t *string)
{
    cstring_s *p;
    char *endptr = NULL;
    float v;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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

__PUB_API__ double cstring_to_double(const cstring_t *string)
{
    cstring_s *p;
    char *endptr = NULL;
    double v;

    __clib_function_init__(true, string, CSTRING, -1);

    p = cstring_ref((cstring_t *)string);
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

__PUB_API__ bool cstring_is_number(const cstring_t *string)
{
    cstring_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CSTRING, false);

    value = cstring_ref((cstring_t *)string);
    l = cstring_length(value);

    for (i = 0; i < l; i++)
        if (isdigit(cstring_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cstring_unref(value);

    return ret;
}

__PUB_API__ bool cstring_is_float_number(const cstring_t *string)
{
    cstring_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CSTRING, false);

    value = cstring_ref((cstring_t *)string);
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

__PUB_API__ bool cstring_is_alphanumeric(const cstring_t *string)
{
    cstring_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CSTRING, false);

    value = cstring_ref((cstring_t *)string);
    l = cstring_length(value);

    for (i = 0; i < l; i++)
        if (isalnum(cstring_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cstring_unref(value);

    return ret;
}

__PUB_API__ int cstring_idchr(cstring_t *string, unsigned int p)
{
    cstring_s *s;
    int size;

    __clib_function_init__(true, string, CSTRING, -1);

    s = cstring_ref(string);
    size = cstring_length(s);

    if (p > (unsigned int)size) {
        cstring_unref(s);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    memmove(&s->str[p], &s->str[p + 1], size - p);
    cstring_unref(s);

    return 0;
}

__PUB_API__ int cstring_dchr(cstring_t *string, char c)
{
    cstring_t *value;
    int t, p;

    __clib_function_init__(true, string, CSTRING, -1);

    value = cstring_ref(string);
    t = cstring_cchr(value, c);

    if (t == 0)
        goto end_block;

    do {
        p = cstring_find(value, c);
        cstring_idchr(value, p);
        t--;
    } while (t);

end_block:
    cstring_unref(value);
    return 0;
}

__PUB_API__ int cstring_cpy(cstring_t *dest, const cstring_t *src)
{
    cstring_t *p, *q;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(dest, CSTRING) == false) ||
        (validate_object(src, CSTRING) == false))
    {
        return -1;
    }

    p = cstring_ref(dest);
    q = cstring_ref((cstring_t *)src);

    cstring_clear(p);
    cstring_cat(p, "%s", cstring_valueof(q));

    cstring_unref(q);
    cstring_unref(p);

    return 0;
}

