
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Feb 20 09:29:22 2017
 * Project: libcollections
 *
 * Copyright (C) 2017 Rodrigo Freitas
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

#include "collections.h"

#define PRIME_NUMBER                131
#define hashsize(n)                 ((unsigned short int)1 << (n))
#define hashmask(n)                 (hashsize(n) - 1)

#define mix(a, b, c)    \
{   \
    a -= b; a -= c; a ^= (c >> 13); \
    b -= c; b -= a; b ^= (a << 8); \
    c -= a; c -= b; c ^= (b >> 13); \
    a -= b; a -= c; a ^= (c >> 12);  \
    b -= c; b -= a; b ^= (a << 16); \
    c -= a; c -= b; c ^= (b >> 5); \
    a -= b; a -= c; a ^= (c >> 3);  \
    b -= c; b -= a; b ^= (a << 10); \
    c -= a; c -= b; c ^= (b >> 15); \
}


#define chashtable_members                              \
    cl_struct_member(unsigned int, size)                \
    cl_struct_member(bool, replace_data)                \
    cl_struct_member(char **, keys)                     \
    cl_struct_member(void **, table)                    \
    cl_struct_member(bool, (*compare)(void *, void *))  \
    cl_struct_member(void, (*release)(void *))          \
    cl_struct_member(struct cref_s, ref)

cl_struct_declare(hashtable_s, chashtable_members);

#define hashtable_s         cl_struct(hashtable_s)

/*
 *
 * Functions to handle elements inside a list of keys exported by chashtable_keys
 * function.
 *
 */

static int compare_to_keys(clist_node_t *a, clist_node_t *b)
{
    char *key_a = clist_node_content(a);
    char *key_b = clist_node_content(b);

    return strcmp(key_a, key_b);
}

static int filter_keys(clist_node_t *a, void *b)
{
    char *key_a = clist_node_content(a);
    char *key_b = (char *)b;

    if (strcmp(key_a, key_b) == 0)
        return 1;

    return 0;
}

static int equals_keys(clist_node_t *a, clist_node_t *b)
{
    char *key_a = clist_node_content(a);
    char *key_b = clist_node_content(b);

    if (strcmp(key_a, key_b) == 0)
        return 1;

    return 0;
}

static clist_t *create_list_of_keys(bool dup)
{
    return clist_create((dup == true) ? free : NULL, compare_to_keys,
                        filter_keys, equals_keys);
}

/*
 * Copies all stored keys to a list of keys.
 */
static void add_keys_to_list_of_keys(clist_t *keys, hashtable_s *hashtable,
    bool dup)
{
    unsigned int i;

    for (i = 0; i < hashtable->size; i++)
        if (hashtable->keys[i] != NULL)
            clist_push(keys, (dup == true) ? strdup(hashtable->keys[i])
                                           : hashtable->keys[i], -1);
}

/*
 * Count all values stored in the hash table.
 */
static int count_values(hashtable_s *hashtable)
{
    unsigned int i, c = 0;

    for (i = 0; i < hashtable->size; i++)
        if (hashtable->table[i] != NULL)
            c++;

    return c;
}

static void create_keys_storage(hashtable_s *hashtable, unsigned int size)
{
    hashtable->keys = calloc(size, sizeof(char *));
}

static void add_key(hashtable_s *hashtable, const char *key, unsigned int idx)
{
    if (hashtable->keys[idx] != NULL)
        free(hashtable->keys[idx]);

    hashtable->keys[idx] = strdup(key);
}

static void delete_key(hashtable_s *hashtable, unsigned int idx)
{
    if (hashtable->keys[idx] != NULL) {
        free(hashtable->keys[idx]);
        hashtable->keys[idx] = NULL;
    }
}

static void clear_hashtable(hashtable_s *hashtable)
{
    unsigned int i;

    for (i = 0; i < hashtable->size; i++) {
        if (hashtable->table[i] != NULL)
            hashtable->table[i] = NULL;

        delete_key(hashtable, i);
    }
}

static bool contains_key(hashtable_s *hashtable, const char *key)
{
    unsigned int i;

    /* Very poor search method */
    for (i = 0; i < hashtable->size; i++)
        if ((hashtable->keys[i] != NULL) &&
            (strcmp(hashtable->keys[i], key) == 0))
        {
            return true;
        }

    return false;
}

static bool contains_value(hashtable_s *hashtable, void *data)
{
    unsigned int i;

    /* Very poor search method */
    for (i = 0; i < hashtable->size; i++)
        if (NULL == hashtable->compare) {
            if (hashtable->table[i] == data)
                return true;
        } else
            if ((hashtable->compare)(hashtable->table[i], data))
                return true;

    return false;
}

static void destroy_hashtable_s(const struct cref_s *ref)
{
    hashtable_s *h = cl_container_of(ref, hashtable_s, ref);
    unsigned int i;

    if (NULL == h)
        return;

    if (h->keys != NULL) {
        for (i = 0; i < h->size; i++)
            if (h->keys[i] != NULL)
                free(h->keys[i]);

        free(h->keys);
    }

    if (h->table != NULL) {
        for (i = 0; i < h->size; i++)
            if ((h->table[i] != NULL) && (h->release != NULL))
                (h->release)(h->table[i]);

        free(h->table);
    }

    free(h);
}

static hashtable_s *new_hashtable_s(unsigned int size, bool replace_data,
    bool (*compare)(void *, void *), void (*release)(void *))
{
    hashtable_s *h = NULL;

    h = calloc(1, sizeof(hashtable_s));

    if (NULL == h) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    h->table = calloc(size, sizeof(void *));

    if (NULL == h->table) {
        free(h);
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    h->size = size;
    h->replace_data = replace_data;
    h->compare = compare;
    h->release = release;
    create_keys_storage(h, size);

    /* Reference count */
    h->ref.count = 1;
    h->ref.free = destroy_hashtable_s;

    set_typeof(CHASHTABLE, h);

    return h;
}

static unsigned short int hash16(unsigned char *k, unsigned int length,
    unsigned int initval)
{
    unsigned int a, b, c, len;

    len = length;
    a = b = 0x9e37; /* "random" value */
    c = initval;

    /* Handle a part of the key */
    while (len >= 12) {
        a += (k[0] + ((unsigned int)k[1] << 8) + ((unsigned int)k[2] << 16) +
                ((unsigned int)k[3] << 24));
        b += (k[4] + ((unsigned int)k[5] << 8) + ((unsigned int)k[6] << 16) +
                ((unsigned int)k[7] << 24));
        c += (k[8] + ((unsigned int)k[9] << 8) + ((unsigned int)k[10] << 16) +
                ((unsigned int)k[11] << 24));

        mix(a, b, c);
        k += 12;
        len -= 12;
    }

    /* Handle last 11 bytes */
    c += length;

    switch (len) {
        case 11:
            c += ((unsigned int)k[10] << 24);
        case 10:
            c += ((unsigned int)k[9] << 16);
        case 9:
            c += ((unsigned int)k[8] << 8);

        /* First c byte is reserved to the length */

        case 8:
            b += ((unsigned int)k[7] << 24);
        case 7:
            b += ((unsigned int)k[6] << 16);
        case 6:
            b += ((unsigned int)k[5] << 8);
        case 5:
            b += k[4];

        case 4:
            a += ((unsigned int)k[3] << 24);
        case 3:
            a += ((unsigned int)k[2] << 16);
        case 2:
            a += ((unsigned int)k[1] << 8);
        case 1:
            a += k[0];
    }

    mix(a, b, c);

    return (unsigned short int)(c & hashmask(16));
}

static int hashkey(const char *key, unsigned int hashtable_size)
{
    unsigned int old_hash = PRIME_NUMBER, rnd;

    rnd = hash16((unsigned char *)key, 16, old_hash);

    return (rnd % hashtable_size);
}

/*
 *
 * API
 *
 */

__PUB_API__ chashtable_t *chashtable_ref(chashtable_t *hashtable)
{
    hashtable_s *h = (hashtable_s *)hashtable;

    __clib_function_init__(true, hashtable, CHASHTABLE, NULL);
    cref_inc(&h->ref);

    return hashtable;
}

__PUB_API__ int chashtable_unref(chashtable_t *hashtable)
{
    hashtable_s *h = (hashtable_s *)hashtable;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);
    cref_dec(&h->ref);

    return 0;
}

__PUB_API__ chashtable_t *chashtable_init(unsigned int size, bool replace_data,
    bool (*compare)(void *, void *), void (*release)(void *))
{
    hashtable_s *h = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (size == 0) {
        cset_errno(CL_INVALID_VALUE);
        return NULL;
    }

    h = new_hashtable_s(size, replace_data, compare, release);

    if (NULL == h)
        return NULL;

    return h;
}

__PUB_API__ int chashtable_uninit(chashtable_t *hashtable)
{
    return chashtable_unref(hashtable);
}

__PUB_API__ void *chashtable_put(chashtable_t *hashtable, const char *key,
    void *data)
{
    hashtable_s *h;
    int idx = -1;
    void *ret = NULL;

    __clib_function_init__(true, hashtable, CHASHTABLE, NULL);

    if ((NULL == key) || (NULL == data)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    h = chashtable_ref(hashtable);
    idx = hashkey(key, h->size);

    if (idx < 0) {
        cset_errno(CL_INVALID_VALUE);
        goto end_block;
    } else if (h->table[idx] != NULL) {
        if (h->replace_data == false) {
            /* We don't handle collisions */
            cset_errno(CL_HASHTABLE_COLLISION);
            goto end_block;
        }
    }

    ret = h->table[idx];
    h->table[idx] = data;
    add_key(h, key, idx);

end_block:
    chashtable_unref(h);
    return ret;
}

__PUB_API__ void *chashtable_get(chashtable_t *hashtable, const char *key)
{
    hashtable_s *h;
    int idx = -1;
    void *ptr = NULL;

    __clib_function_init__(true, hashtable, CHASHTABLE, NULL);

    if (NULL == key) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    h = chashtable_ref(hashtable);
    idx = hashkey(key, h->size);

    if (idx < 0) {
        cset_errno(CL_INVALID_VALUE);
        goto end_block;
    }

    ptr = h->table[idx];

end_block:
    chashtable_unref(h);
    return ptr;
}

__PUB_API__ int chashtable_delete(chashtable_t *hashtable, const char *key)
{
    hashtable_s *h;
    int idx = -1, ret = -1;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);

    if (NULL == key) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    h = chashtable_ref(hashtable);
    idx = hashkey(key, h->size);

    if (idx < 0) {
        cset_errno(CL_INVALID_VALUE);
        goto end_block;
    }

    ret = 0;
    h->table[idx] = NULL;
    delete_key(h, idx);

end_block:
    chashtable_unref(h);
    return ret;
}

__PUB_API__ bool chashtable_contains_key(chashtable_t *hashtable,
    const char *key)
{
    hashtable_s *h;
    bool ret = false;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);

    if (NULL == key) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    h = chashtable_ref(hashtable);
    ret = contains_key(h, key);
    chashtable_unref(h);

    return ret;
}

__PUB_API__ int chashtable_size(chashtable_t *hashtable)
{
    hashtable_s *h;
    int size = -1;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);

    h = chashtable_ref(hashtable);
    size = count_values(h);
    chashtable_unref(h);

    return size;
}

__PUB_API__ int chashtable_clear(chashtable_t *hashtable)
{
    hashtable_s *h;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);

    h = chashtable_ref(hashtable);
    clear_hashtable(h);
    chashtable_unref(h);

    return 0;
}

__PUB_API__ bool chashtable_contains_value(chashtable_t *hashtable, void *data)
{
    hashtable_s *h;
    bool ret = false;

    __clib_function_init__(true, hashtable, CHASHTABLE, -1);

    if (NULL == data) {
        cset_errno(CL_NULL_ARG);
        return false;
    }

    h = chashtable_ref(hashtable);
    ret = contains_value(h, data);
    chashtable_unref(h);

    return ret;
}

__PUB_API__ clist_t *chashtable_keys(chashtable_t *hashtable, bool dup)
{
    hashtable_s *h;
    clist_t *keys = NULL;

    __clib_function_init__(true, hashtable, CHASHTABLE, NULL);
    keys = create_list_of_keys(dup);

    if (NULL == keys)
        return NULL;

    h = chashtable_ref(hashtable);
    add_keys_to_list_of_keys(keys, h, dup);
    chashtable_unref(h);

    return keys;
}

