
/*
 * Description: Simple API for handle thread initialization.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>

#include <pthread.h>

#include "collections.h"

struct sync_data_s {
    enum cthread_type   type;
    enum cthread_state  state;
};

struct cthread_s {
    struct sync_data_s  sdata;
    pthread_t           thread_id;
    pthread_attr_t      attr;
    void                *user_data;
};

static struct cthread_s *new_thread_data(void *user_data)
{
    struct cthread_s *td = NULL;

    td = calloc(1, sizeof(struct cthread_s));

    if (NULL == td) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    td->user_data = user_data;

    return td;
}

static void destroy_thread_data(struct cthread_s *td)
{
    if (NULL == td)
        return;

    free(td);
}

static bool validate_thread_state(enum cthread_state state)
{
    if (state >= CL_THREAD_ST_MAX_STATE)
        return false;

    return true;
}

void LIBEXPORT *cthread_get_user_data(cthread_data_t *arg)
{
    struct cthread_s *td;

    cerrno_clear();

    if (NULL == arg) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    td = (struct cthread_s *)arg;

    return td->user_data;
}

int LIBEXPORT cthread_set_state(cthread_t *t, enum cthread_state state)
{
    struct cthread_s *td = (struct cthread_s *)t;

    cerrno_clear();

    if (NULL == td) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (validate_thread_state(state) == false) {
        cset_errno(CL_INVALID_STATE);
        return -1;
    }

    td->sdata.state = state;

    return 0;
}

int LIBEXPORT cthread_wait_startup(const cthread_t *t)
{
    struct cthread_s *td = (struct cthread_s *)t;

    cerrno_clear();

    if (NULL == td) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    while (td->sdata.state == CL_THREAD_ST_CREATED)
        cmsleep(10);

    if (td->sdata.state == CL_THREAD_ST_INIT_ERROR)
        return 1;

    return 0;
}

int LIBEXPORT cthread_destroy(cthread_t *t)
{
    struct cthread_s *td = (struct cthread_s *)t;

    cerrno_clear();

    if (NULL == td) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (td->sdata.type == CL_THREAD_JOINABLE)
        pthread_join(td->thread_id, NULL);

    destroy_thread_data(td);

    return 0;
}

cthread_t LIBEXPORT *cthread_create(enum cthread_type type,
    void *(*start_routine)(cthread_data_t *), void *user_data)
{
    struct cthread_s *td = NULL;
    int detachstate = PTHREAD_CREATE_JOINABLE;

    cerrno_clear();
    td = new_thread_data(user_data);

    if (NULL == td)
        return NULL;

    if (type == CL_THREAD_DETACHED)
        detachstate = PTHREAD_CREATE_DETACHED;

    pthread_attr_init(&td->attr);
    pthread_attr_setdetachstate(&td->attr, detachstate);
    cthread_set_state(td, CL_THREAD_ST_CREATED);

    if (pthread_create(&td->thread_id, &td->attr, start_routine, td) < 0) {
        destroy_thread_data(td);
        cset_errno(CL_CREATE_FAILED);
        return NULL;
    }

    return td;
}

