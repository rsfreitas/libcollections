
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:19 2015
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

#ifndef _COLLECTIONS_API_EVENT_H
#define _COLLECTIONS_API_EVENT_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <event.h> directly; include <collections.h> instead."
# endif
#endif

/* Tipos de operacoes de validacao */
enum cl_event_validation {
    CL_EVENT_VAL_EQUAL,
    CL_EVENT_VAL_NOT_EQUAL,
    CL_EVENT_VAL_GREATER,
    CL_EVENT_VAL_GREATER_EQUAL,
    CL_EVENT_VAL_LESS,
    CL_EVENT_VAL_LESS_EQUAL,
    CL_EVENT_VAL_CUSTOM
};

/* Tipo de execucao do evento */
enum cl_event_execution {
    CL_EVENT_EXEC_ONCE,
    CL_EVENT_EXEC_UNLIMITED
};

/* Tipo de comparacoes entre as condicoes registradas */
enum cl_event_comparison_type {
    CL_EVENT_CMP_AND,
    CL_EVENT_CMP_OR
};

/* Valores de retorno de uma funcao de validacao */
#define CL_EVENT_VAL_RETURN_OK                 0
#define CL_EVENT_VAL_RETURN_ERROR              -1

/**
 * @name cl_event_init
 * @brief Gets a cl_event_t object for a specific event monitoring.
 *
 * This functions creates and returns a cl_event_t object used to store
 * conditions, that, when all of this conditions are met, a function \a event
 * will be executed, with \a arg as its argument.
 *
 * The number of times that this event may occur is defined by \a exec argument.
 * Allowing indicate an event that occurs only once (CL_EVENT_EXEC_ONCE) or event
 * which can occur several times (CL_EVENT_EXEC_UNLIMITED).
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
 * @return On success returns a cl_event_t object or NULL otherwise.
 */
cl_event_t *cl_event_init(enum cl_event_execution exec, const char *name,
                          void (*event)(void *), void *arg,
                          void (*reset_conditions)(void *), void *reset_arg);

/**
 * @name cl_event_condition_register
 * @brief Adds a condition that requires to be attended for an event occur.
 *
 * To accomplish a condition, a function must return CL_EVENT_VAL_RETURN_OK.
 *
 * @param [in] e: The cl_event_t object.
 * @param [in] val: Validation type that will be added.
 * @param [in] cmp_type: Condition comparison type.
 * @param [in] id: Identification condition number.
 * @param [in] ptr: Pointer to the content that will be validated.
 * @param [in] value: Poniter to the value that the content must have so the
 *                    condition function may return CL_EVENT_VAL_RETURN_OK.
 * @param [in] custom_v_function: Alternative validation function, in case none
 *                                of the functions implemented in the library are
 *                                sufficient for.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_event_condition_register(cl_event_t *e, enum cl_event_validation val,
                                enum cl_event_comparison_type cmp_type,
                                unsigned int id, void *ptr, void *value,
                                int (*custom_v_function)(void *, void *));

/**
 * @name cl_event_condition_unregister
 * @brief Removes a validation condition to an event.
 *
 * @param [in] e: The cl_event_t object of an event.
 * @param [in] cmp_type: Condition comparison type which will be removed.
 * @param [in] cond_id: Identification condition number.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_event_condition_unregister(cl_event_t *e,
                                  enum cl_event_comparison_type cmp_type,
                                  unsigned int cond_id);

/**
 * @name cl_event_install
 * @brief Puts the event to run.
 *
 * If \a sort_by_id argument is true, the conditions list will be sorted so
 * that the with the lowest identification number has a higher priority and be
 * executed first.
 *
 * @param [in] e: The cl_event_t object.
 * @param [in] sort_by_id: Boolean flag to indicate if the conditions list will
 *                         be sorted or not.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_event_install(cl_event_t *e, bool sort_by_id);

/**
 * @name event_uninstall
 * @brief Ends a specific event.
 *
 * This function must be called if the event is of CL_EVENT_EXEC_UNLIMITED type.
 *
 * @param [in] e: The cl_event_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_event_uninstall(cl_event_t *e);

#endif

