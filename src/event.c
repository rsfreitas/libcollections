
/*
 * Description: API for monitoring events inside an application.
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

#include <pthread.h>

#include "collections.h"

struct event_condition_s {
    clist_entry_t               *prev;
    clist_entry_t               *next;
    unsigned int                id;
    void                        *ptr;
    void                        *value;
    enum cl_event_validation      validation;
    int                         (*v_function)(void *, void *);
};

#define cevent_members                                              \
    cl_struct_member(struct event_condition_s *, evc_or)            \
    cl_struct_member(struct event_condition_s *, evc_and)           \
    cl_struct_member(pthread_t, t_id)                               \
    cl_struct_member(pthread_mutex_t, m_evc)                        \
    cl_struct_member(enum cl_event_execution, exec_type)               \
    cl_struct_member(char *, name)                                  \
    cl_struct_member(bool, sort)                                    \
    cl_struct_member(bool, end_thread)                              \
    cl_struct_member(int, total_or_cond)                            \
    cl_struct_member(int, total_and_cond)                           \
    cl_struct_member(void, (*ev_function)(void *))                  \
    cl_struct_member(void *, arg)                                   \
    cl_struct_member(void, (*reset_cond)(void *))                   \
    cl_struct_member(void *, reset_arg)

cl_struct_declare(cevent_s, cevent_members);

#define cevent_s        cl_struct(cevent_s)

bool validate_execution_type(enum cl_event_execution exec_type)
{
    if (exec_type > CEVENT_EXEC_UNLIMITED)
        return false;

    return true;
}

bool validate_validation_type(enum cl_event_validation val)
{
    if (val > CEVENT_VAL_CUSTOM)
        return false;

    return true;
}

bool validate_comparison_type(enum cl_event_comparison_type cmp_type)
{
    if ((cmp_type != CEVENT_CMP_AND) && (cmp_type != CEVENT_CMP_OR))
        return false;

    return true;
}

bool validate_conditions(cevent_s *ev)
{
    /*
     * If they are being used together, the condition lists must have,
     * mandatorily, more than one element.
     */
    if ((ev->total_or_cond == 1) || (ev->total_and_cond == 1))
        return false;

    return true;
}

static struct event_condition_s *new_event_condition(void)
{
    struct event_condition_s *c = NULL;

    c = calloc(1, sizeof(struct event_condition_s));

    if (NULL == c) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    return c;
}

static cevent_s *new_event(void)
{
    cevent_s *e = NULL;

    e = calloc(1, sizeof(cevent_s));

    if (NULL == e) {
        cset_errno(CL_NO_MEM);
        return NULL;
    }

    e->evc_or = NULL;
    e->evc_and = NULL;
    e->reset_cond = NULL;
    e->reset_arg = NULL;

    set_typeof(CEVENT, e);
    pthread_mutex_init(&e->m_evc, NULL);

    return e;
}

static void destroy_event(cevent_s *ev)
{
    if (ev->name != NULL)
        free(ev->name);

    if (ev->evc_or != NULL)
        cdll_free(ev->evc_or, NULL);

    if (ev->evc_and != NULL)
        cdll_free(ev->evc_and, NULL);

    free(ev);
}

static void update_total_conditions(cevent_s *ev)
{
    ev->total_or_cond = cdll_size(ev->evc_or);
    ev->total_and_cond = cdll_size(ev->evc_and);
}

static int call_validation_function(void *a, void *b __attribute__((unused)))
{
    struct event_condition_s *evc = (struct event_condition_s *)a;

    return (evc->v_function)(evc->ptr, evc->value);
}

static bool or_validation(cevent_s *ev)
{
    struct event_condition_s *p = NULL;

    for (p = ev->evc_or; p; p = p->next)
        if (call_validation_function(p, NULL) == CEVENT_VAL_RETURN_OK) {
            /* Returns true on the first function that returns Ok value. */
            return true;
        }

    return false;
}

static bool and_validation(cevent_s *ev)
{
    struct event_condition_s *ret = NULL;

    /*
     * If any internal function returns a value other than zero, i.e.,
     * CEVENT_VAL_RETURN_ERROR, the function 'cdll_map' returns a pointer to
     * that element on the list.
     */
    ret = cdll_map(ev->evc_and, call_validation_function, NULL);

    return (ret == NULL) ? true : false;
}

/*
 * Call validation functions previously registered.
 */
static bool may_call_event(cevent_s *ev)
{
    bool st_or = false, st_and = false;

    pthread_mutex_lock(&ev->m_evc);

    if (ev->total_or_cond)
        st_or = or_validation(ev);

    if (ev->total_and_cond)
        st_and = and_validation(ev);

    pthread_mutex_unlock(&ev->m_evc);

    /*
     * XXX: Is this the best way???
     */
    if ((ev->total_and_cond && (st_and == true)) ||
        (ev->total_or_cond && (st_or == true)))
    {
        return true;
    }

    return false;
}

static void call_event(cevent_s *ev)
{
    (ev->ev_function)(ev->arg);
}

static void reset_event_conditions(cevent_s *ev)
{
    if (ev->reset_cond != NULL)
        (ev->reset_cond)(ev->reset_arg);
}

static void *cevent_thread(void *param)
{
    cevent_s *event;
    bool event_executed = false;

    event = (cevent_s *)param;

    while (event_executed == false) {
        cmsleep(1);

        if (may_call_event(event) == true) {
            call_event(event);
            reset_event_conditions(event);
            event_executed = true;
        }

        /* If the event is of CEVENT_EXEC_UNLIMITED type waits again. */
        if ((event_executed == true) &&
            (event->exec_type == CEVENT_EXEC_UNLIMITED))
        {
            event_executed = false;
        }

        if (event->end_thread == true)
            break;
    }

    /*
     * If is a unique event, releases memory, preventing the user for having
     * to do that.
     */
    if (event->exec_type == CEVENT_EXEC_ONCE)
        destroy_event(event);

    return NULL;
}

static int cmp_condition(void *a, void *b)
{
    struct event_condition_s *ev1 = (struct event_condition_s *)a;
    struct event_condition_s *ev2 = (struct event_condition_s *)b;

    if (ev2->id < ev1->id)
        return -1;
    else if (ev2->id == ev1->id)
        return 0;

    return 1;
}

static void sort_event_conditions(cevent_s *ev)
{
    pthread_mutex_lock(&ev->m_evc);

    if (ev->total_or_cond)
        ev->evc_or = cdll_mergesort(ev->evc_or, cmp_condition);

    if (ev->total_and_cond)
        ev->evc_and = cdll_mergesort(ev->evc_and, cmp_condition);

    pthread_mutex_unlock(&ev->m_evc);
}

static int filter_condition(void *a, void *b)
{
    struct event_condition_s *evc = (struct event_condition_s *)a;
    unsigned int *cond_id = (unsigned int *)b;

    if (evc->id == *cond_id)
        return 1;

    return 0;
}

/* ------ Validation functions ------ */

static int v_eq(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr == *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

static int v_ne(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr != *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

static int v_gt(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr > *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

static int v_ge(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr >= *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

static int v_lt(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr < *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

static int v_le(void *ptr, void *value)
{
    int *tptr = ptr, *tvalue = value;

    if (*tptr <= *tvalue)
        return CEVENT_VAL_RETURN_OK;

    return CEVENT_VAL_RETURN_ERROR;
}

__PUB_API__ cevent_t *cevent_init(enum cl_event_execution exec, const char *name,
    void (*event)(void *), void *arg, void (*reset_conditions)(void *),
    void *reset_arg)
{
    cevent_s *ev = NULL;

    __clib_function_init__(false, NULL, -1, NULL);

    if (validate_execution_type(exec) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return NULL;
    }

    if ((NULL == name) || (NULL == *event)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    ev = new_event();

    if (NULL == ev)
        return NULL;

    ev->ev_function = event;
    ev->arg = arg;
    ev->exec_type = exec;
    ev->name = strdup(name);
    ev->end_thread = false;
    ev->reset_cond = reset_conditions;
    ev->reset_arg = reset_arg;

    return ev;
}

__PUB_API__ int cevent_condition_register(cevent_t *e,
    enum cl_event_validation val, enum cl_event_comparison_type cmp_type,
    unsigned int id, void *ptr, void *value,
    int (*custom_v_function)(void *, void *))
{
    cevent_s *ev = (cevent_s *)e;
    struct event_condition_s *c = NULL;

    __clib_function_init__(true, e, CEVENT, -1);

    if (NULL == ptr) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (validate_validation_type(val) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    if (validate_comparison_type(cmp_type) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    c = new_event_condition();

    if (NULL == c)
        return -1;

    switch (val) {
        case CEVENT_VAL_EQUAL:
            c->v_function = v_eq;
            break;

        case CEVENT_VAL_NOT_EQUAL:
            c->v_function = v_ne;
            break;

        case CEVENT_VAL_GREATER:
            c->v_function = v_gt;
            break;

        case CEVENT_VAL_GREATER_EQUAL:
            c->v_function = v_ge;
            break;

        case CEVENT_VAL_LESS:
            c->v_function = v_lt;
            break;

        case CEVENT_VAL_LESS_EQUAL:
            c->v_function = v_le;
            break;

        case CEVENT_VAL_CUSTOM:
            c->v_function = custom_v_function;
            break;
    }

    c->ptr = ptr;
    c->value = value;
    c->validation = val;
    c->id = id;

    if (cmp_type == CEVENT_CMP_AND)
        ev->evc_and = cdll_unshift(ev->evc_and, c);
    else if (cmp_type == CEVENT_CMP_OR)
        ev->evc_or = cdll_unshift(ev->evc_or, c);

    update_total_conditions(ev);

    if (ev->sort == true)
        sort_event_conditions(ev);

    return 0;
}

__PUB_API__ int cevent_condition_unregister(cevent_t *e,
    enum cl_event_comparison_type cmp_type, unsigned int cond_id)
{
    cevent_s *ev = (cevent_s *)e;
    struct event_condition_s *evc = NULL, *list = NULL;
    unsigned int *p_id = &cond_id;

    __clib_function_init__(true, e, CEVENT, -1);

    if (validate_comparison_type(cmp_type) == false) {
        cset_errno(CL_UNSUPPORTED_TYPE);
        return -1;
    }

    pthread_mutex_lock(&ev->m_evc);

    if (cmp_type == CEVENT_CMP_AND)
        list = ev->evc_and;
    else if (cmp_type == CEVENT_CMP_OR)
        list = ev->evc_or;

    evc = cdll_filter(list, filter_condition, p_id);

    pthread_mutex_unlock(&ev->m_evc);

    if (NULL == evc) {
        cset_errno(CL_OBJECT_NOT_FOUND);
        return -1;
    }

    free(evc);
    return 0;
}

__PUB_API__ int cevent_install(cevent_t *e, bool sort_by_id)
{
    cevent_s *ev = (cevent_s *)e;
    pthread_attr_t t_attr;
    int detachstate = PTHREAD_CREATE_JOINABLE;

    __clib_function_init__(true, e, CEVENT, -1);

    /* Checks if the conditions were correctly inserted. */
    if (validate_conditions(ev) == false) {
        cset_errno(CL_EVENT_CONDITIONS_WRONGLY_INITIALIZED);
        return -1;
    }

    /*
     * All condition added after calling this function will make us sort
     * the conditions list again.
     */
    if (sort_by_id == true)
        sort_event_conditions(ev);

    ev->sort = sort_by_id;

    /*
     * Starts the thread (detachable if is a unique execution) to conditions
     * evaluation.
     */
    if (ev->exec_type == CEVENT_EXEC_ONCE)
        detachstate = PTHREAD_CREATE_DETACHED;

    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, detachstate);

    if (pthread_create(&ev->t_id, &t_attr, (void *)cevent_thread, ev) < 0) {
        cset_errno(CL_CREATE_FAILED);
        return -1;
    }

    return 0;
}

__PUB_API__ int cevent_uninstall(cevent_t *e)
{
    cevent_s *ev = (cevent_s *)e;

    __clib_function_init__(true, e, CEVENT, -1);

    if (ev->exec_type == CEVENT_EXEC_UNLIMITED) {
        ev->end_thread = true;
        pthread_join(ev->t_id, NULL);
    }

    destroy_event(ev);

    return 0;
}

