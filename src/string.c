
/*
 * Description: API to handle strings using a cl_string_t object.
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

#define cl_string_members                   \
    cl_struct_member(uint32_t, size)        \
    cl_struct_member(char *, str)           \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cl_string_s, cl_string_members);

#define cl_string_s           cl_struct(cl_string_s)

static void destroy_string(const struct cl_ref_s *ref)
{
    cl_string_s *string = cl_container_of(ref, cl_string_s, ref);

    if (NULL == string)
        return;

    if (string->str != NULL)
        free(string->str);

    free(string);
    string = NULL;
}

static cl_string_s *new_cstring(void)
{
    cl_string_s *p = NULL;

    p = calloc(1, sizeof(cl_string_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->str = NULL;
    set_typeof(CL_OBJ_STRING, p);

    /* reference count initialization */
    p->ref.free = destroy_string;
    p->ref.count = 1;

    return p;
}

__PUB_API__ cl_string_t *cl_string_ref(cl_string_t *string)
{
    cl_string_s *p = (cl_string_s *)string;

    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);
    cl_ref_inc(&p->ref);

    return string;
}

__PUB_API__ int cl_string_unref(cl_string_t *string)
{
    cl_string_s *p = (cl_string_s *)string;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    cl_ref_dec(&p->ref);

    return 0;
}

/*
 * Frees a cl_string_t object from memory.
 */
__PUB_API__ int cl_string_destroy(cl_string_t *string)
{
    return cl_string_unref(string);
}

/*
 * Creates a new cl_string_t object.
 */
__PUB_API__ cl_string_t *cl_string_create(const char *fmt, ...)
{
    cl_string_s *string = NULL;
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

__PUB_API__ cl_string_t *cl_string_create_empty(unsigned int size)
{
    cl_string_s *string = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    string = new_cstring();

    if (NULL == string)
        return NULL;

    if (size == 0)
        return string;

    string->str = calloc(size, sizeof(char));

    if (NULL == string->str) {
        cset_errno(CL_NO_MEM);
        cl_string_destroy(string);
        return NULL;
    }

    return string;
}

/*
 * Creates a cl_string_t object containing random letters.
 */
__PUB_API__ cl_string_t *cl_string_create_random(unsigned int size)
{
    cl_string_s *p = NULL;
    unsigned int i;
    int n;

    __clib_function_init__(false, NULL, -1, NULL);
    p = new_cstring();

    if (NULL == p)
        return NULL;

    for (i = 0; i < size; i++) {
        n = 'a' + cl_rand('z' - 'a');
        cl_string_cat(p, "%c", (char)n);
    }

    return p;
}

/*
 * Gets the length of a cl_string_t object.
 */
__PUB_API__ int cl_string_length(const cl_string_t *string)
{
    cl_string_s *p;
    int l = -1;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    l = p->size;
    cl_string_unref(p);

    return l;
}

/*
 * Gets the value of a cl_string_t object.
 */
__PUB_API__ const char *cl_string_valueof(const cl_string_t *string)
{
    cl_string_s *p;
    char *ptr = NULL;

    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);

    p = cl_string_ref((cl_string_t *)string);
    ptr = p->str;
    cl_string_unref(p);

    return ptr;
}

/*
 * Returns the char value at the specified index.
 */
__PUB_API__ char cl_string_at(const cl_string_t *string, unsigned int index)
{
    cl_string_s *p;
    char cnt = -1;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    if (index >= (unsigned int)cl_string_length(p)) {
        cl_string_unref(p);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    cnt = p->str[index];
    cl_string_unref(p);

    return cnt;
}

/*
 * Changes the value of a specified index to a new value.
 */
__PUB_API__ int cl_string_set(cl_string_t *string, char c, unsigned int index)
{
    cl_string_s *p;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    if (index >= (unsigned int)cl_string_length(p)) {
        cl_string_unref(p);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    p->str[index] = c;
    cl_string_unref(p);

    return 0;
}

/*
 * Concatenate two strings.
 */
__PUB_API__ int cl_string_cat(cl_string_t *string, const char *fmt, ...)
{
    cl_string_s *p;
    va_list ap;
    char *buff = NULL;
    int l=0;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
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
    cl_string_unref(p);
    return 0;
}

/*
 * Compare two cl_string_t objects.
 */
__PUB_API__ int cl_string_cmp(const cl_string_t *s1, const cl_string_t *s2)
{
    cl_string_s *p1, *p2;
    int ret = -1;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(s1, CL_OBJ_STRING) == false) ||
        (validate_object(s2, CL_OBJ_STRING) == false))
    {
        return -1;
    }

    p1 = cl_string_ref((cl_string_t *)s1);
    p2 = cl_string_ref((cl_string_t *)s2);

    ret = strcmp(p1->str, p2->str);

    cl_string_unref(p2);
    cl_string_unref(p1);

    return ret;
}

/*
 * Compare two cl_string_t objects.
 */
__PUB_API__ int cl_string_ncmp(const cl_string_t *s1, const cl_string_t *s2,
    size_t n)
{
    cl_string_s *p1, *p2;
    int ret = -1;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(s1, CL_OBJ_STRING) == false) ||
        (validate_object(s2, CL_OBJ_STRING) == false))
    {
        return -1;
    }

    p1 = cl_string_ref((cl_string_t *)s1);
    p2 = cl_string_ref((cl_string_t *)s2);

    ret = strncmp(p1->str, p2->str, n);

    cl_string_unref(p2);
    cl_string_unref(p1);

    return ret;
}

__PUB_API__ cl_string_t *cl_string_dup(const cl_string_t *string)
{
    cl_string_s *p;
    cl_string_t *d = NULL;

    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);

    p = cl_string_ref((cl_string_t *)string);
    d = cl_string_create("%s", p->str);
    cl_string_unref(p);

    return d;
}

__PUB_API__ int cl_string_upper(cl_string_t *string)
{
    cl_string_s *p;
    unsigned int i;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref(string);

    for (i = 0; i < p->size; i++)
        if (isascii(p->str[i]))
            p->str[i] = toupper(p->str[i]);

    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_lower(cl_string_t *string)
{
    cl_string_s *p;
    unsigned int i;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref(string);

    for (i = 0; i < p->size; i++)
        if (isascii(p->str[i]))
            p->str[i] = tolower(p->str[i]);

    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_capitalize(cl_string_t *string)
{
    cl_string_s *p;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref(string);
    p->str[0] = toupper(p->str[0]);
    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_find(const cl_string_t *string, char c)
{
    cl_string_s *p;
    int i, idx = -1 /* character not found */;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    for (i = 0; i < cl_string_length(p); i++)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cl_string_unref(p);

    return idx;
}

__PUB_API__ int cl_string_rfind(const cl_string_t *string, char c)
{
    cl_string_s *p;
    int i, idx = 1 /* character not found */;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    for (i = cl_string_length(p) - 1; i >= 0; i--)
        if (p->str[i] == c) {
            idx = i;
            break;
        }

    cl_string_unref(p);

    return idx;
}

__PUB_API__ int cl_string_cchr(const cl_string_t *string, char c)
{
    cl_string_s *p;
    int i, match = 0;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    for (i = 0; i < cl_string_length(p); i++)
        if (p->str[i] == c)
            match++;

    cl_string_unref(p);

    return match;
}

__PUB_API__ int cl_string_ltrim(cl_string_t *string)
{
    cl_string_s *p;
    int size, i;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref(string);
    size = cl_string_length(p);

    while (p->str[0] == ' ') {
        for (i = 0; i < size; i++)
            p->str[i] = p->str[i + 1];

        size--;
        p->str[size] = '\0';
    }

    p->size = size;
    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_rtrim(cl_string_t *string)
{
    cl_string_s *p;
    int size, i;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref(string);
    size = cl_string_length(p);
    i = size - 1;

    while ((i >= 0) && (p->str[i] == ' ')) {
        p->str[i] = '\0';
        i--;
        size--;
    }

    p->size = size;
    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_alltrim(cl_string_t *string)
{
    cl_string_s *p;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref(string);

    cl_string_ltrim(p);
    cl_string_rtrim(p);

    cl_string_unref(p);

    return 0;
}

__PUB_API__ cl_string_t *cl_string_substr(const cl_string_t *string,
    const char *needle)
{
    cl_string_s *p, *o;
    char *ptr = NULL;

    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);

    p = cl_string_ref((cl_string_t *)string);
    ptr = strstr(p->str, needle);

    if (NULL == ptr) {
        cl_string_unref(p);
        return NULL;
    }

    o = cl_string_create("%s", ptr + strlen(needle));
    cl_string_unref(p);

    if (NULL == o)
        return NULL;

    return o;
}

__PUB_API__ int cl_string_rplchr(cl_string_t *string, char c1, char c2)
{
    cl_string_s *p;
    int i, c = 0, index, substitutions = 0;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    c = cl_string_cchr(p, c1);

    if (c <= 0) {
        cl_string_unref(p);
        return 0;
    }

    for (i = 0; i < c; i++) {
        index = cl_string_find(p, c1);

        if (index < 0)
            break;

        cl_string_set(p, c2, index);
        substitutions++;
    }

    cl_string_unref(p);

    return substitutions;
}

__PUB_API__ int cl_string_rplsubstr(cl_string_t *string, const char *old,
    const char *new_)
{
    cl_string_s *p;
    size_t l_old, l_new, l;
    char *s_in1, *s_in2, *s_out, *n;
    int n_old = 0, substitutions = 0;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    if ((NULL == old) || (NULL == new_)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    p = cl_string_ref(string);
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
        cl_string_unref(p);
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
            substitutions++;
        }
    }

    strcat(s_out, s_in2);

    /* Replace on cl_string_t object */
    free(p->str);
    p->str = n;
    p->size = l;
    cl_string_unref(p);

    return substitutions;
}

/*
 * Checks if a cl_string_t object has any valid data.
 */
__PUB_API__ bool cl_string_isempty(const cl_string_t *string)
{
    cl_string_s *p;
    bool b;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);

    p = cl_string_ref((cl_string_t *)string);
    b = (cl_string_length(p) > 0) ? true : false;
    cl_string_unref(p);

    return b;
}

/*
 * Clears the content of a cl_string_t object.
 */
__PUB_API__ int cl_string_clear(cl_string_t *string)
{
    cl_string_s *p;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);
    p = cl_string_ref((cl_string_t *)string);

    if (p->str != NULL) {
        free(p->str);
        p->str = NULL;
    }

    p->size = 0;
    cl_string_unref(p);

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
 * Splits the cl_string_t object around matches of the given tokens.
 */
__PUB_API__ cl_stringlist_t *cl_string_split(const cl_string_t *string,
    const char *delim)
{
    cl_string_s *p;
    cl_stringlist_t *l = NULL;
    char *t = NULL, *tmp = NULL;
    cl_string_t *data;

    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);

    if (NULL == delim) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cl_string_ref((cl_string_t *)string);
    l = cl_stringlist_create();

    if (NULL == l) {
        cl_string_unref(p);
        return NULL;
    }

    t = __strtok(p->str, delim, &tmp);

    if (NULL == t) {
        cl_string_unref(p);
        return l;
    }

    data = cl_string_create("%s", t);
    cl_stringlist_add(l, data);
    cl_string_unref(data);
    free(t);

    while ((t = __strtok(NULL, delim, &tmp)) != NULL) {
        data = cl_string_create("%s", t);
        cl_stringlist_add(l, data);
        cl_string_unref(data);
        free(t);
    }

    cl_string_unref(p);

    return l;
}

__PUB_API__ int cl_string_to_int(const cl_string_t *string)
{
    cl_string_s *p;
    char *endptr = NULL;
    int v;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == INT_MAX) || (v == INT_MIN))) {
        cl_string_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cl_string_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cl_string_unref(p);

    return v;
}

__PUB_API__ long cl_string_to_long(const cl_string_t *string)
{
    cl_string_s *p;
    char *endptr = NULL;
    long v;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    errno = 0;
    v = strtol(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LONG_MAX) || (v == LONG_MIN))) {
        cl_string_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cl_string_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cl_string_unref(p);

    return v;
}

__PUB_API__ long long cl_string_to_long_long(const cl_string_t *string)
{
    cl_string_s *p;
    char *endptr = NULL;
    long long v;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    errno = 0;
    v = strtoll(p->str, &endptr, 10);

    if ((errno == ERANGE) && ((v == LLONG_MAX) || (v == LLONG_MIN))) {
        cl_string_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cl_string_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cl_string_unref(p);

    return v;
}

__PUB_API__ float cl_string_to_float(const cl_string_t *string)
{
    cl_string_s *p;
    char *endptr = NULL;
    float v;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    errno = 0;
    v = strtof(p->str, &endptr);

    if (errno == ERANGE) {
        cl_string_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cl_string_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cl_string_unref(p);

    return v;
}

__PUB_API__ double cl_string_to_double(const cl_string_t *string)
{
    cl_string_s *p;
    char *endptr = NULL;
    double v;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    p = cl_string_ref((cl_string_t *)string);
    errno = 0;
    v = strtod(p->str, &endptr);

    if (errno == ERANGE) {
        cl_string_unref(p);
        cset_errno(CL_NUMBER_RANGE);
        return -1;
    }

    if (endptr == p->str) {
        cl_string_unref(p);
        cset_errno(CL_NOT_A_NUMBER);
        return -1;
    }

    cl_string_unref(p);

    return v;
}

__PUB_API__ bool cl_string_is_number(const cl_string_t *string)
{
    cl_string_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);
    value = cl_string_ref((cl_string_t *)string);
    l = cl_string_length(value);

    for (i = 0; i < l; i++)
        if (isdigit(cl_string_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cl_string_unref(value);

    return ret;
}

__PUB_API__ bool cl_string_is_float_number(const cl_string_t *string)
{
    cl_string_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);

    value = cl_string_ref((cl_string_t *)string);
    l = cl_string_length(value);

    for (i = 0; i < l; i++) {
        if ((isdigit(cl_string_at(value, i)) == 0) &&
            (cl_string_at(value, i) != '.'))
        {
            ret = false;
            break;
        }
    }

    cl_string_unref(value);

    return ret;
}

__PUB_API__ bool cl_string_is_alphanumeric(const cl_string_t *string)
{
    cl_string_t *value;
    int i, l;
    bool ret = true;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);

    value = cl_string_ref((cl_string_t *)string);
    l = cl_string_length(value);

    for (i = 0; i < l; i++)
        if (isalnum(cl_string_at(value, i)) == 0) {
            ret = false;
            break;
        }

    cl_string_unref(value);

    return ret;
}

__PUB_API__ int cl_string_idchr(cl_string_t *string, unsigned int p)
{
    cl_string_s *s;
    int size;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    s = cl_string_ref(string);
    size = cl_string_length(s);

    if (p > (unsigned int)size) {
        cl_string_unref(s);
        cset_errno(CL_WRONG_STRING_INDEX);
        return -1;
    }

    memmove(&s->str[p], &s->str[p + 1], size - p);
    cl_string_unref(s);

    return 0;
}

__PUB_API__ int cl_string_dchr(cl_string_t *string, char c)
{
    cl_string_t *value;
    int t, p;

    __clib_function_init__(true, string, CL_OBJ_STRING, -1);

    value = cl_string_ref(string);
    t = cl_string_cchr(value, c);

    if (t == 0)
        goto end_block;

    do {
        p = cl_string_find(value, c);
        cl_string_idchr(value, p);
        t--;
    } while (t);

end_block:
    cl_string_unref(value);
    return 0;
}

__PUB_API__ int cl_string_cpy(cl_string_t *dest, const cl_string_t *src)
{
    cl_string_t *p, *q;

    __clib_function_init__(false, NULL, -1, -1);

    if ((validate_object(dest, CL_OBJ_STRING) == false) ||
        (validate_object(src, CL_OBJ_STRING) == false))
    {
        return -1;
    }

    p = cl_string_ref(dest);
    q = cl_string_ref((cl_string_t *)src);

    cl_string_clear(p);
    cl_string_cat(p, "%s", cl_string_valueof(q));

    cl_string_unref(q);
    cl_string_unref(p);

    return 0;
}

__PUB_API__ bool cl_string_contains(const cl_string_t *string,
    const char *needle)
{
    bool contains = false;
    cl_string_t *p, *sub = NULL;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);

    if (NULL == needle)
        return false;

    p = cl_string_ref((cl_string_t *)string);
    sub = cl_string_substr(p, needle);
    cl_string_unref(p);

    if (sub != NULL) {
        cl_string_unref(sub);
        contains = true;
    }

    return contains;
}

__PUB_API__ int cl_string_count_matches(const cl_string_t *string,
    const char *needle)
{
    cl_string_t *sub = NULL, *haystack = cl_string_ref((cl_string_t *)string);;
    unsigned int count = 0;

    __clib_function_init__(true, string, CL_OBJ_STRING, false);

    if (NULL == needle)
        return -1;

    do {
        sub = cl_string_substr(haystack, needle);
        cl_string_unref(haystack);

        if (NULL == sub)
            break;

        /* Needle found */
        count++;
        haystack = sub;

    } while (sub != NULL);

    return count;
}

__PUB_API__ int cl_string_set_content(cl_string_t *s, const char *content)
{
    cl_string_s *p = NULL;

    __clib_function_init__(true, s, CL_OBJ_STRING, -1);

    if (NULL == content) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    cl_string_clear(s);

    p = cl_string_ref(s);
    p->str = (char *)content;
    p->size = strlen(content);
    cl_string_unref(p);

    return 0;
}

__PUB_API__ int cl_string_update_length(cl_string_t *s)
{
    cl_string_s *p = NULL;

    __clib_function_init__(true, s, CL_OBJ_STRING, -1);

    p = cl_string_ref(s);
    p->size = strlen(p->str);
    cl_string_unref(p);

    return 0;
}

