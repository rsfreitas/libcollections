
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
    clist_entry_t       *prev;
    clist_entry_t       *next;
    struct cobject_hdr  hdr;
    struct line_s       *child;
    cstring_t           *name;
    cobject_t           *value;
    cstring_t           *comment;
    enum cfg_line_type  line_type;
    char                delim;
};

#define CFG_LINE_OFFSET         \
    (sizeof(clist_entry_t *) + sizeof(clist_entry_t *))

/** INI file structure */
#define cfg_file_members                            \
    cl_struct_member(cstring_t *, filename)         \
    cl_struct_member(struct cfg_line_s *, section)

cl_struct_declare(cfg_file_s, cfg_file_members);

#define cfg_file_s          cl_struct(cfg_file_s)

static bool is_section(const char *line)
{
    bool ret = true;
    cstring_t *p;

    p = cstring_create("%s", line);
    cstring_alltrim(p);

    if ((cstring_cchr(p, '[') != 1) ||
        (cstring_cchr(p, ']') != 1) ||
        (cstring_at(p, 0) != '[') ||
        (cstring_at(p, cstring_length(p) - 1) != ']'))
    {
        ret = false;
    }

    cstring_destroy(p);

    return ret;
}

static struct cfg_line_s *new_cfg_line_s(cstring_t *name, const char *value,
    cstring_t *comment, char delim, enum cfg_line_type type)
{
    struct cfg_line_s *l = NULL;
    cstring_t *tmp;
    enum cl_object object;

    l = calloc(1, sizeof(struct cfg_line_s));

    if (NULL == l)
        return NULL;

    if (name != NULL)
        l->name = cstring_ref(name);

    if (value != NULL) {
        tmp = cstring_create("%s", value);
        l->value = cobject_from_cstring(tmp);
        cstring_destroy(tmp);
    }

    if (comment != NULL)
        l->comment = cstring_ref(comment);

    l->delim = delim;
    l->line_type = type;
    l->child = NULL;

    if (type == CFG_LINE_SECTION)
        object = CFG_SECTION;
    else
        object = CFG_KEY;

    set_typeof_with_offset(object, l, CFG_LINE_OFFSET);

    return l;
}

static void destroy_cfg_line_s(void *a)
{
    struct cfg_line_s *l = (struct cfg_line_s *)a;

    if (l->comment != NULL)
        cstring_unref(l->comment);

    if (l->value != NULL)
        cobject_destroy(l->value);

    if (l->name != NULL)
        cstring_unref(l->name);

    if (l->child != NULL)
        cdll_free(l->child, destroy_cfg_line_s);

    free(l);
}

static cfg_file_s *new_cfg_file_s(const char *filename)
{
    cfg_file_s *p = NULL;

    p = calloc(1, sizeof(cfg_file_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    set_typeof(CFG_FILE, p);
    p->filename = cstring_create("%s", filename);
    p->section = NULL;

    return p;
}

static void destroy_cfg_file_s(cfg_file_s *file)
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
    comment = cstring_dup(ref);
    cstring_unref(ref);
    cstring_alltrim(comment);

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

    content = cstring_dup(p);
    cstring_unref(p);
    cstring_alltrim(content);

    return content;
}

static cstring_t *get_data(const cstring_t *s, int index)
{
    cstring_list_t *l = NULL;
    cstring_t *r = NULL, *ref;

    l = cstring_split(s, "=");
    ref = cstring_list_get(l, index);
    r = cstring_dup(ref);
    cstring_unref(ref);
    cstring_alltrim(r);
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

static cfg_file_s *__cfg_load(const char *filename)
{
    FILE *fp;
    char *line = NULL;
    cfg_file_s *file = NULL;
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
            v = cobject_to_cstring(l->value);

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

static cstring_t *print_cfg(const cfg_file_s *file)
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
    cfg_file_s *file = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

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
    __clib_function_init__(true, file, CFG_FILE, -1);
    destroy_cfg_file_s(file);

    return 0;
}

/*
 * Write the contents of a cfg_file_t value to a file. All file content will
 * be overwritten.
 */
int LIBEXPORT cfg_sync(const cfg_file_t *file, const char *filename)
{
    cfg_file_s *f = (cfg_file_s *)file;
    FILE *fp;
    const char *p;
    cstring_t *s;

    __clib_function_init__(true, file, CFG_FILE, -1);
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
    cfg_file_s *f = (cfg_file_s *)file;
    struct cfg_line_s *s = NULL, *k = NULL;
    va_list ap;
    char *b = NULL;
    cstring_t *t;

    __clib_function_init__(true, file, CFG_FILE, -1);

    if ((NULL == section) || (NULL == key)) {
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

        k->value = cobject_from_cstring(t);
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
    cfg_file_s *f = (cfg_file_s *)file;
    struct cfg_line_s *l = NULL;

    __clib_function_init__(true, file, CFG_FILE, NULL);

    if (NULL == section) {
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

    __clib_function_init_ex__(true, section, CFG_SECTION, CFG_LINE_OFFSET,
                              NULL);

    if (NULL == key) {
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

    __clib_function_init_ex__(true, section, CFG_SECTION, CFG_LINE_OFFSET,
                              NULL);

    return s->name;
}

/*
 * Gets the key name from a cfg_key_t object.
 */
cstring_t LIBEXPORT *cfg_key_name(const cfg_key_t *key)
{
    struct cfg_line_s *k = (struct cfg_line_s *)key;

    __clib_function_init_ex__(true, key, CFG_KEY, CFG_LINE_OFFSET, NULL);

    return k->name;
}

/*
 * Gets the actual value from a cfg_key_t object.
 */
cobject_t LIBEXPORT *cfg_key_value(const cfg_key_t *key)
{
    struct cfg_line_s *k = (struct cfg_line_s *)key;

    __clib_function_init_ex__(true, key, CFG_KEY, CFG_LINE_OFFSET, NULL);

    return cobject_ref(k->value);
}

cstring_t LIBEXPORT *cfg_to_cstring(const cfg_file_t *file)
{
    cstring_t *s = NULL;

    __clib_function_init__(true, file, CFG_FILE, NULL);
    s = print_cfg(file);

    return s;
}

