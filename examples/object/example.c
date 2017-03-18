
/*
 * Description: Example showing the use of the cl_object_t object from
 *              libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Apr  1 09:44:50 2016
 * Project: example.c
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
#include <unistd.h>

#include <collections.h>

static void help(void)
{
    fprintf(stdout, "Usage: value [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "cl_object_t API.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\tShow this help screen.\n");
    fprintf(stdout, "  -c\tChar test.\n");
    fprintf(stdout, "  -C\tUnsigned char test.\n");
    fprintf(stdout, "  -i\tInt test.\n");
    fprintf(stdout, "  -I\tUnsigned int test.\n");
    fprintf(stdout, "  -f\tFloat test.\n");
    fprintf(stdout, "  -d\tDouble test.\n");
    fprintf(stdout, "  -s\tC string test.\n");
    fprintf(stdout, "  -S\tCollection string test.\n");
    fprintf(stdout, "  -l\tLong test.\n");
    fprintf(stdout, "  -L\tUnsigned long test.\n");
    fprintf(stdout, "  -k\tLong long test.\n");
    fprintf(stdout, "  -K\tUnsigned long long test.\n");
    fprintf(stdout, "  -j\tShort int test.\n");
    fprintf(stdout, "  -J\tUnsigned short int test.\n");
    fprintf(stdout, "  -a\tAll tests.\n");

    fprintf(stdout, "\n");
}

static void print_cl_object_as_cvalue(cl_object_t *v)
{
    cl_string_t *s;

    s = cl_object_to_cstring(v);
    printf("%s: '%s'\n", __FUNCTION__, cl_string_valueof(s));
    cl_string_unref(s);
}

static void do_test_char(void)
{
    cl_object_t *v;
    char c = 0;

    v = cl_object_create(CL_CHAR, 'z', NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_CHAR, &c, NULL);
    printf("%s: '%c'\n", __FUNCTION__, c);

    cl_object_unref(v);
}

static void do_test_uchar(void)
{
    cl_object_t *v;
    unsigned char c = 0;

    v = cl_object_create(CL_UCHAR, 186, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_UCHAR, &c, NULL);
    printf("%s: '%d'\n", __FUNCTION__, c);

    cl_object_unref(v);
}

static void do_test_int(void)
{
    cl_object_t *v;
    int i = 0;

    v = cl_object_create(CL_INT, 42, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_INT, &i, NULL);
    printf("%s: '%d'\n", __FUNCTION__, i);

    cl_object_unref(v);
}

static void do_test_uint(void)
{
    cl_object_t *v;
    unsigned int i = 0;

    v = cl_object_create(CL_UINT, 420000, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_UINT, &i, NULL);
    printf("%s: '%d'\n", __FUNCTION__, i);

    cl_object_unref(v);
}

static void do_test_sint(void)
{
    cl_object_t *v;
    short int i = 0;

    v = cl_object_create(CL_SINT, 3210, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_SINT, &i, NULL);
    printf("%s: '%hd'\n", __FUNCTION__, i);

    cl_object_unref(v);
}

static void do_test_usint(void)
{
    cl_object_t *v;
    unsigned short int i = 0;

    v = cl_object_create(CL_USINT, 63210, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_USINT, &i, NULL);
    printf("%s: '%hu'\n", __FUNCTION__, i);

    cl_object_unref(v);
}

static void do_test_float(void)
{
    cl_object_t *v;
    float f = 0.0f;

    v = cl_object_create(CL_FLOAT, 3.1415f, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_FLOAT, &f, NULL);
    printf("%s: '%f'\n", __FUNCTION__, f);

    cl_object_unref(v);
}

static void do_test_double(void)
{
    cl_object_t *v;
    double f = 0.0f;

    v = cl_object_create(CL_DOUBLE, 3.14159265f, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_DOUBLE, &f, NULL);
    printf("%s: '%e'\n", __FUNCTION__, f);

    cl_object_unref(v);
}

static void do_test_string(void)
{
    cl_object_t *v;
    char *p = NULL;

    v = cl_object_create(CL_STRING, "This is only a test", NULL);
    print_cl_object_as_cvalue(v);

    p = CL_OBJECT_AS_STRING(v);
    printf("%s: '%s'\n", __FUNCTION__, p);
    free(p);

    cl_object_unref(v);
}

static void do_test_cstring(void)
{
    cl_object_t *v;
    cl_string_t *o = NULL, *p = NULL;

    o = cl_string_create("%s", "This is another test string");
    v = cl_object_create(CL_CSTRING, o, NULL);
    cl_string_unref(o);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_CSTRING, &p, NULL);
    printf("%s: '%s'\n", __FUNCTION__, cl_string_valueof(p));
    cl_string_unref(p);

    cl_object_unref(v);
}

static void do_test_long(void)
{
    cl_object_t *v;
    long l = 0;

    v = cl_object_create(CL_LONG, 2147483647, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_LONG, &l, NULL);
    printf("%s: '%ld'\n", __FUNCTION__, l);

    cl_object_unref(v);
}

static void do_test_ulong(void)
{
    cl_object_t *v;
    unsigned long l = 0;

    v = cl_object_create(CL_ULONG, -2147483647 - 1, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_ULONG, &l, NULL);
    printf("%s: '%lu'\n", __FUNCTION__, l);

    cl_object_unref(v);
}

static void do_test_llong(void)
{
    cl_object_t *v;
    long long l = 0;

    v = cl_object_create(CL_LLONG, 1.12589990684e+15, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_LLONG, &l, NULL);
    printf("%s: '%lld'\n", __FUNCTION__, l);

    cl_object_unref(v);
}

static void do_test_ullong(void)
{
    cl_object_t *v;
    unsigned long long l = 0;

    v = cl_object_create(CL_ULLONG, 9.22337203685e+18, NULL);
    print_cl_object_as_cvalue(v);

    cl_object_get(v, CL_OBJECT_ULLONG, &l, NULL);
    printf("%s: '%llu'\n", __FUNCTION__, l);

    cl_object_unref(v);
}

int main(int argc, char **argv)
{
    const char *opt = "hacCiIfdsSlLkKjJ";
    int option;
    bool test_all = false, test_char = false, test_uchar = false,
         test_int = false, test_uint = false, test_float = false,
         test_double = false, test_string = false, test_cstring = false,
         test_long = false, test_ulong = false, test_llong = false,
         test_ullong = false, test_sint = false, test_usint = false;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'a':
                test_all = true;
                break;

            case 'c':
                test_char = true;
                break;

            case 'C':
                test_uchar = true;
                break;

            case 'i':
                test_int = true;
                break;

            case 'I':
                test_uint = true;
                break;

            case 'f':
                test_float = true;
                break;

            case 'd':
                test_double = true;
                break;

            case 's':
                test_string = true;
                break;

            case 'S':
                test_cstring = true;
                break;

            case 'l':
                test_long = true;
                break;

            case 'L':
                test_ulong = true;
                break;

            case 'k':
                test_llong = true;
                break;

            case 'K':
                test_ullong = true;
                break;

            case 'j':
                test_sint = true;
                break;

            case 'J':
                test_usint = true;
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    cl_init(NULL);

    if (test_all || test_char)
        do_test_char();

    if (test_all || test_uchar)
        do_test_uchar();

    if (test_all || test_int)
        do_test_int();

    if (test_all || test_uint)
        do_test_uint();

    if (test_all || test_float)
        do_test_float();

    if (test_all || test_double)
        do_test_double();

    if (test_all || test_string)
        do_test_string();

    if (test_all || test_cstring)
        do_test_cstring();

    if (test_all || test_long)
        do_test_long();

    if (test_all || test_ulong)
        do_test_ulong();

    if (test_all || test_llong)
        do_test_llong();

    if (test_all || test_ullong)
        do_test_ullong();

    if (test_all || test_sint)
        do_test_sint();

    if (test_all || test_usint)
        do_test_usint();

    cl_uninit();

    /* This makes valgrind report no memory leaks. */
    cl_exit();

    return 0;
}

