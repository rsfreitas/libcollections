
/*
 * Description: Example showing the 'cfg' API from libcollections.
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Dec 20 20:05:48 2015
 * Project: examples
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
#include <unistd.h>

#include <collections.h>

static void help(void)
{
    fprintf(stdout, "Usage: cfg [OPTIONS]\n");
    fprintf(stdout, "An example showing how to use the libcollections "
                    "configuration file API.\n");

    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "  -h\t\t\tShow this help screen.\n");
    fprintf(stdout, "  -f [file name]\tIndicate the configuration file to "
                    "handle.\n");

    fprintf(stdout, "\n");
}

static void print_file_to_user(cl_cfg_file_t *cfg)
{
    cl_string_t *s;

    s = cl_cfg_to_cstring(cfg);

    if (s != NULL) {
        fprintf(stdout, "<<BEGIN>>%s<<END>>\n", cl_string_valueof(s));
        cl_string_destroy(s);
    }
}

static cl_cfg_block_t *get_section(cl_cfg_file_t *cfg, const char *name)
{
    cl_cfg_block_t *s;

    fprintf(stdout, "Trying to get block '%s'\n", name);
    s = cl_cfg_block(cfg, name);

    if (NULL == s) {
        fprintf(stderr, "Section '%s' not found\n", name);
        return NULL;
    }

    fprintf(stdout, "Section loaded\n");

    return s;
}

static int get_number_of_windows(cl_cfg_block_t *block)
{
    cl_cfg_entry_t *k;
    cl_object_t *v;
    int value = 0;

    k = cl_cfg_block_entry(block, "windows");

    if (NULL == k) {
        fprintf(stderr, "Key 'windows' not found\n");
        return -1;
    }

    v = cl_cfg_entry_value(k);
    value = CL_OBJECT_AS_INT(v);
    fprintf(stdout, "Value of entry 'windows' is: %d\n", value);
    cl_object_unref(v);

    return value;
}

static void show_windows_config(cl_cfg_file_t *cfg, int windows)
{
    int i, w, h;
    char *s = NULL;
    cl_object_t *v;
    cl_cfg_entry_t *k;

    for (i = 0; i < windows; i++) {
        asprintf(&s, "window_%d", i + 1);

        /* width */
        k = cl_cfg_entry(cfg, s, "width");

        if (NULL == k) {
            fprintf(stderr, "Key 'width' not found\n");
            free(s);
            return;
        }

        v = cl_cfg_entry_value(k);
        w = CL_OBJECT_AS_INT(v);
        cl_object_unref(v);

        /* height */
        k = cl_cfg_entry(cfg, s, "height");

        if (NULL == k) {
            fprintf(stderr, "Key 'height' not found\n");
            free(s);
            return;
        }

        v = cl_cfg_entry_value(k);
        h = CL_OBJECT_AS_INT(v);
        cl_object_unref(v);

        fprintf(stdout, "Window [%d]: width=%d, height=%d\n", i + 1, w, h);
        free(s);
    }
}

int main(int argc, char **argv)
{
    const char *opt = "f:h";
    int option, wnd;
    char *filename = NULL;
    cl_cfg_file_t *cfg;
    cl_cfg_block_t *block;

    do {
        option = getopt(argc, argv, opt);

        switch (option) {
            case 'h':
                help();
                return 1;

            case 'f':
                filename = strdup(optarg);
                break;

            case '?':
                return -1;
        }
    } while (option != -1);

    if (NULL == filename) {
        fprintf(stderr, "There must be a valid cfg file name.\n");
        return -1;
    }

    cl_init(NULL);

    /* loads configuration file */
    cfg = cl_cfg_load(filename);

    if (NULL == cfg) {
        fprintf(stderr, "Error: %s.\n", cl_strerror(cl_get_last_error()));
        return -1;
    } else
        fprintf(stdout, "Successfully loaded '%s' file\n", filename);

    print_file_to_user(cfg);
    block = get_section(cfg, "main_section");

    if (NULL == block)
        goto end_block;

    wnd = get_number_of_windows(block);
    show_windows_config(cfg, wnd);

    {
        cl_cfg_entry_t *k;
        cl_object_t *v;
        char *p;

        k = cl_cfg_entry(cfg, "window_3", "teste");
        v = cl_cfg_entry_value(k);
        p = CL_OBJECT_AS_STRING(v);
        printf("%s: '%s'\n", __FUNCTION__, p);
        free(p);
        cl_object_unref(v);
    }

end_block:
    cl_cfg_unload(cfg);

    if (filename != NULL)
        free(filename);

    cl_uninit();

    /* This makes valgrind report no memory leaks. */
    cl_exit();

    return 0;
}

