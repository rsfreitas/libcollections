
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
#include <stdarg.h>

#include <math.h>

#include "collections.h"

#define CL_JSON_IS_REFERENCE              256

typedef struct cl_json_s {
    cl_list_entry_t         *prev;
    cl_list_entry_t         *next;
    struct cl_object_hdr    hdr;
    enum cl_json_type       type;
    cl_string_t             *name;
    cl_string_t             *value;
    void                    *child;
} cl_json_s;

#define CL_JSON_OBJECT_OFFSET         \
    (sizeof(cl_list_entry_t *) + sizeof(cl_list_entry_t *))

struct jvalue_s {
    char                *value;
    size_t              vlen;
    enum cl_json_type   type;
};

static const char *parse(cl_json_s *n, const char *s);
static char *print_value(cl_json_s *j, int depth, bool fmt);

struct jvalue_s __jvalues[] = {
    { "null",   4,  CL_JSON_NULL },
    { "false",  5,  CL_JSON_FALSE },
    { "true",   4,  CL_JSON_TRUE }
};

#define JVALUES_SIZE            \
    (sizeof(__jvalues) / (sizeof(__jvalues[0])))

static const unsigned char __first_byte_mark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

/*
 *
 * Internal functions
 *
 */

static const char *get_jvalue_value(enum cl_json_type type)
{
    unsigned int i;

    for (i = 0; i < JVALUES_SIZE; i++)
        if (__jvalues[i].type == type)
            return __jvalues[i].value;

    return NULL;
}

static cl_json_s *cl_json_new(void)
{
    cl_json_s *j = NULL;

    j = calloc(1, sizeof(cl_json_s));

    if (NULL == j) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    j->child = NULL;
    typeof_set_with_offset(CL_OBJ_JSON, j, CL_JSON_OBJECT_OFFSET);

    return j;
}

static cl_json_s *__dup(const cl_json_s *j)
{
    cl_json_s *p = NULL;

    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = j->type;
    p->value = (j->value != NULL) ? cl_string_dup(j->value) : NULL;
    p->name = (j->name != NULL) ? cl_string_dup(j->name) : NULL;

    return p;
}

static void __cl_json_delete(void *a)
{
    cl_json_s *c = (cl_json_s *)a;

    if (!(c->type & CL_JSON_IS_REFERENCE) && c->child) {
        cl_dll_free(c->child, __cl_json_delete);
        c->child = NULL;
    }

    if (!(c->type & CL_JSON_IS_REFERENCE) && c->value) {
        cl_string_destroy(c->value);
        c->value = NULL;
    }

    if (c->name) {
        cl_string_destroy(c->name);
        c->name = NULL;
    }

    free(c);
    c = NULL;
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

/*
 * Here we skip all lines beginning with a comment char.
 */
static const char *skip_comment(const char *s)
{
    while (*s == '#') {
        while (s && *s && (*s != '\n'))
            s++;

        s = skip_chars(s);
    }

    return s;
}

static const char *parse_string(cl_json_s *n, const char *s)
{
    const char *ptr;
    char *ptr2;
    int len = 0;
    unsigned uc, uc2;
    cl_string_t *out;

    /* Ignore comment lines */
    if (*s == '#')
        s = skip_chars(skip_comment(s));

    if (*s != '\"') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    ptr = s + 1;
    len = get_string_length(s);
    out = cl_string_create_empty(len + 1);

    if (NULL == out)
        return NULL;

    ptr2 = (char *)cl_string_valueof(out);

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
    cl_string_update_length(n->value);
    n->type = CL_JSON_STRING;

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
    enum cl_json_type *type)
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
        *type = CL_JSON_NUMBER_FLOAT;
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

static const char *parse_number(cl_json_s *j, const char *num)
{
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, sign_subscale = 1;
    enum cl_json_type type = CL_JSON_NUMBER;

    if (*num == '-') {
        sign = -1;
        num++;
    }

    num = get_number(num, &n);
    num = get_float_part(num, &n, &scale, &type);

    if (type == CL_JSON_NUMBER_FLOAT) {
        num = get_exponent_notation(num, &sign_subscale, &subscale);
        n = sign * n * pow(10.0, (scale + subscale * sign_subscale));
        j->value = cl_string_create("%f", n);
    } else {
        n = sign * n;
        j->value = cl_string_create("%d", (int)n);
    }

    j->type = type;

    return num;
}

static const char *parse_array(cl_json_s *j, const char *s)
{
    cl_json_s *n;

    if (*s != '[') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    j->type = CL_JSON_ARRAY;
    s = skip_chars(s + 1);

    if (*s == ']')
        return s + 1; /* empty array */

    n = cl_json_new();

    if (NULL == n)
        return NULL;

    s = skip_chars(parse(n, skip_chars(s)));

    if (NULL == s) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    j->child = cl_dll_unshift(j->child, n);

    while (*s == ',') {
        n = cl_json_new();

        if (NULL == n)
            return NULL;

        /* Parse value */
        s = skip_chars(parse(n, skip_chars(s + 1)));

        if (NULL == s) {
            cset_errno(CL_NULL_ARG);
            return NULL;
        }

        j->child = cl_dll_unshift(j->child, n);
    }

    if (*s == ']')
        return s + 1; /* end of array */

    cset_errno(CL_PARSE_ERROR);

    return NULL;
}

static const char *parse_object(cl_json_s *j, const char *s)
{
    cl_json_s *n;

    if (*s != '{') {
        cset_errno(CL_PARSE_ERROR);
        return NULL;
    }

    j->type = CL_JSON_OBJECT;
    s = skip_chars(s + 1);

    if (*s == '}')
        return s + 1; /* empty */

    n = cl_json_new();

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

    j->child = cl_dll_unshift(j->child, n);

    while (*s == ',') {
        n = cl_json_new();

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

        j->child = cl_dll_unshift(j->child, n);
    }

    if (*s == '}')
        return s + 1;

    cset_errno(CL_PARSE_ERROR);

    return NULL;
}

static const char *parse(cl_json_s *n, const char *s)
{
    unsigned int i;

    if (NULL == s)
        return NULL;

    for (i = 0; i < JVALUES_SIZE; i++)
        if (!strncmp(s, __jvalues[i].value, __jvalues[i].vlen)) {
            n->type = __jvalues[i].type;
            return s + __jvalues[i].vlen;
        }

    /* Ignore comment lines */
    if (*s == '#')
        s = skip_chars(skip_comment(s));

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

static int find_object(void *a, void *b)
{
    cl_json_s *p = (cl_json_s *)a;
    char *name = (char *)b;

    if (strcmp(cl_string_valueof(p->name), name) == 0)
        return 1;

    return 0;
}

static int __cl_json_dup(void *a, void *b)
{
    cl_json_s *node = (cl_json_s *)a;
    cl_json_s *list = (cl_json_s *)b;
    cl_json_s *p;

    p = __dup(node);
    p->type = node->type & (~CL_JSON_IS_REFERENCE);

    if (node->child != NULL)
        cl_dll_map(node->child, __cl_json_dup, p);

    list->child = cl_dll_unshift(list->child, p);

    return 0;
}

static cl_json_t *create_reference(cl_json_s *item)
{
    cl_json_s *ref = NULL;

    ref = __dup(item);

    cl_string_destroy(ref->name);
    ref->type |= CL_JSON_IS_REFERENCE;
    ref->next = ref->prev = 0;

    return ref;
}

static char *print_number(cl_json_s *item)
{
    cl_string_t *value = NULL;
    char *str = NULL;
    double d;
    int i, b = 0;

    value = cl_json_get_object_value(item);
    d = cl_string_to_double(value);
    i = cl_string_to_int(value);

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

static char *print_string(const cl_string_t *value)
{
    const char *ptr;
    char *ptr2, *out;
    int len = 0;
    unsigned char token;

    if (NULL == value)
        return strdup("");

    ptr = cl_string_valueof(value);

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
    ptr = cl_string_valueof(value);
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

static cl_string_t *output_array(cl_stringlist_t *sl, bool fmt)
{
    cl_string_t *out = NULL, *v;
    int i;

    /* Output */
    out = cl_string_create("[");

    for (i = 0; i < cl_stringlist_size(sl); i++) {
        v = cl_stringlist_get(sl, i);
        cl_string_cat(out, "%s", cl_string_valueof(v));
        cl_string_unref(v);

        if (i != (cl_stringlist_size(sl) - 1)) {
            cl_string_cat(out, ",");

            if (fmt == true)
                cl_string_cat(out, " ");
        }
    }

    cl_string_cat(out, "]");

    return out;
}

static char *print_array(cl_json_s *item, int depth, bool fmt)
{
    char *ptr;
    cl_stringlist_t *sl = NULL;
    cl_string_t *v = NULL;
    cl_json_s *child = item->child;

    sl = cl_stringlist_create();

    /* Get child values */
    while (child) {
        ptr = print_value(child, depth + 1, fmt);

        if (NULL == ptr) {
            cl_stringlist_destroy(sl);
            return NULL;
        }

        v = cl_string_create("%s", ptr);
        cl_stringlist_add(sl, v);
        cl_string_unref(v);
        free(ptr);
        child = child->next;
    }

    v = output_array(sl, fmt);
    ptr = strdup(cl_string_valueof(v));
    cl_string_destroy(v);
    cl_stringlist_destroy(sl);

    return ptr;
}

static cl_string_t *output_object(cl_stringlist_t *sl_names,
    cl_stringlist_t *sl_values, int depth, bool fmt)
{
    cl_string_t *out = NULL, *v;
    int i, j;

    out = cl_string_create("{");

    if (fmt == true)
        cl_string_cat(out, "\n");

    /* Both lists must have the same sizes */
    for (i = 0; i < cl_stringlist_size(sl_names); i++) {
        if (fmt == true)
            for (j = 0; j < depth; j++)
                cl_string_cat(out, "\t");

        v = cl_stringlist_get(sl_names, i);
        cl_string_cat(out, "%s", cl_string_valueof(v));
        cl_string_unref(v);
        cl_string_cat(out, ":");

        if (fmt == true)
            cl_string_cat(out, "\t");

        v = cl_stringlist_get(sl_values, i);
        cl_string_cat(out, "%s", cl_string_valueof(v));
        cl_string_unref(v);

        if (i != (cl_stringlist_size(sl_names) - 1))
            cl_string_cat(out, ",");

        if (fmt == true)
            cl_string_cat(out, "\n");
    }

    if (fmt == true)
        for (i = 0; i < depth; i++)
            cl_string_cat(out, "\t");

    cl_string_cat(out, "}");

    return out;
}

static char *print_object(cl_json_s *item, int depth, bool fmt)
{
    char *ptr;
    cl_json_s *child = item->child;
    cl_stringlist_t *sl_names = NULL, *sl_values = NULL;
    cl_string_t *v;

    sl_names = cl_stringlist_create();
    sl_values = cl_stringlist_create();
    depth++;

    while (child) {
        ptr = print_string(child->name);

        if (NULL == ptr)
            goto end_block;

        v = cl_string_create("%s", ptr);
        cl_stringlist_add(sl_names, v);
        cl_string_unref(v);
        free(ptr);

        ptr = print_value(child, depth, fmt);

        if (NULL == ptr)
            goto end_block;

        v = cl_string_create("%s", ptr);
        cl_stringlist_add(sl_values, v);
        cl_string_unref(v);
        free(ptr);

        child = child->next;
    }

    v = output_object(sl_names, sl_values, depth, fmt);
    ptr = strdup(cl_string_valueof(v));
    cl_string_unref(v);

end_block:
    cl_stringlist_destroy(sl_names);
    cl_stringlist_destroy(sl_values);

    return ptr;
}

static char *print_value(cl_json_s *j, int depth, bool fmt)
{
    char *p = NULL;
    int type;

    type = (j->type) & 255;

    switch (type) {
    case CL_JSON_NULL:
    case CL_JSON_FALSE:
    case CL_JSON_TRUE:
        p = strdup(get_jvalue_value(type));
        break;

    case CL_JSON_NUMBER:
    case CL_JSON_NUMBER_FLOAT:
        p = print_number(j);
        break;

    case CL_JSON_STRING:
        p = print_string(j->value);
        break;

    case CL_JSON_ARRAY:
        p = print_array(j, depth, fmt);
        break;

    case CL_JSON_OBJECT:
        p = print_object(j, depth, fmt);
        break;
    }

    return p;
}

/*
 *
 * API
 *
 */

cl_json_t *cl_json_parse_string(const char *string)
{
    cl_json_s *c = NULL;
    int error;

    __clib_function_init__(false, NULL, -1, NULL);

    if (strlen(string) == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    c = cl_json_new();

    if (NULL == c)
        return NULL;

    /* parse */
    if (parse(c, skip_chars(string)) == NULL) {
        error = cl_get_last_error();
        cl_json_delete(c);
        cset_errno(error);
        return NULL;
    }

    return c;
}

cl_json_t *cl_json_parse(const cl_string_t *string)
{
    __clib_function_init__(true, string, CL_OBJ_STRING, NULL);

    return cl_json_parse_string(cl_string_valueof(string));
}

cl_json_t *cl_json_read_file(const char *filename)
{
    unsigned char *b;
    unsigned int bsize;
    cl_json_t *j = NULL;
    cl_string_t *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    b = cl_fload(filename, &bsize);

    if (NULL == b)
        return NULL;

    s = cl_string_create("%s", b);
    cl_funload(b);
    j = cl_json_parse(s);
    cl_string_destroy(s);

    return j;
}

int cl_json_write_file(const cl_json_t *j, const char *filename)
{
    cl_string_t *s;
    int ret;

    __clib_function_init_ex__(true, j, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET, -1);

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    s = cl_json_to_cstring(j, false);

    if (NULL == s)
        return -1;

    ret = cl_fsave(filename, (unsigned char *)cl_string_valueof(s),
                   cl_string_length(s));

    cl_string_destroy(s);

    return ret;
}

void cl_json_delete(cl_json_t *j)
{
    cl_json_s *c = (cl_json_s *)j;

    if (library_initialized() == false)
        return;

    if (typeof_validate_object_with_offset(j, CL_OBJ_JSON,
                                    CL_JSON_OBJECT_OFFSET) == false)
    {
        return;
    }

    cl_dll_free(c->child, __cl_json_delete);

    if (!(c->type & CL_JSON_IS_REFERENCE) && c->value) {
        cl_string_destroy(c->value);
        c->value = NULL;
    }

    if (c->name) {
        cl_string_destroy(c->name);
        c->name = NULL;
    }

    free(c);
}

int cl_json_get_array_size(const cl_json_t *array)
{
    cl_json_s *p = (cl_json_s *)array;

    __clib_function_init_ex__(true, array, CL_OBJ_JSON,
                              CL_JSON_OBJECT_OFFSET, -1);

    if (p->type != CL_JSON_ARRAY)
        return -1;

    return cl_dll_size(p->child);
}

cl_json_t *cl_json_get_array_item(const cl_json_t *array,
    unsigned int item)
{
    cl_json_s *p = (cl_json_s *)array, *n;
    int size;

    __clib_function_init_ex__(true, array, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    size = cl_json_get_array_size(array);

    if ((size < 0) || ((int)item >= size))
        return NULL;

    n = cl_dll_at(p->child, item);

    return n;
}

cl_json_t *cl_json_get_array_item_by_name(const cl_json_t *array,
    const char *name)
{
    cl_json_s *p = (cl_json_s *)array, *n;
    int size;

    __clib_function_init_ex__(true, array, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    size = cl_json_get_array_size(array);

    if (size <= 0)
        return NULL;

    n = cl_dll_map(p->child, find_object, (char *)name);

    return n;
}

cl_json_t *cl_json_get_object_item(const cl_json_t *json,
    const char *name)
{
    cl_json_s *p = NULL, *root = (cl_json_s *)json;

    __clib_function_init_ex__(true, json, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    p = cl_dll_map(root->child, find_object, (char *)name);

    return p;
}

cl_string_t *cl_json_get_object_name(const cl_json_t *o)
{
    cl_json_s *p = (cl_json_s *)o;

    __clib_function_init_ex__(true, o, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    return p->name;
}

cl_string_t *cl_json_get_object_value(const cl_json_t *o)
{
    cl_json_s *p = (cl_json_s *)o;

    __clib_function_init_ex__(true, o, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    return p->value;
}

enum cl_json_type cl_json_get_object_type(const cl_json_t *o)
{
    cl_json_s *p = (cl_json_s *)o;

    __clib_function_init_ex__(true, o, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET, -1);

    return p->type;
}

cl_json_t *cl_json_dup(const cl_json_t *root)
{
    cl_json_s *r = (cl_json_s *)root;
    cl_json_s *l = NULL;

    __clib_function_init_ex__(true, root, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    l = __dup(r);
    l->type = r->type & (~CL_JSON_IS_REFERENCE);
    cl_dll_map(r->child, __cl_json_dup, l);

    return l;
}

cl_json_t *cl_json_create_array(void)
{
    cl_json_s *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = CL_JSON_ARRAY;

    return p;
}

cl_json_t *cl_json_create_object(void)
{
    cl_json_s *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = CL_JSON_OBJECT;

    return p;
}

cl_json_t *cl_json_create_null(void)
{
    cl_json_s *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = CL_JSON_NULL;

    return p;
}

cl_json_t *cl_json_create_false(void)
{
    cl_json_s *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = CL_JSON_FALSE;

    return p;
}

cl_json_t *cl_json_create_true(void)
{
    cl_json_s *p = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    p->type = CL_JSON_TRUE;

    return p;
}

cl_json_t *cl_json_create_number(int n)
{
    cl_json_s *p = NULL;
    cl_string_t *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    s = cl_string_create("%d", n);
    p->type = CL_JSON_NUMBER;
    p->value = s;

    return p;
}

cl_json_t *cl_json_create_number_float(float n)
{
    cl_json_s *p = NULL;
    cl_string_t *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    s = cl_string_create("%f", n);
    p->type = CL_JSON_NUMBER;
    p->value = s;

    return p;
}

cl_json_t *cl_json_create_string(const char *string)
{
    cl_json_s *p = NULL;
    cl_string_t *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    s = cl_string_create("%s", string);
    p->type = CL_JSON_STRING;
    p->value = s;

    return p;
}

cl_json_t *cl_json_create_node(enum cl_json_type type,
    const char *fmt, ...)
{
    cl_json_s *p = NULL;
    va_list ap;
    char *buff;
    cl_string_t *s = NULL;

    __clib_function_init__(false, NULL, -1, NULL);
    p = cl_json_new();

    if (NULL == p)
        return NULL;

    va_start(ap, fmt);
    vasprintf(&buff, fmt, ap);
    va_end(ap);

    s = cl_string_create_empty(0);
    cl_string_set_content(s, buff);
    p->type = type;
    p->value = s;

    return p;
}

cl_json_t *cl_json_create_int_array(const int *values, int size)
{
    int i;
    cl_json_s *a = NULL, *n;

    __clib_function_init__(false, NULL, -1, NULL);
    a = cl_json_create_array();

    if (NULL == a)
        return NULL;

    for (i = 0; i < size; i++) {
        n = cl_json_create_number(values[i]);

        if (NULL == n)
            return NULL;

        a->child = cl_dll_unshift(a->child, n);
    }

    return a;
}

cl_json_t *cl_json_create_float_array(const float *values, int size)
{
    int i;
    cl_json_s *a = NULL, *n;

    __clib_function_init__(false, NULL, -1, NULL);
    a = cl_json_create_array();

    if (NULL == a)
        return NULL;

    for (i = 0; i < size; i++) {
        n = cl_json_create_number_float(values[i]);

        if (NULL == n)
            return NULL;

        a->child = cl_dll_unshift(a->child, n);
    }

    return a;
}

cl_json_t *cl_json_create_string_array(const cl_stringlist_t *values)
{
    int i, size;
    cl_json_s *a = NULL, *n;

    __clib_function_init__(true, values, CL_OBJ_STRINGLIST, NULL);
    a = cl_json_create_array();

    if (NULL == a)
        return NULL;

    size = cl_stringlist_size(values);

    for (i = 0; i < size; i++) {
        n = cl_json_create_string(cl_string_valueof(cl_stringlist_get(values,
                                                                       i)));

        if (NULL == n)
            return NULL;

        a->child = cl_dll_unshift(a->child, n);
    }

    return a;
}

int cl_json_add_item_to_array(cl_json_t *array, const cl_json_t *item)
{
    cl_json_s *a = (cl_json_s *)array;
    cl_json_s *n = (cl_json_s *)item;

    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(array, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(item, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    a->child = cl_dll_unshift(a->child, n);

    return 0;
}

int cl_json_add_item_to_object(cl_json_t *root, const char *name,
    cl_json_t *item)
{
    cl_json_s *r = (cl_json_s *)root;
    cl_json_s *n = (cl_json_s *)item;

    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(root, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(item, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (n->name != NULL)
        cl_string_destroy(n->name);

    n->name = cl_string_create("%s", name);
    r->child = cl_dll_unshift(r->child, n);

    return 0;
}

int cl_json_add_item_reference_to_array(cl_json_t *array,
    cl_json_t *item)
{
    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(array, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(item, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    return cl_json_add_item_to_array(array, create_reference(item));
}

int cl_json_add_item_reference_to_object(cl_json_t *root,
    const char *name, cl_json_t *item)
{
    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(root, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(item, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return cl_json_add_item_to_object(root, name, create_reference(item));
}

int cl_json_delete_item_from_array(cl_json_t *array,
    unsigned int index)
{
    cl_json_s *p, *root = (cl_json_s *)array;
    int size;

    __clib_function_init_ex__(true, array, CL_OBJ_JSON,
                              CL_JSON_OBJECT_OFFSET, -1);

    size = cl_json_get_array_size(array);

    if ((size < 0) || ((int)index >= size))
        return -1;

    p = cl_dll_delete_indexed(&root, index, NULL);

    if (NULL == p)
        return -1;

    cl_json_delete(p);

    return 0;
}

int cl_json_delete_item_from_array_by_name(const cl_json_t *array,
    const char *name)
{
    cl_json_s *p = (cl_json_s *)array, *n;
    int size;

    __clib_function_init_ex__(true, array, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              -1);

    size = cl_json_get_array_size(array);

    if (size <= 0)
        return -1;

    n = cl_dll_filter(p->child, find_object, (char *)name);

    if (n != NULL) {
        cl_json_delete(n);
        return 0;
    }

    return -1;
}

int cl_json_delete_item_from_object(cl_json_t *json,
    const char *name)
{
    cl_json_s *p = NULL, *root = (cl_json_s *)json;

    __clib_function_init_ex__(true, json, CL_OBJ_JSON,
                              CL_JSON_OBJECT_OFFSET, -1);

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    p = cl_dll_filter(&root->child, find_object, (char *)name);

    if (NULL == p)
        return -1;

    cl_json_delete(p);

    return 0;
}

int cl_json_replace_item_in_array(cl_json_t *array,
    unsigned int index, cl_json_t *new_item)
{
    cl_json_s *a = (cl_json_s *)array;
    cl_json_s *p = NULL, *n;
    unsigned int i = 0;

    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(array, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(new_item, CL_OBJ_JSON,
                                     CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    if (index >= (unsigned int)cl_dll_size(a->child)) {
        return -1;
    }

    while (a->child) {
        n = cl_dll_pop(&a->child);

        if (i == index) {
            cl_json_delete(n);
            n = new_item;
        }

        p = cl_dll_unshift(p, n);
        i++;
    }

    if (p != NULL)
        a->child = p;

    return 0;
}

int cl_json_replace_item_in_object(cl_json_t *root,
    const char *name, cl_json_t *new_item)
{
    cl_string_t *tmp = NULL;
    cl_json_s *a = (cl_json_s *)root;
    cl_json_s *p = NULL, *n;

    __clib_function_init__(false, NULL, -1, -1);

    if ((typeof_validate_object_with_offset(root, CL_OBJ_JSON,
                                            CL_JSON_OBJECT_OFFSET) == false) ||
        (typeof_validate_object_with_offset(new_item, CL_OBJ_JSON,
                                            CL_JSON_OBJECT_OFFSET) == false))
    {
        return -1;
    }

    if (NULL == name) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    tmp = cl_string_create("%s", name);

    while (a->child) {
        n = cl_dll_pop(&a->child);

        if (cl_string_cmp(n->name, tmp) == 0) {
            cl_json_delete(n);
            n = new_item;
            n->name = tmp;
        }

        p = cl_dll_unshift(p, n);
    }

    if (p != NULL)
        a->child = p;

    return 0;
}

cl_string_t *cl_json_to_cstring(const cl_json_t *j,
    bool friendly_output)
{
    char *p = NULL;
    cl_string_t *out;

    __clib_function_init_ex__(true, j, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET, NULL);
    p = print_value((cl_json_s *)j, 0, friendly_output);

    if (NULL == p)
        return NULL;

    out = cl_string_create("%s", p);
    free(p);

    return out;
}

char *cl_json_to_string(const cl_json_t *j, bool friendly_output)
{
    char *p = NULL;

    __clib_function_init_ex__(true, j, CL_OBJ_JSON, CL_JSON_OBJECT_OFFSET,
                              NULL);

    p = print_value((cl_json_s *)j, 0, friendly_output);

    if (NULL == p)
        return NULL;

    return p;
}

const char *cl_json_type_to_string(enum cl_json_type type)
{
    __clib_function_init__(false, NULL, -1, NULL);

    switch (type) {
    case CL_JSON_STRING:
        return "string";

    case CL_JSON_NUMBER:
        return "number";

    case CL_JSON_NUMBER_FLOAT:
        return "number-float";

    case CL_JSON_OBJECT:
        return "object";

    case CL_JSON_ARRAY:
        return "array";

    case CL_JSON_TRUE:
    case CL_JSON_FALSE:
        return "boolean";

    case CL_JSON_NULL:
        return "null";
    }

    return NULL;
}

