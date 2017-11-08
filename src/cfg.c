
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
#define cfg_line_members                            \
    cl_struct_member(cl_list_t *, child)            \
    cl_struct_member(cl_string_t *, name)           \
    cl_struct_member(cl_object_t *, value)          \
    cl_struct_member(cl_string_t *, comment)        \
    cl_struct_member(enum cfg_line_type, line_type) \
    cl_struct_member(char, delim)                   \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cfg_line_s, cfg_line_members);
#define cfg_line_s              cl_struct(cfg_line_s)

/** INI file structure */
#define cl_cfg_file_members                         \
    cl_struct_member(cl_string_t *, filename)       \
    cl_struct_member(cl_list_t *, block)            \
    cl_struct_member(struct cl_ref_s, ref)

cl_struct_declare(cfg_file_s, cl_cfg_file_members);
#define cfg_file_s              cl_struct(cfg_file_s)

/*
 *
 * Objects creation/destruction
 *
 */

static cfg_line_s *cl_cfg_line_ref(cfg_line_s *line)
{
    cl_ref_inc(&line->ref);

    return line;
}

static void cl_cfg_line_unref(void *l)
{
    cfg_line_s *line = (cfg_line_s *)l;

    cl_ref_dec(&line->ref);
}

static void destroy_cfg_line_s(void *a)
{
    cfg_line_s *l = (cfg_line_s *)a;

    if (l->comment != NULL)
        cl_string_unref(l->comment);

    if (l->value != NULL)
        cl_object_destroy(l->value);

    if (l->name != NULL)
        cl_string_unref(l->name);

    if (l->child != NULL)
        cl_list_destroy(l->child);

    free(l);
}

static void __destroy_cfg_line(const struct cl_ref_s *ref)
{
    cfg_line_s *l = cl_container_of(ref, cfg_line_s, ref);

    if (NULL == l)
        return;

    destroy_cfg_line_s(l);
}

static cfg_line_s *new_cfg_line_s(cl_string_t *name,
    const char *value, cl_string_t *comment, char delim,
    enum cfg_line_type type)
{
    cfg_line_s *l = NULL;
    cl_string_t *tmp;
    enum cl_object object;

    l = calloc(1, sizeof(cfg_line_s));

    if (NULL == l)
        return NULL;

    if (name != NULL)
        l->name = cl_string_ref(name);

    if (value != NULL) {
        tmp = cl_string_create("%s", value);
        l->value = cl_object_from_cstring(tmp);
        cl_string_destroy(tmp);
    }

    if (comment != NULL)
        l->comment = cl_string_ref(comment);

    l->delim = delim;
    l->line_type = type;
    l->child = cl_list_create(cl_cfg_line_unref, NULL, NULL, NULL);

    if (type == CFG_LINE_SECTION)
        object = CL_OBJ_CFG_BLOCK;
    else
        object = CL_OBJ_CFG_ENTRY;

    /* Reference count */
    l->ref.count = 1;
    l->ref.free = __destroy_cfg_line;

    typeof_set(object, l);

    return l;
}

static void destroy_cfg_file_s(const struct cl_ref_s *ref)
{
    cfg_file_s *file = cl_container_of(ref, cfg_file_s, ref);

    if (NULL == file)
        return;

    if (file->filename != NULL)
        cl_string_destroy(file->filename);

    if (file->block != NULL)
        cl_list_destroy(file->block);

    free(file);
}

static cfg_file_s *new_cfg_file_s(const char *filename)
{
    cfg_file_s *p = NULL;

    p = calloc(1, sizeof(cfg_file_s));

    if (NULL == p) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    typeof_set(CL_OBJ_CFG_FILE, p);
    p->filename = cl_string_create("%s", filename);
    p->block = cl_list_create(cl_cfg_line_unref, NULL, NULL, NULL);

    /* Reference count */
    p->ref.count = 1;
    p->ref.free = destroy_cfg_file_s;

    return p;
}

/*
 *
 * Internal functions
 *
 */

static bool is_full_block_name(const char *line)
{
    bool ret = true;
    cl_string_t *p;

    p = cl_string_create("%s", line);
    cl_string_alltrim(p);

    if ((cl_string_cchr(p, '[') != 1) ||
        (cl_string_cchr(p, ']') != 1) ||
        (cl_string_at(p, 0) != '[') ||
        (cl_string_at(p, cl_string_length(p) - 1) != ']'))
    {
        ret = false;
    }

    cl_string_destroy(p);

    return ret;
}

static char get_comment_delim_char(const cl_string_t *s)
{
    char c = 0;

    if (cl_string_find(s, ';') >= 0)
        c = ';';
    else if (cl_string_find(s, '#') >= 0)
        c = '#';

    return c;
}

static cl_string_t *get_comment(const cl_string_t *s, char delim,
    cl_stringlist_t **list)
{
    char tmp[2] = {0};
    int list_size = 0;
    cl_string_t *ref, *comment;

    sprintf(tmp, "%c", delim);
    *list = cl_string_split(s, tmp);

    if (NULL == *list)
        return NULL;

    list_size = cl_stringlist_size(*list);

    if (list_size < 1) {
        /* Never falls here */
        cl_stringlist_destroy(*list);
        return NULL;
    }

    /*
     * @list_size will always be > 1. So the comment will be in the
     * @list_size - 1 index at the list
     */
    ref = cl_stringlist_get(*list, list_size - 1);
    comment = cl_string_dup(ref);
    cl_string_unref(ref);
    cl_string_alltrim(comment);

    return comment;
}

static cl_string_t *get_line_content(const cl_string_t *s, char delim,
    const cl_stringlist_t *list)
{
    cl_string_t *p = NULL, *content;
    int l;

    if (NULL == list)
        return cl_string_dup(s);

    p = cl_stringlist_get(list, 0);

    if (delim != 0) {
        /* Has only comment */
        l = cl_string_length(p);

        if (l == 0) {
            cl_string_unref(p);
            return NULL;
        }
    }

    content = cl_string_dup(p);
    cl_string_unref(p);
    cl_string_alltrim(content);

    return content;
}

static cl_string_t *get_data(const cl_string_t *s, int index)
{
    cl_stringlist_t *l = NULL;
    cl_string_t *r = NULL, *ref;

    l = cl_string_split(s, "=");
    ref = cl_stringlist_get(l, index);
    r = cl_string_dup(ref);
    cl_string_unref(ref);
    cl_string_alltrim(r);
    cl_stringlist_destroy(l);

    return r;
}

static cl_string_t *get_name(const cl_string_t *s)
{
    return get_data(s, 0);
}

static cl_string_t *get_value(const cl_string_t *s)
{
    return get_data(s, 1);
}

static cfg_line_s *cvt_line_to_cfg_line(const char *line)
{
    enum cfg_line_type line_type = 0;
    char cdelim = 0;
    cl_string_t *s = NULL, *comment = NULL, *data = NULL, *name = NULL,
        *value = NULL;
    cl_stringlist_t *list = NULL;
    cfg_line_s *cline = NULL;

    if (!strlen(line)) {
        line_type |= CFG_LINE_EMPTY;
        goto end_block;
    }

    s = cl_string_create("%s", line);

    /* Check comment */
    cdelim = get_comment_delim_char(s);

    if (cdelim != 0) {
        comment = get_comment(s, cdelim, &list);
        line_type |= CFG_LINE_COMMENT;
    }

    data = get_line_content(s, cdelim, list);

    if (NULL == data)
        goto end_block;

    if (is_full_block_name(cl_string_valueof(data))) {
        name = cl_string_dup(data);
        line_type |= CFG_LINE_SECTION;
    } else {
        name = get_name(data);
        value = get_value(data);
        line_type |= CFG_LINE_KEY;
    }

end_block:
    cline = new_cfg_line_s((name != NULL) ? name : NULL,
                           (value != NULL) ? cl_string_valueof(value) : NULL,
                           (comment != NULL) ? comment : NULL,
                           cdelim, line_type);

    cl_string_unref(name);
    cl_string_unref(comment);

    if (value != NULL)
        cl_string_unref(value);

    if (data != NULL)
        cl_string_unref(data);

    if (list != NULL)
        cl_stringlist_destroy(list);

    if (s != NULL)
        cl_string_unref(s);

    return cline;
}

static cfg_file_s *__cfg_load(const char *filename)
{
    FILE *fp = NULL;
    char *line = NULL;
    cfg_file_s *file = NULL;
    cfg_line_s *cline = NULL, *block = NULL;

    file = new_cfg_file_s(filename);

    /*
     * If filename == NULL means that we're creating a cl_cfg_file_t to write
     * new information.
     */
    if ((NULL == file) || (NULL == filename))
        goto end_block;

    fp = fopen(filename, "r");

    if (NULL == fp) {
        cset_errno(CL_FILE_OPEN_ERROR);
        return NULL;
    }

    while ((line = cl_freadline(fp)) != NULL) {
        cline = cvt_line_to_cfg_line(line);

        /* Unrecognized line */
        if (NULL == cline) {
            cset_errno(CL_UNRECOGNIZED_CFG_LINE);
            goto end_block;
        }

        if ((cline->line_type & CFG_LINE_SECTION) ||
            ((NULL == block) &&
             ((cline->line_type & CFG_LINE_EMPTY) ||
              (cline->line_type & CFG_LINE_COMMENT))))
        {
            cl_list_unshift(file->block, cline, -1);
            block = cline;
        } else {
            if (block != NULL)
                cl_list_unshift(block->child, cline, -1);
        }

        free(line);
    }

end_block:
    if (fp != NULL)
        fclose(fp);

    if (line != NULL)
        free(line);

    return file;
}

static int search_block(cl_list_node_t *a, void *b)
{
    cfg_line_s *s = (cfg_line_s *)cl_list_node_content(a);
    char *n = (char *)b;
    cl_string_t *p;
    int ret;

    if (!(s->line_type & CFG_LINE_SECTION))
        return 0;

    if (is_full_block_name(n) == true)
        p = cl_string_create("%s", n);
    else
        p = cl_string_create("[%s]", n);

    ret = cl_string_cmp(s->name, p);
    cl_string_destroy(p);

    return (ret == 0) ? 1 : 0;
}

static int search_entry(cl_list_node_t *a, void *b)
{
    cfg_line_s *k = (cfg_line_s *)cl_list_node_content(a);
    char *n = (char *)b;
    cl_string_t *p;
    int ret;

    if (!(k->line_type & CFG_LINE_KEY))
        return 0;

    p = cl_string_create("%s", n);
    ret = cl_string_cmp(k->name, p);
    cl_string_destroy(p);

    return (ret == 0) ? 1 : 0;
}

static int write_line_to_file(cl_list_node_t *a, void *b)
{
    cfg_line_s *l = (cfg_line_s *)cl_list_node_content(a);
    cl_string_t *s = (cl_string_t *)b;
    enum cfg_line_type type;
    cl_string_t *v = NULL;

    type = (l->line_type != CFG_LINE_COMMENT) 
                ? l->line_type & ~CFG_LINE_COMMENT 
                : l->line_type;

    switch (type) {
        case CFG_LINE_EMPTY:
            cl_string_cat(s, "\n");
            break;

        case CFG_LINE_COMMENT:
            cl_string_cat(s, "%c %s\n", l->delim,
                          cl_string_valueof(l->comment));

            break;

        case CFG_LINE_SECTION:
            if (l->line_type & CFG_LINE_COMMENT) {
                cl_string_cat(s, "%s %c %s\n", cl_string_valueof(l->name),
                              l->delim, cl_string_valueof(l->comment));
            } else
                cl_string_cat(s, "%s\n", cl_string_valueof(l->name));

            break;

        case CFG_LINE_KEY:
            v = cl_object_to_cstring(l->value);

            if (l->line_type & CFG_LINE_COMMENT) {
                cl_string_cat(s, "%s=%s %c %s\n", cl_string_valueof(l->name),
                              (NULL == v) ? "" : cl_string_valueof(v), l->delim,
                              cl_string_valueof(l->comment));
            } else
                cl_string_cat(s, "%s=%s\n", cl_string_valueof(l->name),
                              (NULL == v) ? "" : cl_string_valueof(v));

            if (v != NULL)
                cl_string_destroy(v);

            break;
    }

    if (l->child != NULL)
        cl_list_map(l->child, write_line_to_file, s);

    return 0;
}

static cl_string_t *print_cfg(const cfg_file_s *file)
{
    cl_string_t *s = NULL;

    if (NULL == file->block)
        return cl_string_create_empty(0);

    s = cl_string_create_empty(0);
    cl_list_map(file->block, write_line_to_file, s);

    return s;
}

/*
 * Loads a INI configuration file to a cl_cfg_file_t value.
 */
__PUB_API__ cl_cfg_file_t *cl_cfg_load(const char *filename)
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

__PUB_API__ cl_cfg_file_t *cl_cfg_create(void)
{
    __clib_function_init__(false, NULL, -1, NULL);
    return __cfg_load(NULL);
}

/*
 * Frees all memory previously allocated on a cl_cfg_file_t value.
 */
__PUB_API__ int cl_cfg_unload(cl_cfg_file_t *file)
{
    return cl_cfg_unref(file);
}

/*
 * Write the contents of a cl_cfg_file_t value to a file. All file content will
 * be overwritten.
 */
__PUB_API__ int cl_cfg_sync(const cl_cfg_file_t *file, const char *filename)
{
    cfg_file_s *f = (cfg_file_s *)file;
    FILE *fp;
    const char *p;
    cl_string_t *s;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, -1);
    p = (filename == NULL) ? cl_string_valueof(f->filename) : filename;
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

    if (cl_string_isempty(s) == false)
        fprintf(fp, "%s", cl_string_valueof(s));

    cl_string_destroy(s);
    fclose(fp);

    return 0;
}

/*
 * Sets a value to a specific entry inside a block.
 */
__PUB_API__ int cl_cfg_set_value(cl_cfg_file_t *file, const char *block,
    const char *entry, const char *fmt, ...)
{
    cfg_file_s *f = (cfg_file_s *)file;
    cfg_line_s *s = NULL, *k = NULL;
    va_list ap;
    char *b = NULL;
    cl_string_t *t;
    cl_list_node_t *node = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, -1);

    if ((NULL == block) || (NULL == entry)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    vasprintf(&b, fmt, ap);
    va_end(ap);

    node = cl_list_map(f->block, search_block, (void *)block);

    if (NULL == node) {
        /*
         * We create an empty line to keep a distance from one block to
         * another
         */
        s = new_cfg_line_s(NULL, NULL, NULL, 0, CFG_LINE_EMPTY);
        cl_list_unshift(f->block, s, -1);

        if (is_full_block_name(block) == true)
            t = cl_string_create("%s", block);
        else
            t = cl_string_create("[%s]", block);

        s = new_cfg_line_s(t, NULL, NULL, 0, CFG_LINE_SECTION);
        cl_string_unref(t);

        if (NULL == s)
            return -1;

        t = cl_string_create("%s", entry);
        k = new_cfg_line_s(t, b, NULL, 0, CFG_LINE_KEY);
        cl_string_unref(t);

        if (NULL == k) {
            destroy_cfg_line_s(s);
            return -1;
        }

        cl_list_unshift(s->child, k, -1);
        cl_list_unshift(f->block, s, -1);

        goto end_block;
    }

    s = cl_list_node_content(node);
    cl_list_node_unref(node);

    node = cl_list_map(s->child, search_entry, (void *)entry);

    if (NULL == node) {
        t = cl_string_create("%s", entry);
        k = new_cfg_line_s(t, b, NULL, 0, CFG_LINE_KEY);
        cl_string_unref(t);

        if (NULL == k)
            return -1;

        cl_list_push(s->child, k, -1);
    } else {
        k = cl_list_node_content(node);
        cl_list_node_unref(node);
        t = cl_string_create("%s", b);

        if (k->value != NULL)
            cl_string_destroy(k->value);

        k->value = cl_object_from_cstring(t);
        cl_string_destroy(t);
    }

end_block:
    if (b != NULL)
        free(b);

    return 0;
}

/*
 * Search and get a pointer to a specific block from a cl_cfg_file_t object.
 */
__PUB_API__ cl_cfg_block_t *cl_cfg_block(const cl_cfg_file_t *file,
    const char *block)
{
    cfg_file_s *f = (cfg_file_s *)file;
    cfg_line_s *l = NULL;
    cl_list_node_t *node = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, NULL);

    if (NULL == block) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    node = cl_list_map(f->block, search_block, (void *)block);

    if (NULL == node) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    l = cl_list_node_content(node);
    cl_list_node_unref(node);

    return l;
}

__PUB_API__ cl_cfg_entry_t *cl_cfg_entry(const cl_cfg_file_t *file,
    const char *block, const char *entry)
{
    cl_cfg_block_t *s = NULL;

    s = cl_cfg_block(file, block);

    if (NULL == s)
        return NULL;

    return cl_cfg_block_entry(s, entry);
}

/*
 * Search and get a pointer to a specific entry from a cl_cfg_block_t object.
 */
__PUB_API__ cl_cfg_entry_t *cl_cfg_block_entry(const cl_cfg_block_t *block,
    const char *entry)
{
    cfg_line_s *s = (cfg_line_s *)block;
    cfg_line_s *l = NULL;
    cl_list_node_t *node = NULL;

    __clib_function_init__(true, block, CL_OBJ_CFG_BLOCK, NULL);

    if (NULL == entry) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    node = cl_list_map(s->child, search_entry, (void *)entry);

    if (NULL == node) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return NULL;
    }

    l = cl_list_node_content(node);
    cl_list_node_unref(node);

    return l;
}

/*
 * Gets the block name from a cl_cfg_block_t object.
 */
__PUB_API__ cl_string_t *cl_cfg_block_name(const cl_cfg_block_t *block)
{
    cfg_line_s *s = (cfg_line_s *)block;

    __clib_function_init__(true, block, CL_OBJ_CFG_BLOCK, NULL);

    return s->name;
}

/*
 * Gets the entry name from a cl_cfg_entry_t object.
 */
__PUB_API__ cl_string_t *cl_cfg_entry_name(const cl_cfg_entry_t *entry)
{
    cfg_line_s *k = (cfg_line_s *)entry;

    __clib_function_init__(true, entry, CL_OBJ_CFG_ENTRY, NULL);

    return k->name;
}

/*
 * Gets the actual value from a cl_cfg_entry_t object.
 */
__PUB_API__ cl_object_t *cl_cfg_entry_value(const cl_cfg_entry_t *entry)
{
    cfg_line_s *k = (cfg_line_s *)entry;

    __clib_function_init__(true, entry, CL_OBJ_CFG_ENTRY, NULL);

    return cl_object_ref(k->value);
}

__PUB_API__ cl_string_t *cl_cfg_entry_comment(const cl_cfg_entry_t *entry)
{
    cfg_line_s *k = (cfg_line_s *)entry;

    __clib_function_init__(true, entry, CL_OBJ_CFG_ENTRY, NULL);

    return k->comment;
}

__PUB_API__ cl_string_t *cl_cfg_to_cstring(const cl_cfg_file_t *file)
{
    cl_string_t *s = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, NULL);
    s = print_cfg(file);

    return s;
}

static int append_entry_name(cl_list_node_t *a, void *b)
{
    cfg_line_s *entry = (cfg_line_s *)cl_list_node_content(a);
    cl_stringlist_t *keys = (cl_stringlist_t *)b;

    cl_stringlist_add(keys, entry->name);

    return 0;
}

__PUB_API__ cl_stringlist_t *cl_cfg_all_entry_names(const cl_cfg_file_t *file,
    const char *block)
{
    cfg_line_s *s;
    cl_stringlist_t *keys;

    s = cl_cfg_block(file, block);

    if (NULL == s)
        return NULL;

    keys = cl_stringlist_create();
    cl_list_map(s->child, append_entry_name, keys);

    return keys;
}

__PUB_API__ cl_stringlist_t *cl_cfg_block_entry_names(const cl_cfg_block_t *block)
{
    cfg_line_s *s = (cfg_line_s *)block;
    cl_stringlist_t *keys;

    __clib_function_init__(true, block, CL_OBJ_CFG_BLOCK, NULL);
    keys = cl_stringlist_create();
    cl_list_map(s->child, append_entry_name, keys);

    return keys;
}

static int append_block_name(cl_list_node_t *a, void *b)
{
    cfg_line_s *block = (cfg_line_s *)cl_list_node_content(a);
    cl_stringlist_t *sections = (cl_stringlist_t *)b;
    cl_string_t *s = cl_string_dup(block->name);

    /* Remove brackets */
    cl_string_dchr(s, '[');
    cl_string_dchr(s, ']');

    cl_stringlist_add(sections, s);
    cl_string_unref(s);

    return 0;
}

__PUB_API__ cl_stringlist_t *cl_cfg_all_block_names(const cl_cfg_file_t *file)
{
    cfg_file_s *f = (cfg_file_s *)file;
    cl_stringlist_t *sections = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, NULL);
    sections = cl_stringlist_create();
    cl_list_map(f->block, append_block_name, sections);

    return sections;
}

__PUB_API__ cl_cfg_entry_t *cl_cfg_entry_ref(cl_cfg_entry_t *entry)
{
    __clib_function_init__(true, entry, CL_OBJ_CFG_ENTRY, NULL);

    return cl_cfg_line_ref(entry);
}

__PUB_API__ int cl_cfg_entry_unref(cl_cfg_entry_t *entry)
{
    __clib_function_init__(true, entry, CL_OBJ_CFG_ENTRY, -1);
    cl_cfg_line_unref(entry);

    return 0;
}

__PUB_API__ cl_cfg_block_t *cl_cfg_block_ref(cl_cfg_block_t *block)
{
    __clib_function_init__(true, block, CL_OBJ_CFG_BLOCK, NULL);

    return cl_cfg_line_ref(block);
}

__PUB_API__ int cl_cfg_block_unref(cl_cfg_block_t *block)
{
    __clib_function_init__(true, block, CL_OBJ_CFG_BLOCK, -1);
    cl_cfg_line_unref(block);

    return 0;
}

__PUB_API__ cl_cfg_file_t *cl_cfg_ref(cl_cfg_file_t *file)
{
    cfg_file_s *f = (cfg_file_s *)file;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, NULL);
    cl_ref_inc(&f->ref);

    return file;
}

__PUB_API__ int cl_cfg_unref(cl_cfg_file_t *file)
{
    cfg_file_s *f = (cfg_file_s *)file;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, -1);
    cl_ref_dec(&f->ref);

    return 0;
}

__PUB_API__ int cl_cfg_set_value_comment(cl_cfg_file_t *file,
    const char *block, const char *entry, const char *fmt, ...)
{
    cfg_line_s *cl_key = NULL;
    va_list ap;
    char *s = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, -1);
    cl_key = cl_cfg_entry(file, block, entry);

    if (NULL == cl_key) {
        cset_errno(CL_VALUE_NOT_FOUND);
        return -1;
    }

    va_start(ap, fmt);
    vasprintf(&s, fmt, ap);
    va_end(ap);

    if (cl_key->comment != NULL)
        cl_string_unref(cl_key->comment);

    cl_key->comment = cl_string_create("%s", s);
    free(s);

    return 0;
}

__PUB_API__ cl_object_t *cl_cfg_get_value(const cl_cfg_file_t *file,
    const char *block, const char *entry)
{
    cfg_line_s *cl_entry = NULL;

    __clib_function_init__(true, file, CL_OBJ_CFG_FILE, NULL);
    cl_entry = cl_cfg_entry(file, block, entry);

    if (NULL == cl_entry) {
        cset_errno(CL_VALUE_NOT_FOUND);
        return NULL;
    }

    return cl_cfg_entry_value(cl_entry);
}

