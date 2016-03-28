
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

static void print_file_to_user(cfg_file_t *cfg)
{
    cstring_t *s;

    s = cfg_to_string(cfg);

    if (s != NULL) {
        fprintf(stdout, "<<BEGIN>>%s<<END>>\n", cstring_valueof(s));
        cstring_destroy(s);
    }
}

static cfg_section_t *get_section(cfg_file_t *cfg, const char *name)
{
    cfg_section_t *s;

    fprintf(stdout, "Trying to get section '%s'\n", name);
    s = cfg_get_section(cfg, name);

    if (NULL == s) {
        fprintf(stderr, "Section '%s' not found\n", name);
        return NULL;
    }

    fprintf(stdout, "Section loaded\n");

    return s;
}

static int get_number_of_windows(cfg_section_t *section)
{
    cfg_key_t *k;
    cvalue_t *v;
    int value = 0;

    k = cfg_get_key_from_section(section, "windows");

    if (NULL == k) {
        fprintf(stderr, "Key 'windows' not found\n");
        return -1;
    }

    v = cfg_key_value(k);
    value = CVALUE_INT(v);
    fprintf(stdout, "Value of key 'windows' is: %d\n", value);
    cvalue_unref(v);

    return value;
}

static void show_windows_config(cfg_file_t *cfg, int windows)
{
    int i, w, h;
    char *s = NULL;
    cvalue_t *v;
    cfg_key_t *k;

    for (i = 0; i < windows; i++) {
        asprintf(&s, "window_%d", i + 1);

        /* width */
        k = cfg_get_key(cfg, s, "width");

        if (NULL == k) {
            fprintf(stderr, "Key 'width' not found\n");
            free(s);
            return;
        }

        v = cfg_key_value(k);
        w = CVALUE_INT(v);
        cvalue_unref(v);

        /* height */
        k = cfg_get_key(cfg, s, "height");

        if (NULL == k) {
            fprintf(stderr, "Key 'height' not found\n");
            free(s);
            return;
        }

        v = cfg_key_value(k);
        h = CVALUE_INT(v);
        cvalue_unref(v);

        fprintf(stdout, "Window [%d]: width=%d, height=%d\n", i + 1, w, h);
        free(s);
    }
}

int main(int argc, char **argv)
{
    const char *opt = "f:h";
    int option, wnd;
    char *filename = NULL;
    cfg_file_t *cfg;
    cfg_section_t *section;

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

    /* loads configuration file */
    cfg = cfg_load(filename);

    if (NULL == cfg) {
        fprintf(stderr, "Error: %s.\n", cstrerror(cget_last_error()));
        return -1;
    } else
        fprintf(stdout, "Successfully loaded '%s' file\n", filename);

    print_file_to_user(cfg);
    section = get_section(cfg, "main_section");

    if (NULL == section)
        goto end_block;

    wnd = get_number_of_windows(section);
    show_windows_config(cfg, wnd);

end_block:
    cfg_unload(cfg);

    if (filename != NULL)
        free(filename);

    /* This makes valgrind report no memory leaks. */
    cexit();

    return 0;
}

