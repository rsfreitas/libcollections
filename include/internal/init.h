
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Mar  8 08:06:47 2018
 * Project: libcollections
 *
 * Copyright (C) 2018 Rodrigo Freitas All rights reserved.
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

#ifndef _COLLECTIONS_INTERNAL_INIT_H
#define _COLLECTIONS_INTERNAL_INIT_H

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <init.h> directly; include <collections.h> instead."
# endif
#endif

/*
 * This macro is responsible to do internal library validations, such as
 * clearing the internal error code, checking if the library environment
 * was initiated and validating a specific object.
 *
 * Every exported function must call this macro at the begining.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 * @return_value: Since this macro may be called from within any exported
 *                function, its type of return value may vary. Here we
 *                tell what will be this value.
 */
#define __clib_function_init__(obj_validation, object, type, return_value)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return return_value;\
\
        if (obj_validation == true)\
            if (typeof_validate_object(object, type) == false)\
                return return_value;\
    } while (0);

/*
 * This a copy of the previous macro with a slightly modification.
 *
 * Every exported function that needs to validate an object passed as
 * arguments with an offset in its header must call this macro at the begining.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 * @offset: The object's offset to access its type.
 * @return_value: Since this macro may be called from within any exported
 *                function, its type of return value may vary. Here we
 *                tell what will be this value.
 */
#define __clib_function_init_ex__(obj_validation, object, type, offset,\
        return_value)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return return_value;\
\
        if (obj_validation == true)\
            if (typeof_validate_object_with_offset(object, type, offset) == false)\
                return return_value;\
    } while (0);

/*
 * Another variation of the previous macro. Here we don't have a return value.
 * This should be used from within void functions.
 *
 * @obj_validation: A boolean flag indicating if an object will be validated
 *                  here.
 * @object: A pointer to the object which may be validated or NULL if the
 *          validation won't be made.
 * @type: The type of the object, to validate it.
 */
#define __clib_function_init_ex2__(obj_validation, object, type)\
    do {\
        cerrno_clear();\
\
        if (library_initialized() == false)\
            return;\
\
        if (obj_validation == true)\
            if (typeof_validate_object(object, type) == false)\
                return;\
    } while (0);

bool library_initialized(void);
struct random_data *library_random_data(void);
cl_json_t *library_configuration(void);
const char *library_package_name(void);
char *library_file_mime_type(const char *filename);
char *library_buffer_mime_type(const unsigned char *buffer, unsigned int size);

#endif
