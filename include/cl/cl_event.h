
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:19 2015
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

#ifndef _COLLECTIONS_CL_EVENT_H
#define _COLLECTIONS_CL_EVENT_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_event.h> directly; include <collections.h> instead."
# endif
#endif

/* Tipos de operacoes de validacao */
enum event_validation {
    EVENT_VAL_EQUAL,
    EVENT_VAL_NOT_EQUAL,
    EVENT_VAL_GREATER,
    EVENT_VAL_GREATER_EQUAL,
    EVENT_VAL_LESS,
    EVENT_VAL_LESS_EQUAL,
    EVENT_VAL_CUSTOM
};

/* Tipo de execucao do evento */
enum event_execution {
    EVENT_EXEC_ONCE,
    EVENT_EXEC_UNLIMITED
};

/* Tipo de comparacoes entre as condicoes registradas */
enum event_comparison_type {
    EVENT_CMP_AND,
    EVENT_CMP_OR
};

/* Valores de retorno de uma funcao de validacao */
#define EVENT_VAL_RETURN_OK                 0
#define EVENT_VAL_RETURN_ERROR              -1

/**
 * @name cevent_init
 * @brief Gets a cevent_t object for a specific event monitoring.
 *
 * This functions creates and returns a cevent_t object used to store
 * conditions, that, when all of this conditions are met, a function \a event
 * will be executed, with \a arg as its argument.
 *
 * The number of times that this event may occur is defined by \a exec argument.
 * Allowing indicate an event that occurs only once (EVENT_EXEC_ONCE) or event
 * which can occur several times (EVENT_EXEC_UNLIMITED).
 *
 * The \a reset_conditions argument receives a pointer to a function responsable
 * for clear all registered conditions for such an event. This way, for an event
 * of type EVEN_EXEC_UNLIMITED, the library will call this function right after
 * the evente has been executed, allowing it to be executed again.
 *
 * @param [in] exec: Type of event execution.
 * @param [in] name: Identification name of the event.
 * @param [in] event: Real event function.
 * @param [in] arg: Argument passed to the event function.
 * @param [in] reset_conditions: Function called to clear all conditions related
 *                               to the event.
 * @param [in] reset_arg: Argument passed to the reset condition function.
 *
 * @return On success returns a cevent_t object or NULL otherwise.
 */
cevent_t *cevent_init(enum event_execution exec, const char *name,
                      void (*event)(void *), void *arg,
                      void (*reset_conditions)(void *), void *reset_arg);

/**
 * @name cevent_condition_register
 * @brief Adds a condition that requires to be attended for an event occur.
 *
 * To accomplish a condition, a function must return EVENT_VAL_RETURN_OK.
 *
 * @param [in] e: The cevent_t object.
 * @param [in] val: Validation type that will be added.
 * @param [in] cmp_type: Condition comparison type.
 * @param [in] id: Identification condition number.
 * @param [in] ptr: Pointer to the content that will be validated.
 * @param [in] value: Poniter to the value that the content must have so the
 *                    condition function may return EVENT_VAL_RETURN_OK.
 * @param [in] custom_v_function: Alternative validation function, in case none
 *                                of the functions implemented in the library are
 *                                sufficient for.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cevent_condition_register(cevent_t *e, enum event_validation val,
                              enum event_comparison_type cmp_type,
                              unsigned int id, void *ptr, void *value,
                              int (*custom_v_function)(void *, void *));

/**
 * @name cevent_condition_unregister
 * @brief Removes a validation condition to an event.
 *
 * @param [in] e: The cevent_t object of an event.
 * @param [in] cmp_type: Condition comparison type which will be removed.
 * @param [in] cond_id: Identification condition number.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cevent_condition_unregister(cevent_t *e, enum event_comparison_type cmp_type,
                                unsigned int cond_id);

/**
 * @name cevent_install
 * @brief Puts the event to run.
 *
 * If \a sort_by_id argument is true, the conditions list will be sorted so
 * that the with the lowest identification number has a higher priority and be
 * executed first.
 *
 * @param [in] e: The cevent_t object.
 * @param [in] sort_by_id: Boolean flag to indicate if the conditions list will
 *                         be sorted or not.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cevent_install(cevent_t *e, bool sort_by_id);

/**
 * @name event_uninstall
 * @brief Ends a specific event.
 *
 * This function must be called if the event is of EVENT_EXEC_UNLIMITED type.
 *
 * @param [in] e: The cevent_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cevent_uninstall(cevent_t *e);

#endif

