
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
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

#ifndef _COLLECTIONS_CL_STRING_H
#define _COLLECTIONS_CL_STRING_H	1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_string.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cstring_new
 * @brief Creates a new cstring_t object.
 *
 * Creates a new cstring_t object containing value passed as argument.
 * If argument \a fmt is NULL then the object will be created empty.
 *
 * @param [in] fmt: The format of the newly created string.
 * @param [in] ...: Variable arguments will values of the respective format.
 *
 * @return On success the cstring_t object will be returned. Returns NULL
 *         otherwise.
 */
cstring_t *cstring_new(const char *fmt, ...);

/**
 * @name cstring_new_ex
 * @brief Creates a new cstring_t object.
 *
 * @param [in] size:
 *
 * @return On success the cstring_t object will be returned. Returns NULL
 *         otherwise.
 */
cstring_t *cstring_new_ex(unsigned int size);

/**
 * @name cstring_random
 * @brief Creates a cstring_t object containing random letters.
 *
 * @param [in] size: Size of the newly created string.
 *
 * @return On success the cstring_t object will be returned. Returns NULL
 *         otherwise.
 */
cstring_t *cstring_random(unsigned int size);

/**
 * @name cstring_destroy
 * @brief Frees a cstring_t object from memory.
 *
 * @param [in,out] string The cstring_t object that will be released.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cstring_destroy(cstring_t *string);

/**
 * @name cstring_length
 * @brief Gets the length of a cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return Returns the cstring_t object length on success or -1 otherwise.
 */
int cstring_length(const cstring_t *string);

/**
 * @name cstring_valueof
 * @brief Gets the value of a cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return Returns the cstring_t object value as a 'const char *' on success
 *         or NULL otherwise.
 */
const char *cstring_valueof(const cstring_t *string);

/**
 * @name cstring_dup
 * @brief Duplicates a cstring_t object.
 *
 * @param [in] string: The cstring_t object which will be duplicated.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_dup(const cstring_t *string);

/**
 * @name cstring_at
 * @brief Returns the char value at the specified index.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] index: The index of the char value.
 *
 * @return Returns the char value at the specified index of a cstring_t object.
 */
char cstring_at(const cstring_t *string, unsigned int index);

/**
 * @name cstring_set
 * @brief Changes the value of a specified index to a new value.
 *
 * @param [in,out] string: The cstring_t object.
 * @param [in] c: The new value.
 * @param [in] index: The index of the value to be replaced.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cstring_set(cstring_t *string, char c, unsigned int index);

/**
 * @name cstring_cat
 * @brief Concatenate two strings.
 *
 * @param [in,out] string: The cstring_t object.
 * @param [in] fmt: The format of the string to be concatenated.
 * @param [in] ...: The values of the format string.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cstring_cat(cstring_t *string, const char *fmt, ...);

/**
 * @name cstring_upper
 * @brief Converts all letters from a cstring_t object to uppercase.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_upper(const cstring_t *string);

/**
 * @name cstring_lower
 * @brief Converts all letters from a cstring_t object to lowercase.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_lower(const cstring_t *string);

/**
 * @name cstring_capitalize
 * @brief Convert the first letter of a cstring_t object to uppercase.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_capitalize(const cstring_t *string);

/**
 * @name cstring_find
 * @brief Searches for a specific char inside a cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the char index inside the cstring_t object or
 *         -1 otherwise.
 */
int cstring_find(const cstring_t *string, char c);

/**
 * @name cstring_rfind
 * @brief Searches for a specific char inside a cstring_t object from the end.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the char index inside the cstring_t object or
 *         -1 otherwise.
 */
int cstring_rfind(const cstring_t *string, char c);

/**
 * @name cstring_cchr
 * @brief Searches and count the number of occurrences of a char inside a
 *        cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the number of occurrences of the char inside the
 *         cstring_t object or -1 otherwise.
 */
int cstring_cchr(const cstring_t *string, char c);

/**
 * @name cstring_ltrim
 * @brief Removes all white spaces from the beginning of a cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_ltrim(const cstring_t *string);

/**
 * @name cstring_rtrim
 * @brief Removes all white spaces from the end of a cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_rtrim(const cstring_t *string);

/**
 * @name cstring_alltrim
 * @brief Removes all white spaces from the beginning and from the end of a
 *        cstring_t object.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_alltrim(const cstring_t *string);

/**
 * @name cstring_cmp
 * @brief Compare two cstring_t objects.
 *
 * @param [in] s1: The first string.
 * @param [in] s2: The second string.
 *
 * @return Returns less than, equal to, or greater than zero if \a s1 is found,
 *         respectively, to be less than, to match, or be greater than \a s2.
 */
int cstring_cmp(const cstring_t *s1, const cstring_t *s2);

/**
 * @name cstring_ncmp
 * @brief Compare two cstring_t objects.
 *
 * @param [in] s1: The fist string.
 * @param [in] s2: The second string.
 * @param [in] n: The number of bytes to compare.
 *
 * @return Returns less than, equal to, or greater than zero if the first \a n
 *         bytes of \a s1 is found, respectively, to be less than, to match, or
 *         be greater than \a s2.
 */
int cstring_ncmp(const cstring_t *s1, const cstring_t *s2, size_t n);

/**
 * @name cstring_rplchr
 * @brief Replaces all occurrences of char \a c1 by \a c2 in a cstring_t
 *        object.
 *
 * @param [in,out] string: The cstring_t object.
 * @param [in] c1: The char which will be replaced.
 * @param [in] c2: The new char.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cstring_rplchr(cstring_t *string, char c1, char c2);

/**
 * @name cstring_rplsubstr
 * @brief Replaces all occurrences of a specific substring inside a cstring_t
 *        object.
 *
 * @param [in,out] string: The cstring_t object.
 * @param [in] old: The old substring.
 * @param [in] new_: The new substring.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cstring_rplsubstr(cstring_t *string, const char *old, const char *new_);

/**
 * @name cstring_isempty
 * @brief Checks if a cstring_t object has any valid data.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return Returns CL_TRUE if cstring_t object has valid data or CL_FALSE
 *         otherwise.
 */
cbool_t cstring_isempty(const cstring_t *string);

/**
 * @name cstring_clear
 * @brief Clears the content of a cstring_t object.
 *
 * @param [in,out] string: The cstring_t object.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cstring_clear(cstring_t *string);

/**
 * @name cstring_substr
 * @brief Searches for a specific substring inside a cstring_t object and
 *        extract it.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] needle: The substring.
 *
 * @return On success returns the new cstring_t object or NULL otherwise.
 */
cstring_t *cstring_substr(const cstring_t *string, const char *needle);

/**
 * @name cstring_value_as_int
 * @brief Converts the value of a cstring_t object to int.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the int value of a cstring_t object or -1
 *         otherwise.
 */
int cstring_value_as_int(const cstring_t *string);

/**
 * @name cstring_value_as_long
 * @brief Converts the value of a cstring_t object to long.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the long value of a cstring_t object or
 *         -1 otherwise.
 */
long cstring_value_as_long(const cstring_t *string);

/**
 * @name cstring_value_as_long_long
 * @brief Converts the value of a cstring_t object to long long.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the long long value of a cstring_t object or
 *         -1 otherwise.
 */
long long cstring_value_as_long_long(const cstring_t *string);

/**
 * @name cstring_value_as_float
 * @brief Converts the value of a cstring_t object to float.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the float value of a cstring_t object or -1
 *         otherwise.
 */
float cstring_value_as_float(const cstring_t *string);

/**
 * @name cstring_value_as_double
 * @brief Converts the value of a cstring_t object to double.
 *
 * @param [in] string: The cstring_t object.
 *
 * @return On success returns the double value of a cstring_t object ot -1
 *         otherwise.
 */
double cstring_value_as_double(const cstring_t *string);

/**
 * @name cstring_split
 * @brief Splits the cstring_t object around matches of the given tokens.
 *
 * @param [in] string: The cstring_t object.
 * @param [in] delim: The list of tokens.
 *
 * @return Returns a cstring_list_t object containing all substrings splitted
 *         on success or NULL otherwise
 */
cstring_list_t *cstring_split(const cstring_t *string, const char *delim);

/**
 * @name cstring_is_number
 * @brief Checks if a cstring_t content represents a number.
 *
 * @param [in] value: The cstring_t object.
 *
 * @return If the content of the object is a number returns CL_TRUE otherwise
 *         returns CL_FALSE.
 */
cbool_t cstring_is_number(const cstring_t *value);

/**
 * @name cstring_is_float_number
 * @brief Checks if a cstring_t content represents a float number.
 *
 * @param [in] value: The cstring_t object.
 *
 * @return If the content of the object is a float number returns CL_TRUE
 *         otherwise returns CL_FALSE.
 */
cbool_t cstring_is_float_number(const cstring_t *value);

/**
 * @name cstring_is_alphanumeric
 * @brief Checks if a cstring_t content represents an alphanumeric text.
 *
 * @param [in] value: The cstring_t object.
 *
 * @return If the content of the object has an alphanumeric content returns
 *         CL_TRUE otherwise returns CL_FALSE.
 */
cbool_t cstring_is_alphanumeric(const cstring_t *value);

/**
 * @name cstring_ref
 * @brief Increases the reference count for a cstring_t item.
 *
 * @param [in,out] string: The cstring_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cstring_t *cstring_ref(cstring_t *string);

/**
 * @name cstring_unref
 * @brief Decreases the reference count for a cstring_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] string: The cstring_t item.
 *
 * @return On sucess returns 0 or -1 otherwise.
 */
int cstring_unref(cstring_t *string);

#endif

