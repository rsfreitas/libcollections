
/*
 * Description: API do handle JSON data. This API is based on
 *              cJSON by Dave Gamble.
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

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include <math.h>

#include "collections.h"

#define CJSON_IS_REFERENCE              256

#define cjson_members                       \
    cl_struct_member(clist_t *, prev)       \
    cl_struct_member(clist_t *, next)       \
    cl_struct_member(void *, child)         \
    cl_struct_member(cstring_t *, name)     \
    cl_struct_member(enum cjson_type, type) \
    cl_struct_member(cstring_t *, value)

cl_struct_declare(cjson_s, cjson_members);

#define cjson_s             cl_struct(cjson_s)

struct jvalue_s {
    char            *value;
    size_t          vlen;
    enum cjson_type type;
};

static const char *parse(cjson_s *n, const char *s);
static char *print_value(cjson_s *j, int depth, bool fmt);

struct jvalue_s __jvalues[] = {
    { "null",   4,  CJSON_NULL },
    { "false",  5,  CJSON_FALSE },
    { "true",   4,  CJSON_TRUE }
};

#define JVALUES_SIZE            \
    (sizeof(__jvalues) / (sizeof(__jvalues[0])))

static const unsigned char __first_byte_mark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static const char *get_jvalue_value(enum cjson_type type)
{
    unsigned int i;

    for (i = 0; i < JVALUES_SIZE; i++)
        if (__jvalues[i].type == type)
            return __jvalues[i].value;

    return NULL;
}

static cjson_s *cjson_new(void)
{
    cjson_s *j = NULL;

    j = calloc(1, sizeof(cjson_s));

    if (NULL == j) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    j->child = NULL;
    set_typeof(CJSON, j);

    return j;
}

static cjson_t *__dup(const cjson_s *j)
{
    cjson_s *p = NULL;

    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = j->type;
    p->value = (j->value != NULL) ? cstring_dup(j->value) : NULL;
    p->name = (j->name != NULL) ? cstring_dup(j->name) : NULL;

    return p;
}

static void __cjson_delete(void *a)
{
    cjson_s *c = (cjson_s *)a;

    if (!(c->type & CJSON_IS_REFERENCE) && c->child)
        cdll_free(c->child, __cjson_delete);

    if (!(c->type & CJSON_IS_REFERENCE) && c->value)
        cstring_destroy(c->value);

    if (c->name)
        cstring_destroy(c->name);

    free(c);
}

/*
 * Skip special chars.
 */
static const char *skip_chars(const char *in)
{
    while (in && *in && ((unsigned char)*in <= 32))
        in++;

    return in;
}

static int get_string_length(const char *s)
{
    const char *p = s + 1;
    int l=0;

    while (*p != '\"' && *p && ++l)
        if (*p++ == '\\')
            p++;

    return l;
}

static const char *parse_string(cjson_s *n, const char *s)
{
    const char *ptr = s + 1;
    char *ptr2;
    int len = 0;
    unsigned uc, uc2;
    cstring_t *out;

    if (*s != '\"') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    len = get_string_length(s);
    out = cstring_create_empty(len + 1);

    if (NULL == out)
        return NULL;

    ptr2 = (char *)cstring_valueof(out);

    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            ptr++;

            switch (*ptr) {
                case 'b':
                    *ptr2++ = '\b';
                    break;

                case 'f':
                    *ptr2++ = '\f';
                    break;

                case 'n':
                    *ptr2++ = '\n';
                    break;

                case 'r':
                    *ptr2++ = '\r';
                    break;

                case 't':
                    *ptr2++ = '\t';
                    break;

                case 'u': /* transcode utf16 to utf8. */
                    sscanf(ptr + 1, "%4x", &uc);
                    ptr += 4; /* get unicode char */

                    /* checks for invalid chars */
                    if (((uc >= 0xDC00) && (uc <= 0xDFFF)) || (uc == 0))
                        break;

                    if ((uc >= 0xD800) && (uc <= 0xDBFF)) {
                        if ((ptr[1] != '\\') || (ptr[2] != 'u'))
                            break;

                        sscanf(ptr + 3, "%4x", &uc2);
                        ptr += 6;

                        if ((uc2 < 0xDC00) || (uc2 > 0xDFFF))
                            break;

                        uc = 0x10000 | ((uc & 0x3FF) << 10) | (uc2 & 0x3FF);
                    }

                    len = 4;

                    if (uc < 0x80)
                        len = 1;
                    else if (uc < 0x800)
                        len = 2;
                    else if (uc < 0x10000)
                        len = 3;

                    ptr2 += len;

                    switch (len) {
                        case 4:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;

                        case 3:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;

                        case 2:
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;

                        case 1:
                            *--ptr2 = (uc | __first_byte_mark[len]);
                    }

                    ptr2 += len;
                    break;

                default:
                    *ptr2++ = *ptr;
                    break;
            }

            ptr++;
        }
    }

    *ptr2 = 0;

    if (*ptr == '\"')
        ptr++;

    n->value = out;
    n->type = CJSON_STRING;

    return ptr;
}

static const char *get_number(const char *num, double *n)
{
    double tmp = 0;

    if (*num == '0')
        num++;

    if ((*num >= '1') && (*num <= '9')) {
        do {
            tmp = (tmp * 10.0) + (*num++ - '0');
        } while ((*num >= '0') && (*num <= '9'));
    }

    *n = tmp;

    return num;
}

static const char *get_float_part(const char *num, double *d, double *scale,
    enum cjson_type *type)
{
    double n = *d, s = 0;

    if ((*num == '.') && (num[1] >= '0') && (num[1] <= '9')) {
        num++;

        do {
            n = (n * 10.0) + (*num++ - '0');
            s--;
        } while ((*num >= '0') && (*num <= '9'));

        *d = n;
        *scale = s;
        *type = CJSON_NUMBER_FLOAT;
    }

    return num;
}

static const char *get_exponent_notation(const char *num, int *sign_subscale,
    int *subscale)
{
    int ss = 1, s = 0;

    if ((*num == 'e') || (*num == 'E')) {
        num++;

        if (*num == '+')
            num++;
        else if (*num == '-') {
            ss = -1;
            num++;
        }

        while ((*num >= '0') && (*num <= '9'))
            s = (s * 10) + (*num++ - '0');

        *sign_subscale = ss;
        *subscale = s;
    }

    return num;
}

static const char *parse_number(cjson_s *j, const char *num)
{
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, sign_subscale = 1;
    enum cjson_type type = CJSON_NUMBER;

    if (*num == '-') {
        sign = -1;
        num++;
    }

    num = get_number(num, &n);
    num = get_float_part(num, &n, &scale, &type);
    num = get_exponent_notation(num, &sign_subscale, &subscale);

    n = sign * n * pow(10.0, (scale + subscale * sign_subscale));

    j->value = cstring_create("%f", n);
    j->type = type;

    return num;
}

static const char *parse_array(cjson_s *j, const char *s)
{
    cjson_s *n;

    if (*s != '[') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    j->type = CJSON_ARRAY;
    s = skip_chars(s + 1);

    if (*s == ']')
        return s + 1; /* empty array */

    n = cjson_new();

    if (NULL == n)
        return NULL;

    s = skip_chars(parse(n, skip_chars(s)));

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    j->child = cdll_unshift(j->child, n);

    while (*s == ',') {
        n = cjson_new();

        if (NULL == n)
            return NULL;

        /* Parse value */
        s = skip_chars(parse(n, skip_chars(s + 1)));

        if (NULL == s) {
            cset_errno(CL_NULL_ARG);
            return NULL;
        }

        j->child = cdll_unshift(j->child, n);
    }

    if (*s == ']')
        return s + 1; /* end of array */

    cset_errno(CL_PARSE_ERROR);

    return NULL;
}

static const char *parse_object(cjson_s *j, const char *s)
{
    cjson_s *n;

    if (*s != '{') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    j->type = CJSON_OBJECT;
    s = skip_chars(s + 1);

    if (*s == '}')
        return s + 1; /* empty */

    n = cjson_new();

    if (NULL == n)
        return NULL;

    /* Parse object name */
    s = skip_chars(parse_string(n, skip_chars(s)));

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    n->name = n->value;
    n->value = NULL;

    if (*s != ':') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    /* Parse object value */
    s = skip_chars(parse(n, skip_chars(s + 1)));

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    j->child = cdll_unshift(j->child, n);

    while (*s == ',') {
        n = cjson_new();

        if (NULL == n)
            return NULL;

        /* Parse object name */
        s = skip_chars(parse_string(n, skip_chars(s + 1)));

        if (NULL == s) {
            cset_errno(CL_NULL_ARG);
            return NULL;
        }

        n->name = n->value;
        n->value = 0;

        if (*s != ':') {
            cset_errno(CL_PARSE_ERROR);
            return NULL;
        }

        /* Parse object value */
        s = skip_chars(parse(n, skip_chars(s + 1)));

        if (NULL == s) {
            cset_errno(CL_NULL_ARG);
            return NULL;
        }

        j->child = cdll_unshift(j->child, n);
    }

    if (*s == '}')
        return s + 1;

    cset_errno(CL_PARSE_ERROR);

    return NULL;
}

static const char *parse(cjson_s *n, const char *s)
{
    unsigned int i;

    if (NULL == s)
        return NULL;

    for (i = 0; i < JVALUES_SIZE; i++)
        if (!strncmp(s, __jvalues[i].value, __jvalues[i].vlen)) {
            n->type = __jvalues[i].type;
            return s + __jvalues[i].vlen;
        }

    switch (*s) {
        case '\"':
            return parse_string(n, s);

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return parse_number(n, s);

        case '[':
            return parse_array(n, s);

        case '{':
            return parse_object(n, s);
    }

    return NULL;
}

cjson_t LIBEXPORT *cjson_parse(const cstring_t *string)
{
    cjson_s *c = NULL;

    cerrno_clear();

    if (validate_object(string, CSTRING) == false)
        return NULL;

    if (cstring_length(string) == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    c = cjson_new();

    if (NULL == c)
        return NULL;

    /* parse */
    if (parse(c, skip_chars(cstring_valueof(string))) == NULL) {
        cjson_delete(c);
        return NULL;
    }

    return c;
}

cjson_t LIBEXPORT *cjson_read_file(const char *filename)
{
    unsigned char *b;
    unsigned int bsize;
    cjson_t *j = NULL;
    cstring_t *s = NULL;

    cerrno_clear();

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    b = cfload(filename, &bsize);

    if (NULL == b)
        return NULL;

    s = cstring_create("%s", b);
    cfunload(b);
    j = cjson_parse(s);
    cstring_destroy(s);

    return j;
}

int LIBEXPORT cjson_write_file(const cjson_t *j, const char *filename)
{
    cstring_t *s;
    int ret;

    cerrno_clear();

    if (validate_object(j, CJSON) == false)
        return -1;

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s = cjson_to_cstring(j, false);

    if (NULL == s)
        return -1;

    ret = cfsave(filename, (unsigned char *)cstring_valueof(s),
                 cstring_length(s));

    cstring_destroy(s);

    return ret;
}

void LIBEXPORT cjson_delete(cjson_t *j)
{
    cjson_s *c = (cjson_s *)j;

    if (validate_object(j, CJSON) == false)
        return;

    cdll_free(c->child, __cjson_delete);
    free(c);
}

int LIBEXPORT cjson_get_array_size(const cjson_t *array)
{
    cjson_s *p = (cjson_s *)array;

    cerrno_clear();

    if (validate_object(array, CJSON) == false)
        return -1;

    if (p->type != CJSON_ARRAY)
        return -1;

    return cdll_size(p->child);
}

cjson_t LIBEXPORT *cjson_get_array_item(const cjson_t *array, unsigned int item)
{
    cjson_s *p = (cjson_s *)array, *n;
    int size;

    cerrno_clear();

    if (validate_object(array, CJSON) == false)
        return NULL;

    size = cjson_get_array_size(array);

    if ((size < 0) || ((int)item >= size))
        return NULL;

    n = cdll_at(p->child, item);

    return n;
}

static int find_object(void *a, void *b)
{
    cjson_s *p = (cjson_s *)a;
    char *name = (char *)b;

    if (strcmp(cstring_valueof(p->name), name) == 0)
        return 1;

    return 0;
}

cjson_t LIBEXPORT *cjson_get_object_item(const cjson_t *json, const char *name)
{
    cjson_s *p = NULL, *root = (cjson_s *)json;

    cerrno_clear();

    if (validate_object(json, CJSON) == false)
        return NULL;

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cdll_map(root->child, find_object, (char *)name);

    return p;
}

cstring_t LIBEXPORT *cjson_get_object_name(const cjson_t *o)
{
    cjson_s *p = (cjson_s *)o;

    cerrno_clear();

    if (validate_object(o, CJSON) == false)
        return NULL;

    return p->name;
}

cstring_t LIBEXPORT *cjson_get_object_value(const cjson_t *o)
{
    cjson_s *p = (cjson_s *)o;

    cerrno_clear();

    if (validate_object(o, CJSON) == false)
        return NULL;

    return p->value;
}

enum cjson_type LIBEXPORT cjson_get_object_type(const cjson_t *o)
{
    cjson_s *p = (cjson_s *)o;

    cerrno_clear();

    if (validate_object(o, CJSON) == false)
        return -1;

    return p->type;
}

static int __cjson_dup(void *a, void *b)
{
    cjson_s *node = (cjson_s *)a;
    cjson_s *list = (cjson_s *)b;
    cjson_s *p;

    p = __dup(node);
    p->type = node->type & (~CJSON_IS_REFERENCE);

    if (node->child != NULL)
        cdll_map(node->child, __cjson_dup, p);

    list->child = cdll_unshift(list->child, p);

    return 0;
}

cjson_t LIBEXPORT *cjson_dup(const cjson_t *root)
{
    cjson_s *r = (cjson_s *)root;
    cjson_s *l = NULL;

    cerrno_clear();

    if (validate_object(root, CJSON) == false)
        return NULL;

    l = __dup(r);
    l->type = r->type & (~CJSON_IS_REFERENCE);
    cdll_map(r->child, __cjson_dup, l);

    return l;
}

cjson_t LIBEXPORT *cjson_create_array(void)
{
    cjson_s *p = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = CJSON_ARRAY;

    return p;
}

cjson_t LIBEXPORT *cjson_create_object(void)
{
    cjson_s *p = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = CJSON_OBJECT;

    return p;
}

cjson_t LIBEXPORT *cjson_create_null(void)
{
    cjson_s *p = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = CJSON_NULL;

    return p;
}

cjson_t LIBEXPORT *cjson_create_false(void)
{
    cjson_s *p = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = CJSON_FALSE;

    return p;
}

cjson_t LIBEXPORT *cjson_create_true(void)
{
    cjson_s *p = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    p->type = CJSON_TRUE;

    return p;
}

cjson_t LIBEXPORT *cjson_create_number(int n)
{
    cjson_s *p = NULL;
    cstring_t *s = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    s = cstring_create("%d", n);
    p->type = CJSON_NUMBER;
    p->value = s;

    return p;
}

cjson_t LIBEXPORT *cjson_create_number_float(float n)
{
    cjson_s *p = NULL;
    cstring_t *s = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    s = cstring_create("%f", n);
    p->type = CJSON_NUMBER;
    p->value = s;

    return p;
}

cjson_t LIBEXPORT *cjson_create_string(const char *string)
{
    cjson_s *p = NULL;
    cstring_t *s = NULL;

    cerrno_clear();
    p = cjson_new();

    if (NULL == p)
        return NULL;

    s = cstring_create("%s", string);
    p->type = CJSON_STRING;
    p->value = s;

    return p;
}

cjson_t LIBEXPORT *cjson_create_int_array(const int *values, int size)
{
    int i;
    cjson_s *a = NULL, *n;

    cerrno_clear();
    a = cjson_create_array();

    if (NULL == a)
        return NULL;

    for (i = 0; i < size; i++) {
        n = cjson_create_number(values[i]);

        if (NULL == n)
            return NULL;

        a->child = cdll_unshift(a->child, n);
    }

    return a;
}

cjson_t LIBEXPORT *cjson_create_float_array(const float *values, int size)
{
    int i;
    cjson_s *a = NULL, *n;

    cerrno_clear();
    a = cjson_create_array();

    if (NULL == a)
        return NULL;

    for (i = 0; i < size; i++) {
        n = cjson_create_number_float(values[i]);

        if (NULL == n)
            return NULL;

        a->child = cdll_unshift(a->child, n);
    }

    return a;
}

cjson_t LIBEXPORT *cjson_create_string_array(const cstring_list_t *values)
{
    int i, size;
    cjson_s *a = NULL, *n;

    cerrno_clear();

    if (validate_object(values, CSTRINGLIST) == false)
        return NULL;

    a = cjson_create_array();

    if (NULL == a)
        return NULL;

    size = cstring_list_size(values);

    for (i = 0; i < size; i++) {
        n = cjson_create_string(cstring_valueof(cstring_list_get(values, i)));

        if (NULL == n)
            return NULL;

        a->child = cdll_unshift(a->child, n);
    }

    return a;
}

int LIBEXPORT cjson_add_item_to_array(cjson_t *array, const cjson_t *item)
{
    cjson_s *a = (cjson_s *)array;
    cjson_s *n = (cjson_s *)item;

    cerrno_clear();

    if ((validate_object(array, CJSON) == false) ||
        (validate_object(item, CJSON) == false))
    {
        return -1;
    }

    a->child = cdll_unshift(a->child, n);

    return 0;
}

int LIBEXPORT cjson_add_item_to_object(cjson_t *root, const char *name,
    cjson_t *item)
{
    cjson_s *r = (cjson_s *)root;
    cjson_s *n = (cjson_s *)item;

    cerrno_clear();

    if ((validate_object(root, CJSON) == false) ||
        (validate_object(item, CJSON) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (n->name != NULL)
        cstring_destroy(n->name);

    n->name = cstring_create("%s", name);
    r->child = cdll_unshift(r->child, n);

    return 0;
}

static cjson_t *create_reference(cjson_s *item)
{
    cjson_s *ref = NULL;

    ref = __dup(item);

    cstring_destroy(ref->name);
    ref->type |= CJSON_IS_REFERENCE;
    ref->next = ref->prev = 0;

    return ref;
}

int LIBEXPORT cjson_add_item_reference_to_array(cjson_t *array, cjson_t *item)
{
    cerrno_clear();

    if ((validate_object(array, CJSON) == false) ||
        (validate_object(item, CJSON) == false))
    {
        return -1;
    }

    return cjson_add_item_to_array(array, create_reference(item));
}

int LIBEXPORT cjson_add_item_reference_to_object(cjson_t *root, const char *name,
    cjson_t *item)
{
    cerrno_clear();

    if ((validate_object(root, CJSON) == false) ||
        (validate_object(item, CJSON) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return cjson_add_item_to_object(root, name, create_reference(item));
}

int LIBEXPORT cjson_delete_item_from_array(cjson_t *array, unsigned int index)
{
    cjson_s *p, *root = (cjson_s *)array;
    int size;

    cerrno_clear();

    if (validate_object(array, CJSON) == false)
        return -1;

    size = cjson_get_array_size(array);

    if ((size < 0) || ((int)index >= size))
        return -1;

    p = cdll_delete_indexed(&root, index, NULL);

    if (NULL == p)
        return -1;

    cjson_delete(p);

    return 0;
}

int LIBEXPORT cjson_delete_item_from_object(cjson_t *json, const char *name)
{
    cjson_s *p = NULL, *root = (cjson_s *)json;

    cerrno_clear();

    if (validate_object(json, CJSON) == false)
        return -1;

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    p = cdll_filter(&root->child, find_object, (char *)name);

    if (NULL == p)
        return -1;

    cjson_delete(p);

    return 0;
}

int LIBEXPORT cjson_replace_item_in_array(cjson_t *array, unsigned int index,
    cjson_t *new_item)
{
    cjson_s *a = (cjson_s *)array;
    cjson_s *p = NULL, *n;
    unsigned int i = 0;

    cerrno_clear();

    if ((validate_object(array, CJSON) == false) ||
        (validate_object(new_item, CJSON) == false))
    {
        return -1;
    }

    if (index >= (unsigned int)cdll_size(a->child)) {
        return -1;
    }

    while (a->child) {
        n = cdll_pop(&a->child);

        if (i == index) {
            cjson_delete(n);
            n = new_item;
        }

        p = cdll_unshift(p, n);
        i++;
    }

    if (p != NULL)
        a->child = p;

    return 0;
}

int LIBEXPORT cjson_replace_item_in_object(cjson_t *root, const char *name,
    cjson_t *new_item)
{
    cstring_t *tmp = NULL;
    cjson_s *a = (cjson_s *)root;
    cjson_s *p = NULL, *n;

    cerrno_clear();

    if ((validate_object(root, CJSON) == false) ||
        (validate_object(new_item, CJSON) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    tmp = cstring_create("%s", name);

    while (a->child) {
        n = cdll_pop(&a->child);

        if (cstring_cmp(n->name, tmp) == 0) {
            cjson_delete(n);
            n = new_item;
        }

        p = cdll_unshift(p, n);
    }

    if (p != NULL)
        a->child = p;

    return 0;
}

static char *print_number(cjson_s *item)
{
    cstring_t *value = NULL;
    char *str = NULL;
    double d;
    int i, b = 0;

    value = cjson_get_object_value(item);
    d = cstring_to_double(value);
    i = cstring_to_int(value);

    if ((fabs((double)i - d) <= DBL_EPSILON) &&
        (d <= INT_MAX) &&
        (d >= INT_MIN))
    {
        b = asprintf(&str, "%d", i);
    } else {
        if (fabs(floor(d) - d) <= DBL_EPSILON)
            b = asprintf(&str, "%.0f", d);
        else if ((fabs(d) < 1.0e-6) || (fabs(d) > 1.0e9))
            b = asprintf(&str, "%e", d);
        else
            b = asprintf(&str, "%f", d);
    }

    if (b == -1)
        return NULL;

    return str;
}

static char *print_string(const cstring_t *value)
{
    const char *ptr;
    char *ptr2, *out;
    int len = 0;
    unsigned char token;

    if (NULL == value)
        return strdup("");

    ptr = cstring_valueof(value);

    while ((token = *ptr) && ++len) {
        if (strchr("\"\\\b\f\n\r\t", token))
            len++;
        else if (token < 32)
            len += 5;

        ptr++;
    }

    out = calloc(len + 3, sizeof(char));

    if (!out) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    ptr2 = out;
    ptr = cstring_valueof(value);
    *ptr2++ = '\"';

    while (*ptr) {
        if (((unsigned char)*ptr > 31) && (*ptr != '\"') && (*ptr != '\\'))
            *ptr2++ = *ptr++;
        else {
            *ptr2++ = '\\';

            switch (token = *ptr++) {
                case '\\':
                    *ptr2++ = '\\';
                    break;

                case '\"':
                    *ptr2++ = '\"';
                    break;

                case '\b':
                    *ptr2++ = 'b';
                    break;

                case '\f':
                    *ptr2++ = 'f';
                    break;

                case '\n':
                    *ptr2++ = 'n';
                    break;

                case '\r':
                    *ptr2++ = 'r';
                    break;

                case '\t':
                    *ptr2++ = 't';
                    break;

                default:
                    sprintf(ptr2, "u%04x", token);
                    ptr2 += 5;
                    break;
            }
        }
    }

    *ptr2++ = '\"';
    *ptr2++ = 0;

    return out;
}

static cstring_t *output_array(cstring_list_t *sl, bool fmt)
{
    cstring_t *out = NULL, *v;
    int i;

    /* Output */
    out = cstring_create("[");

    for (i = 0; i < cstring_list_size(sl); i++) {
        v = cstring_list_get(sl, i);
        cstring_cat(out, "%s", cstring_valueof(v));
        cstring_unref(v);

        if (i != (cstring_list_size(sl) - 1)) {
            cstring_cat(out, ",");

            if (fmt == true)
                cstring_cat(out, " ");
        }
    }

    cstring_cat(out, "]");

    return out;
}

static char *print_array(cjson_s *item, int depth, bool fmt)
{
    char *ptr;
    cstring_list_t *sl = NULL;
    cstring_t *v = NULL;
    cjson_s *child = item->child;

    sl = cstring_list_create();

    /* Get child values */
    while (child) {
        ptr = print_value(child, depth + 1, fmt);

        if (NULL == ptr) {
            cstring_list_destroy(sl);
            return NULL;
        }

        v = cstring_create("%s", ptr);
        cstring_list_add(sl, v);
        cstring_unref(v);
        free(ptr);
        child = child->next;
    }

    v = output_array(sl, fmt);
    ptr = strdup(cstring_valueof(v));
    cstring_destroy(v);
    cstring_list_destroy(sl);

    return ptr;
}

static cstring_t *output_object(cstring_list_t *sl_names,
    cstring_list_t *sl_values, int depth, bool fmt)
{
    cstring_t *out = NULL, *v;
    int i, j;

    out = cstring_create("{");

    if (fmt == true)
        cstring_cat(out, "\n");

    /* Both lists must have the same sizes */
    for (i = 0; i < cstring_list_size(sl_names); i++) {
        if (fmt == true)
            for (j = 0; j < depth; j++)
                cstring_cat(out, "\t");

        v = cstring_list_get(sl_names, i);
        cstring_cat(out, "%s", cstring_valueof(v));
        cstring_unref(v);
        cstring_cat(out, ":");

        if (fmt == true)
            cstring_cat(out, "\t");

        v = cstring_list_get(sl_values, i);
        cstring_cat(out, "%s", cstring_valueof(v));
        cstring_unref(v);

        if (i != (cstring_list_size(sl_names) - 1))
            cstring_cat(out, ",");

        if (fmt == true)
            cstring_cat(out, "\n");
    }

    if (fmt == true)
        for (i = 0; i < depth; i++)
            cstring_cat(out, "\t");

    cstring_cat(out, "}");

    return out;
}

static char *print_object(cjson_s *item, int depth, bool fmt)
{
    char *ptr;
    cjson_s *child = item->child;
    cstring_list_t *sl_names = NULL, *sl_values = NULL;
    cstring_t *v;

    sl_names = cstring_list_create();
    sl_values = cstring_list_create();
    depth++;

    while (child) {
        ptr = print_string(child->name);

        if (NULL == ptr)
            goto end_block;

        v = cstring_create("%s", ptr);
        cstring_list_add(sl_names, v);
        cstring_unref(v);
        free(ptr);

        ptr = print_value(child, depth, fmt);

        if (NULL == ptr)
            goto end_block;

        v = cstring_create("%s", ptr);
        cstring_list_add(sl_values, v);
        cstring_unref(v);
        free(ptr);

        child = child->next;
    }

    v = output_object(sl_names, sl_values, depth, fmt);
    ptr = strdup(cstring_valueof(v));
    cstring_unref(v);

end_block:
    cstring_list_destroy(sl_names);
    cstring_list_destroy(sl_values);

    return ptr;

}

static char *print_value(cjson_s *j, int depth, bool fmt)
{
    char *p = NULL;
    int type;

    type = (j->type) & 255;

    switch (type) {
        case CJSON_NULL:
        case CJSON_FALSE:
        case CJSON_TRUE:
            p = strdup(get_jvalue_value(type));
            break;

        case CJSON_NUMBER:
        case CJSON_NUMBER_FLOAT:
            p = print_number(j);
            break;

        case CJSON_STRING:
            p = print_string(j->value);
            break;

        case CJSON_ARRAY:
            p = print_array(j, depth, fmt);
            break;

        case CJSON_OBJECT:
            p = print_object(j, depth, fmt);
            break;
    }

    return p;
}

cstring_t LIBEXPORT *cjson_to_cstring(const cjson_t *j, bool friendly_output)
{
    char *p = NULL;
    cstring_t *out;

    cerrno_clear();

    if (validate_object(j, CJSON) == false)
        return NULL;

    p = print_value((cjson_s *)j, 0, friendly_output);

    if (NULL == p)
        return NULL;

    out = cstring_create("%s", p);
    free(p);

    return out;
}

