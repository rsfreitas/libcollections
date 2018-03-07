
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

#ifndef _COLLECTIONS_API_JSON_H
#define _COLLECTIONS_API_JSON_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <json.h> directly; include <collections.h> instead."
# endif
#endif

enum cl_json_type {
    CL_JSON_STRING,
    CL_JSON_NUMBER,
    CL_JSON_NUMBER_FLOAT,
    CL_JSON_OBJECT,
    CL_JSON_ARRAY,
    CL_JSON_TRUE,
    CL_JSON_FALSE,
    CL_JSON_NULL
};

/**
 * @name cl_json_parse_string
 * @brief Parse a C string containing a JSON data.
 *
 * @param [in] string: The C string containing a JSON data.
 *
 * @return On success returns a cl_json_t object containing the JSON data or
 *         NULL otherwise.
 */
cl_json_t *cl_json_parse_string(const char *string);

/**
 * @name cl_json_parse
 * @brief Parse a string containing a JSON data.
 *
 * @param [in] string: The string containing a JSON data.
 *
 * @return On success returns a cl_json_t object containing the JSON data or
 *         NULL otherwise.
 */
cl_json_t *cl_json_parse(const cl_string_t *string);

/**
 * @name cl_json_read_file
 * @brief Loads a JSON file to a cl_json_t object.
 *
 * @param [in] filename: The file name.
 *
 * @return On success returns a cl_json_t object containing the JSON from the
 *         file or NULL otherwise.
 */
cl_json_t *cl_json_read_file(const char *filename);

/**
 * @name cl_json_write_file
 * @brief Saves a cl_json_t object into a text file.
 *
 * @param [in] j: The cl_json_t object which will be saved.
 * @param [in] filename: The file name.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_write_file(const cl_json_t *j, const char *filename);

/**
 * @name cl_json_delete
 * @brief Releases a cl_json_t object from memory.
 *
 * @param [in,out] j: The cl_json_t object which will be released.
 */
void cl_json_delete(cl_json_t *j);

/**
 * @name cl_json_get_array_size
 * @brief Gets the size of a cl_json_t array of elements.
 *
 * @param [in] array: The cl_json_t array.
 *
 * @return On success returns the number of elements from the array or -1
 *         otherwise.
 */
int cl_json_get_array_size(const cl_json_t *array);

/**
 * @name cl_json_get_array_item_by_name
 * @brief Gets a pointer to an element from a cl_json_t array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] name: The array element name.
 *
 * @return On success returns a pointer to the element or NULL otherwise.
 */
cl_json_t *cl_json_get_array_item_by_name(const cl_json_t *array,
                                          const char *name);

/**
 * @name cl_json_get_array_item
 * @brief Gets a pointer to an element from a cl_json_t array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] item: The position of the element from the array.
 *
 * @return On success returns a pointer to the element or NULL otherwise.
 */
cl_json_t *cl_json_get_array_item(const cl_json_t *array, unsigned int item);

/**
 * @name cl_json_get_object_item
 * @brief Gets an object from a cl_json_t object with a specific name.
 *
 * @param [in] json: The cl_json_t object.
 * @param [in] name: The name of the searched object.
 *
 * @return On success returns a pointer to the object found or NULL otherwise.
 */
cl_json_t *cl_json_get_object_item(const cl_json_t *json, const char *name);

/**
 * @name cl_json_get_object_name
 * @brief Gets the name of an object.
 *
 * @param [in] o: The cl_json_t object.
 *
 * @return On success returns the name of the object or NULL otherwise.
 */
cl_string_t *cl_json_get_object_name(const cl_json_t *o);

/**
 * @name cl_json_get_object_value
 * @brief Gets the value of an object.
 *
 * @param [in] o: The cl_json_t object.
 *
 * @return On success returns the value of the object or NULL otherwise.
 */
cl_string_t *cl_json_get_object_value(const cl_json_t *o);

/**
 * @name cl_json_get_object_type
 * @brief Gets the type of an object.
 *
 * @param [in] o: The cl_json_t object.
 *
 * @return On success returns the type of the object or -1 otherwise.
 */
enum cl_json_type cl_json_get_object_type(const cl_json_t *o);

/**
 * @name cl_json_dup
 * @brief Duplicates a cl_json_t object.
 *
 * @param [in] root: cl_json_t object which will be duplicated.
 *
 * @return On success returns the duplicated cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_dup(const cl_json_t *root);

/**
 * @name cl_json_create_array
 * @brief Creates a cl_json_t of type CL_JSON_ARRAY.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_array(void);

/**
 * @name cl_json_create_object
 * @brief Creates a cl_json_t of type CL_JSON_OBJECT.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_object(void);

/**
 * @name cl_json_create_null
 * @brief Creates a cl_json_t of type CL_JSON_NULL.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_null(void);

/**
 * @name cl_json_create_false
 * @brief Creates a cl_json_t of type CL_JSON_FALSE.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_false(void);

/**
 * @name cl_json_create_true
 * @brief Creates a cl_json_t of type CL_JSON_TRUE.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_true(void);

/**
 * @name cl_json_create_number
 * @brief Creates a cl_json_t of type CL_JSON_NUMBER.
 *
 * @param [in] n: The value of the new cl_json_t object.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_number(int n);

/**
 * @name cl_json_create_number_float
 * @brief Creates a cl_json_t of type CL_JSON_NUMBER_FLOAT.
 *
 * @param [in] n: The value of the new cl_json_t object.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_number_float(float n);

/**
 * @name cl_json_create_string
 * @brief Creates a cl_json_t of type CL_JSON_STRING.
 *
 * @param [in] string: The value of the new cl_json_t object.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_string(const char *string);

/**
 * @name cl_json_create_node
 * @brief Creates a JSON node of a certain type.
 *
 * @param [in] type: The node type.
 * @param [in] fmt: The content format.
 * @param [in] ...: The real content.
 *
 * @return On success returns the created cl_json_t object or NULL otherwise.
 */
cl_json_t *cl_json_create_node(enum cl_json_type type, const char *fmt, ...);

/**
 * @name cl_json_create_int_array
 * @brief Creates an array of objects of type CL_JSON_NUMBER.
 *
 * @param [in] values: The values of each element of the array.
 * @param [in] size: The number of elements of the array.
 *
 * @return On success returns the created cl_json_t array or NULL otherwise.
 */
cl_json_t *cl_json_create_int_array(const int *values, int size);

/**
 * @name cl_json_create_float_array
 * @brief Creates an array of objects of type CL_JSON_NUMBER_FLOAT.
 *
 * @param [in] values: The values of each element of the array.
 * @param [in] size: The number of elements of the array.
 *
 * @return On success returns the created cl_json_t array or NULL otherwise.
 */
cl_json_t *cl_json_create_float_array(const float *values, int size);

/**
 * @name cl_json_create_string_array
 * @brief Creates an array of object of type CL_JSON_STRING.
 *
 * @param [in] values: The cl_stringlist_t containing the values of each element
 *                     of the array.
 *
 * @return On success returns the created cl_json_t array or NULL otherwise.
 */
cl_json_t *cl_json_create_string_array(const cl_stringlist_t *values);

/**
 * @name cl_json_add_item_to_array
 * @brief Add an item in an array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_add_item_to_array(cl_json_t *array, const cl_json_t *item);

/**
 * @name cl_json_add_item_to_object
 * @brief Add an item in an object.
 *
 * @param [in] root: The cl_json_t root.
 * @param [in] name: The name of the item.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_add_item_to_object(cl_json_t *root, const char *name,
                               cl_json_t *item);

/**
 * @name cl_json_add_item_reference_to_array
 * @brief Add a reference to an item in an array.
 *
 * @param [in] root: The cl_json_t array.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_add_item_reference_to_array(cl_json_t *array, cl_json_t *item);

/**
 * @name cl_json_add_item_reference_to_object
 * @brief Add a reference to an item in an array.
 *
 * @param [in] root: The cl_json_t root.
 * @param [in] name: The name of the item.
 * @param [in] item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_add_item_reference_to_object(cl_json_t *root, const char *name,
                                         cl_json_t *item);

/**
 * @name cl_json_delete_item_from_array
 * @brief Deletes an item from an array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] index: The index of the item which will be deleted.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_delete_item_from_array(cl_json_t *array, unsigned int index);

/**
 * @name cl_json_delete_item_from_object
 * @brief Deletes an item from an object.
 *
 * @param [in] json: The cl_json_t object.
 * @param [in] name: The name of the deleted item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_delete_item_from_object(cl_json_t *json, const char *name);

/**
 * @name cl_json_replace_item_in_array
 * @brief Replaces an item in an array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] index: The index of the replaced item.
 * @param [in] new_item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_replace_item_in_array(cl_json_t *array, unsigned int index,
                                  cl_json_t *new_item);

/**
 * @name cl_json_replace_item_in_object
 * @brief Replaces an item in an object.
 *
 * @param [in] root: The cl_json_t object.
 * @param [in] name: The name of the replaced object.
 * @param [in] new_item: The new item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_replace_item_in_object(cl_json_t *root, const char *name,
                                   cl_json_t *new_item);

/**
 * @name cl_json_to_cstring
 * @brief Converts a cl_json_t object into a human readable string.
 *
 * @param [in] j: The cl_json_t object.
 * @param [in] friendly_output: Boolean flag to format or not the output string
 *                              in a user friendly format.
 *
 * @return On success returns a cl_string_t containing the cl_json_t object.
 */
cl_string_t *cl_json_to_cstring(const cl_json_t *j, bool friendly_output);

/**
 * @name cl_json_to_string
 * @brief Converts a cl_json_t object into a human readable C string.
 *
 * @param [in] j: The cl_json_t object.
 * @param [in] friendly_output: Boolean flag to format or not the output string
 *                              in a user friendly format.
 *
 * @return On success returns a C string containing the cl_json_t object or NULL
 *         otherwise.
 */
char *cl_json_to_string(const cl_json_t *j, bool friendly_output);

/**
 * @name cl_json_delete_item_from_array_by_name
 * @brief Deletes an item from an array.
 *
 * @param [in] array: The cl_json_t array.
 * @param [in] name: The name of the item which will be deleted.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_json_delete_item_from_array_by_name(const cl_json_t *array,
                                           const char *name);

/**
 * @name cl_json_type_to_string
 * @brief Gives a string represetantion of a json node type.
 *
 * @param [in] type: The JSON node type.
 *
 * @return On success returns a string representing the JSON node type or NULL
 *         otherwise.
 */
const char *cl_json_type_to_string(enum cl_json_type type);

#endif

