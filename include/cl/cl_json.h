
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

#ifndef _COLLECTIONS_CL_JSON_H
#define _COLLECTIONS_CL_JSON_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_json.h> directly; include <collections.h> instead."
# endif
#endif

enum cjson_type {
    CJSON_STRING,
    CJSON_NUMBER,
    CJSON_NUMBER_FLOAT,
    CJSON_OBJECT,
    CJSON_ARRAY,
    CJSON_TRUE,
    CJSON_FALSE,
    CJSON_NULL
};

/**
 * @name cjson_parse
 * @brief Parse a string containing a JSON data.
 *
 * @param [in] string: The string containing a JSON data.
 *
 * @return On success returns a cjson_t object containing the JSON data or
 *         NULL otherwise.
 */
cjson_t *cjson_parse(const cstring_t *string);

/**
 * @name cjson_read_file
 * @brief Loads a JSON file to a cjson_t object.
 *
 * @param [in] filename: The file name.
 *
 * @return On success returns a cjson_t object containing the JSON from the
 *         file or NULL otherwise.
 */
cjson_t *cjson_read_file(const char *filename);

/**
 * @name cjson_write_file
 * @brief Saves a cjson_t object into a text file.
 *
 * @param [in] j: The cjson_t object which will be saved.
 * @param [in] filename: The file name.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_write_file(const cjson_t *j, const char *filename);

/**
 * @name cjson_delete
 * @brief Releases a cjson_t object from memory.
 *
 * @param [in,out] j: The cjson_t object which will be released.
 */
void cjson_delete(cjson_t *j);

/**
 * @name cjson_get_array_size
 * @brief Gets the size of a cjson_t array of elements.
 *
 * @param [in] array: The cjson_t array.
 *
 * @return On success returns the number of elements from the array or -1
 *         otherwise.
 */
int cjson_get_array_size(const cjson_t *array);

/**
 * @name cjson_get_array_item
 * @brief Gets a pointer to an element from a cjson_t array.
 *
 * @param [in] array: The cjson_t array.
 * @param [in] item: The position of the element from the array.
 *
 * @return On success returns a pointer to the element or NULL otherwise.
 */
cjson_t *cjson_get_array_item(const cjson_t *array, unsigned int item);

/**
 * @name cjson_get_object_item
 * @brief Gets an object from a cjson_t object with a specific name.
 *
 * @param [in] json: The cjson_t object.
 * @param [in] name: The name of the searched object.
 *
 * @return On success returns a pointer to the object found or NULL otherwise.
 */
cjson_t *cjson_get_object_item(const cjson_t *json, const char *name);

/**
 * @name cjson_get_object_name
 * @brief Gets the name of an object.
 *
 * @param [in] o: The cjson_t object.
 *
 * @return On success returns the name of the object or NULL otherwise.
 */
cstring_t *cjson_get_object_name(const cjson_t *o);

/**
 * @name cjson_get_object_value
 * @brief Gets the value of an object.
 *
 * @param [in] o: The cjson_t object.
 *
 * @return On success returns the value of the object or NULL otherwise.
 */
cstring_t *cjson_get_object_value(const cjson_t *o);

/**
 * @name cjson_get_object_type
 * @brief Gets the type of an object.
 *
 * @param [in] o: The cjson_t object.
 *
 * @return On success returns the type of the object or -1 otherwise.
 */
enum cjson_type cjson_get_object_type(const cjson_t *o);

/**
 * @name cjson_dup
 * @brief Duplicates a cjson_t object.
 *
 * @param [in] root: cjson_t object which will be duplicated.
 *
 * @return On success returns the duplicated cjson_t object or NULL otherwise.
 */
cjson_t *cjson_dup(const cjson_t *root);

/**
 * @name cjson_create_array
 * @brief Creates a cjson_t of type CJSON_ARRAY.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_array(void);

/**
 * @name cjson_create_object
 * @brief Creates a cjson_t of type CJSON_OBJECT.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_object(void);

/**
 * @name cjson_create_null
 * @brief Creates a cjson_t of type CJSON_NULL.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_null(void);

/**
 * @name cjson_create_false
 * @brief Creates a cjson_t of type CJSON_FALSE.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_false(void);

/**
 * @name cjson_create_true
 * @brief Creates a cjson_t of type CJSON_TRUE.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_true(void);

/**
 * @name cjson_create_number
 * @brief Creates a cjson_t of type CJSON_NUMBER.
 *
 * @param [in] n: The value of the new cjson_t object.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_number(int n);

/**
 * @name cjson_create_number_float
 * @brief Creates a cjson_t of type CJSON_NUMBER_FLOAT.
 *
 * @param [in] n: The value of the new cjson_t object.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_number_float(float n);

/**
 * @name cjson_create_string
 * @brief Creates a cjson_t of type CJSON_STRING.
 *
 * @param [in] string: The value of the new cjson_t object.
 *
 * @return On success returns the created cjson_t object or NULL otherwise.
 */
cjson_t *cjson_create_string(const char *string);

/**
 * @name cjson_create_int_array
 * @brief Creates an array of objects of type CJSON_NUMBER.
 *
 * @param [in] values: The values of each element of the array.
 * @param [in] size: The number of elements of the array.
 *
 * @return On success returns the created cjson_t array or NULL otherwise.
 */
cjson_t *cjson_create_int_array(const int *values, int size);

/**
 * @name cjson_create_float_array
 * @brief Creates an array of objects of type CJSON_NUMBER_FLOAT.
 *
 * @param [in] values: The values of each element of the array.
 * @param [in] size: The number of elements of the array.
 *
 * @return On success returns the created cjson_t array or NULL otherwise.
 */
cjson_t *cjson_create_float_array(const float *values, int size);

/**
 * @name cjson_create_string_array
 * @brief Creates an array of object of type CJSON_STRING.
 *
 * @param [in] values: The cstring_list_t containing the values of each element
 *                     of the array.
 *
 * @return On success returns the created cjson_t array or NULL otherwise.
 */
cjson_t *cjson_create_string_array(const cstring_list_t *values);

/**
 * @name cjson_add_item_to_array
 * @brief Add an item in an array.
 *
 * @param [in] array: The cjson_t array.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_add_item_to_array(cjson_t *array, const cjson_t *item);

/**
 * @name cjson_add_item_to_object
 * @brief Add an item in an object.
 *
 * @param [in] root: The cjson_t root.
 * @param [in] name: The name of the item.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_add_item_to_object(cjson_t *root, const char *name, cjson_t *item);

/**
 * @name cjson_add_item_reference_to_array
 * @brief Add a reference to an item in an array.
 *
 * @param [in] root: The cjson_t array.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_add_item_reference_to_array(cjson_t *array, cjson_t *item);

/**
 * @name cjson_add_item_reference_to_object
 * @brief Add a reference to an item in an array.
 *
 * @param [in] root: The cjson_t root.
 * @param [in] name: The name of the item.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_add_item_reference_to_object(cjson_t *root, const char *name,
                                       cjson_t *item);

/**
 * @name cjson_delete_item_from_array
 * @brief Deletes an item from an array.
 *
 * @param [in] array: The cjson_t array.
 * @param [in] index: The index of the item which will be deleted.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_delete_item_from_array(cjson_t *array, unsigned int index);

/**
 * @name cjson_delete_item_from_object
 * @brief Deletes an item from an object.
 *
 * @param [in] json: The cjson_t object.
 * @param [in] name: The name of the deleted item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_delete_item_from_object(cjson_t *json, const char *name);

/**
 * @name cjson_replace_item_in_array
 * @brief Replaces an item in an array.
 *
 * @param [in] array: The cjson_t array.
 * @param [in] index: The index of the replaced item.
 * @param [in] new_item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_replace_item_in_array(cjson_t *array, unsigned int index,
                                cjson_t *new_item);

/**
 * @name cjson_replace_item_in_object
 * @brief Replaces an item in an object.
 *
 * @param [in] root: The cjson_t object.
 * @param [in] name: The name of the replaced object.
 * @param [in] new_item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cjson_replace_item_in_object(cjson_t *root, const char *name,
                                 cjson_t *new_item);

/**
 * @name cjson_to_string
 * @brief Converts a cjson_t object into a human readable string.
 *
 * @param [in] j: The cjson_t object.
 * @param [in] friendly_output: Boolean flag to format or not the output string
 *                              in a user friendly format.
 *
 * @return On success returns a cstring_t containing the cjson_t object.
 */
cstring_t *cjson_to_string(const cjson_t *j, bool friendly_output);

#endif

