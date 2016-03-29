
/*
 * Description: API to handle configuration INI files.
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
#include <stdarg.h>

#include "collections.h"

enum cfg_line_type {
    CFG_LINE_SECTION = 1,
    CFG_LINE_KEY = 2,
    CFG_LINE_EMPTY = 4,
    CFG_LINE_COMMENT = 8
};

/** INI line structure */
struct cfg_line_s {
    clist_t             *prev;
    clist_t             *next;
    struct cfg_key_s    *child;
    cstring_t           *name;
    cvalue_t            *value;
    cstring_t           *comment;
    enum cfg_line_type  line_type;
    char                delim;
};

/** INI file structure */
struct cfg_file_s {
    cstring_t               *filename;
    struct cfg_line_s       *section;
};

static bool is_section(const char *line)
{
    bool ret = true;
    cstring_t *p, *t;

    p = cstring_create("%s", line);
    t = cstring_alltrim(p);
    cstring_destroy(p);

    if ((cstring_cchr(t, '[') != 1) ||
        (cstring_cchr(t, ']') != 1) ||
        (cstring_at(t, 0) != '[') ||
        (cstring_at(t, cstring_length(t) - 1) != ']'))
    {
        ret = false;
    }

    cstring_destroy(t);

    return ret;
}

static struct cfg_line_s *new_cfg_line_s(cstring_t *name, const char *value,
    cstring_t *comment, char delim, enum cfg_line_type type)
{
    struct cfg_line_s *l = NULL;
    cstring_t *tmp;

    l = calloc(1, sizeof(struct cfg_line_s));

    if (NULL == l)
        return NULL;

    if (name != NULL)
        l->name = cstring_ref(name);

    if (value != NULL) {
        tmp = cstring_create("%s", value);
        l->value = cvalue_from_string(tmp);
        cstring_destroy(tmp);
    }

    if (comment != NULL)
        l->comment = cstring_ref(comment);

    l->delim = delim;
    l->line_type = type;
    l->child = NULL;

    return l;
}

static void destroy_cfg_line_s(void *a)
{
    struct cfg_line_s *l = (struct cfg_line_s *)a;

    if (l->comment != NULL)
        cstring_unref(l->comment);

    if (l->value != NULL)
        cvalue_destroy(l->value);

    if (l->name != NULL)
        cstring_unref(l->name);

    if (l->child != NULL)
        cdll_free(l->child, destroy_cfg_line_s);

    free(l);
}

static struct cfg_file_s *new_cfg_file_s(const char *filename)
{
    struct cfg_file_s *p = NULL;

    p = calloc(1, sizeof(struct cfg_file_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    p->filename = cstring_create("%s", filename);
    p->section = NULL;

    return p;
}

static void destroy_cfg_file_s(struct cfg_file_s *file)
{
    if (NULL == file)
        return;

    if (file->filename != NULL)
        cstring_destroy(file->filename);

    if (file->section != NULL)
        cdll_free(file->section, destroy_cfg_line_s);

    free(file);
}

static char get_comment_delim_char(const cstring_t *s)
{
    char c = 0;

    if (cstring_find(s, ';') >= 0)
        c = ';';
    else if (cstring_find(s, '#') >= 0)
        c = '#';

    return c;
}

static cstring_t *get_comment(const cstring_t *s, char delim,
    cstring_list_t **list)
{
    char tmp[2] = {0};
    int list_size = 0;
    cstring_t *ref, *comment;

    sprintf(tmp, "%c", delim);
    *list = cstring_split(s, tmp);

    if (NULL == *list)
        return NULL;

    list_size = cstring_list_size(*list);

    if (list_size < 1) {
        /* Never falls here */
        cstring_list_destroy(*list);
        return NULL;
    }

    /*
     * @list_size will always be > 1. So the comment will be in the
     * @list_size - 1 index at the list
     */
    ref = cstring_list_get(*list, list_size - 1);
    comment = cstring_alltrim(ref);
    cstring_unref(ref);

    return comment;
}

static cstring_t *get_line_content(const cstring_t *s, char delim,
    const cstring_list_t *list)
{
    cstring_t *p, *content;
    int l;

    if (NULL == list)
        return cstring_dup(s);

    p = cstring_list_get(list, 0);

    if (delim != 0) {
        /* Has only comment */
        l = cstring_length(p);
        cstring_unref(p);

        if (l == 0)
            return NULL;
    }

    content = cstring_alltrim(p);
    cstring_unref(p);

    return content;
}

static cstring_t *get_data(const cstring_t *s, int index)
{
    cstring_list_t *l = NULL;
    cstring_t *r = NULL, *ref;

    l = cstring_split(s, "=");
    ref = cstring_list_get(l, index);
    r = cstring_alltrim(ref);
    cstring_unref(ref);
    cstring_list_destroy(l);

    return r;
}

static cstring_t *get_name(const cstring_t *s)
{
    return get_data(s, 0);
}

static cstring_t *get_value(const cstring_t *s)
{
    return get_data(s, 1);
}

static struct cfg_line_s *cvt_line_to_cfg_line(const char *line)
{
    enum cfg_line_type line_type = 0;
    char cdelim = 0;
    cstring_t *s = NULL, *comment = NULL, *data = NULL, *name = NULL,
        *value = NULL;
    cstring_list_t *list = NULL;
    struct cfg_line_s *cline = NULL;

    if (!strlen(line)) {
        line_type |= CFG_LINE_EMPTY;
        goto end_block;
    }

    s = cstring_create("%s", line);

    /* Check comment */
    cdelim = get_comment_delim_char(s);

    if (cdelim != 0) {
        comment = get_comment(s, cdelim, &list);
        line_type |= CFG_LINE_COMMENT;
    }

    data = get_line_content(s, cdelim, list);

    if (NULL == data)
        goto end_block;

    if (is_section(cstring_valueof(data))) {
        name = cstring_dup(data);
        line_type |= CFG_LINE_SECTION;
    } else {
        name = get_name(data);
        value = get_value(data);
        line_type |= CFG_LINE_KEY;
    }

end_block:
    cline = new_cfg_line_s((name != NULL) ? name : NULL,
                           (value != NULL) ? cstring_valueof(value) : NULL,
                           (comment != NULL) ? comment : NULL,
                           cdelim, line_type);

    cstring_unref(name);
    cstring_unref(comment);

    if (value != NULL)
        cstring_unref(value);

    if (data != NULL)
        cstring_unref(data);

    if (list != NULL)
        cstring_list_destroy(list);

    if (s != NULL)
        cstring_unref(s);

    return cline;
}

static struct cfg_file_s *__cfg_load(const char *filename)
{
    FILE *fp;
    char *line = NULL;
    struct cfg_file_s *file = NULL;
    struct cfg_line_s *cline = NULL, *section = NULL;

    fp = fopen(filename, "r");

    if (NULL == fp) {
        cset_errno(CL_FILE_OPEN_ERROR);
        return NULL;
    }

    file = new_cfg_file_s(filename);

    if (NULL == file)
        goto end_block;

    while ((line = cfreadline(fp)) != NULL) {
        cline = cvt_line_to_cfg_line(line);

        /* Unrecognized line */
        if (NULL == cline) {
            cset_errno(CL_UNRECOGNIZED_CFG_LINE);
            goto end_block;
        }

        if ((cline->line_type & CFG_LINE_SECTION) ||
            ((NULL == section) &&
             ((cline->line_type & CFG_LINE_EMPTY) ||
              (cline->line_type & CFG_LINE_COMMENT))))
        {
            file->section = cdll_unshift(file->section, cline);
            section = cline;
        } else {
            if (section != NULL)
                section->child = cdll_unshift(section->child, cline);
        }

        free(line);
    }

end_block:
    fclose(fp);
    return file;
}

static int search_section(void *a, void *b)
{
    struct cfg_line_s *s = (struct cfg_line_s *)a;
    char *n = (char *)b;
    cstring_t *p;
    int ret;

    if (!(s->line_type & CFG_LINE_SECTION))
        return 0;

    if (is_section(n) == true)
        p = cstring_create("%s", n);
    else
        p = cstring_create("[%s]", n);

    ret = cstring_cmp(s->name, p);
    cstring_destroy(p);

    return (ret == 0) ? 1 : 0;
}

static int search_key(void *a, void *b)
{
    struct cfg_line_s *k = (struct cfg_line_s *)a;
    char *n = (char *)b;
    cstring_t *p;
    int ret;

    if (!(k->line_type & CFG_LINE_KEY))
        return 0;

    p = cstring_create("%s", n);
    ret = cstring_cmp(k->name, p);
    cstring_destroy(p);

    return (ret == 0) ? 1 : 0;
}

static int write_line_to_file(void *a, void *b)
{
    struct cfg_line_s *l = (struct cfg_line_s *)a;
    cstring_t *s = (cstring_t *)b;
    enum cfg_line_type type;
    cstring_t *v = NULL;

    type = (l->line_type != CFG_LINE_COMMENT) 
                ? l->line_type & ~CFG_LINE_COMMENT 
                : l->line_type;

    switch (type) {
        case CFG_LINE_EMPTY:
            cstring_cat(s, "\n");
            break;

        case CFG_LINE_COMMENT:
            cstring_cat(s, "%c %s\n", l->delim, cstring_valueof(l->comment));
            break;

        case CFG_LINE_SECTION:
            if (l->line_type & CFG_LINE_COMMENT) {
                cstring_cat(s, "%s %c %s\n", cstring_valueof(l->name), l->delim,
                            cstring_valueof(l->comment));
            } else
                cstring_cat(s, "%s\n", cstring_valueof(l->name));

            break;

        case CFG_LINE_KEY:
            v = cvalue_to_string(l->value);

            if (l->line_type & CFG_LINE_COMMENT) {
                cstring_cat(s, "%s=%s %c %s\n", cstring_valueof(l->name),
                            (NULL == v) ? "" : cstring_valueof(v), l->delim,
                            cstring_valueof(l->comment));
            } else
                cstring_cat(s, "%s=%s\n", cstring_valueof(l->name),
                            (NULL == v) ? "" : cstring_valueof(v));

            if (v != NULL)
                cstring_destroy(v);

            break;
    }

    if (l->child != NULL)
        cdll_map(l->child, write_line_to_file, s);

    return 0;
}

static cstring_t *print_cfg(const struct cfg_file_s *file)
{
    cstring_t *s = NULL;

    if (NULL == file->section)
        return cstring_create_empty(0);

    s = cstring_create_empty(0);
    cdll_map(file->section, write_line_to_file, s);

    return s;
}

/*
 * Loads a INI configuration file to a cfg_file_t value.
 */
cfg_file_t LIBEXPORT *cfg_load(const char *filename)
{
    struct cfg_file_s *file = NULL;

    cerrno_clear();

    if (NULL == filename) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    file = __cfg_load(filename);

    return file;
}

/*
 * Frees all memory previously allocated on a cfg_file_t value.
 */
int LIBEXPORT cfg_unload(cfg_file_t *file)
{
    cerrno_clear();

    if (NULL == file) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_cfg_file_s(file);

    return 0;
}

/*
 * Write the contents of a cfg_file_t value to a file. All file content will
 * be overwritten.
 */
int LIBEXPORT cfg_sync(const cfg_file_t *file, const char *filename)
{
    struct cfg_file_s *f = (struct cfg_file_s *)file;
    FILE *fp;
    const char *p;
    cstring_t *s;

    cerrno_clear();

    if (NULL == file) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    p = (filename == NULL) ? cstring_valueof(f->filename) : filename;
    fp = fopen(p, "w+");

    if (NULL == fp) {
        cset_errno(CL_FILE_OPEN_ERROR);
        return -1;
    }

    s = print_cfg(f);

    if (NULL == s) {
        fclose(fp);
        return -1;
    }

    fprintf(fp, "%s", cstring_valueof(s));
    cstring_destroy(s);
    fclose(fp);

    return 0;
}

/*
 * Sets a value to a specific key - section.
 */
int LIBEXPORT cfg_set_value(cfg_file_t *file, const char *section,
    const char *key, const char *fmt, ...)
{
    struct cfg_file_s *f = (struct cfg_file_s *)file;
    struct cfg_line_s *s = NULL, *k = NULL;
    va_list ap;
    char *b = NULL;
    cstring_t *t;

    cerrno_clear();

    if ((NULL == file) || (NULL == section) || (NULL == key)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    vasprintf(&b, fmt, ap);
    va_end(ap);

    s = cdll_map(f->section, search_section, (void *)section);

    if (NULL == s) {
        s = new_cfg_line_s(cstring_create("%s", section), NULL, NULL, 0,
                           CFG_LINE_SECTION);

        if (NULL == s)
            return -1;

        k = new_cfg_line_s(cstring_create("%s", key), b, NULL, 0,
                           CFG_LINE_KEY);

        if (NULL == k) {
            destroy_cfg_line_s(s);
            return -1;
        }

        s->child = cdll_unshift(s->child, k);
        f->section = cdll_unshift(f->section, s);

        goto end_block;
    }

    k = cdll_map(s->child, search_key, (void *)key);

    if (NULL == k) {
        k = new_cfg_line_s(cstring_create("%s", key), b, NULL, 0,
                           CFG_LINE_KEY);

        if (NULL == k)
            return -1;

        s->child = cdll_push(s->child, k);
    } else {
        t = cstring_create("%s", b);

        if (k->value != NULL)
            cstring_destroy(k->value);

        k->value = cvalue_from_string(t);
        cstring_destroy(t);
    }

end_block:
    if (b != NULL)
        free(b);

    return 0;
}

/*
 * Search and get a pointer to a specific section from a cfg_file_t object.
 */
cfg_section_t LIBEXPORT *cfg_get_section(const cfg_file_t *file,
    const char *section)
{
    struct cfg_file_s *f = (struct cfg_file_s *)file;
    struct cfg_line_s *l = NULL;

    cerrno_clear();

    if ((NULL == file) || (NULL == section)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    l = cdll_map(f->section, search_section, (void *)section);

    if (NULL == l) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    return l;
}

cfg_key_t LIBEXPORT *cfg_get_key(const cfg_file_t *file, const char *section,
    const char *key)
{
    cfg_section_t *s = NULL;

    s = cfg_get_section(file, section);

    if (NULL == s)
        return NULL;

    return cfg_get_key_from_section(s, key);
}

/*
 * Search and get a pointer to a specific key from a cfg_section_t object.
 */
cfg_key_t LIBEXPORT *cfg_get_key_from_section(const cfg_section_t *section,
    const char *key)
{
    struct cfg_line_s *s = (struct cfg_line_s *)section;
    struct cfg_line_s *l = NULL;

    cerrno_clear();

    if ((NULL == section) || (NULL == key)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    l = cdll_map(s->child, search_key, (void *)key);

    if (NULL == l) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    return l;
}

/*
 * Gets the section name from a cfg_section_t object.
 */
cstring_t LIBEXPORT *cfg_section_name(const cfg_section_t *section)
{
    struct cfg_line_s *s = (struct cfg_line_s *)section;

    cerrno_clear();

    if (NULL == section) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return s->name;
}

/*
 * Gets the key name from a cfg_key_t object.
 */
cstring_t LIBEXPORT *cfg_key_name(const cfg_key_t *key)
{
    struct cfg_line_s *k = (struct cfg_line_s *)key;

    cerrno_clear();

    if (NULL == key) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return k->name;
}

/*
 * Gets the actual value from a cfg_key_t object.
 */
cvalue_t LIBEXPORT *cfg_key_value(const cfg_key_t *key)
{
    struct cfg_line_s *k = (struct cfg_line_s *)key;

    cerrno_clear();

    if (NULL == key) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cvalue_ref(k->value);
}

cstring_t LIBEXPORT *cfg_to_string(const cfg_file_t *file)
{
    cstring_t *s = NULL;

    cerrno_clear();

    if (NULL == file) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    s = print_cfg(file);

    return s;
}

