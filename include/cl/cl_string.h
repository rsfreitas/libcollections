
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:55:20 2015
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

#ifndef _COLLECTIONS_CL_STRING_H
#define _COLLECTIONS_CL_STRING_H	1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_string.h> directly; include <collections.h> instead."
# endif
#endif

/**
 * @name cl_string_create
 * @brief Creates a new cl_string_t object.
 *
 * Creates a new cl_string_t object containing value passed as argument.
 * If argument \a fmt is NULL then the object will be created empty.
 *
 * @param [in] fmt: The format of the newly created string.
 * @param [in] ...: Variable arguments will values of the respective format.
 *
 * @return On success the cl_string_t object will be returned. Returns NULL
 *         otherwise.
 */
cl_string_t *cl_string_create(const char *fmt, ...)
                              __attribute__((format(printf, 1, 2)));

/**
 * @name cl_string_create_empty
 * @brief Creates a new empty cl_string_t object.
 *
 * @param [in] size: The internal size of the string.
 *
 * @return On success the cl_string_t object will be returned. Returns NULL
 *         otherwise.
 */
cl_string_t *cl_string_create_empty(unsigned int size);

/**
 * @name cl_string_create_random
 * @brief Creates a cl_string_t object containing random letters.
 *
 * @param [in] size: Size of the newly created string.
 *
 * @return On success the cl_string_t object will be returned. Returns NULL
 *         otherwise.
 */
cl_string_t *cl_string_create_random(unsigned int size);

/**
 * @name cl_string_destroy
 * @brief Frees a cl_string_t object from memory.
 *
 * @param [in,out] string The cl_string_t object that will be released.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_string_destroy(cl_string_t *string);

/**
 * @name cl_string_length
 * @brief Gets the length of a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return Returns the cl_string_t object length on success or -1 otherwise.
 */
int cl_string_length(const cl_string_t *string);

/**
 * @name cl_string_valueof
 * @brief Gets the value of a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return Returns the cl_string_t object value as a 'const char *' on success
 *         or NULL otherwise.
 */
const char *cl_string_valueof(const cl_string_t *string);

/**
 * @name cl_string_dup
 * @brief Duplicates a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object which will be duplicated.
 *
 * @return On success returns the new cl_string_t object or NULL otherwise.
 */
cl_string_t *cl_string_dup(const cl_string_t *string);

/**
 * @name cl_string_at
 * @brief Returns the char value at the specified index.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] index: The index of the char value.
 *
 * @return Returns the char value at the specified index of a cl_string_t object.
 */
char cl_string_at(const cl_string_t *string, unsigned int index);

/**
 * @name cl_string_set
 * @brief Changes the value of a specified index to a new value.
 *
 * @param [in,out] string: The cl_string_t object.
 * @param [in] c: The new value.
 * @param [in] index: The index of the value to be replaced.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_string_set(cl_string_t *string, char c, unsigned int index);

/**
 * @name cl_string_cat
 * @brief Concatenate a C string with a cl_string_t object.
 *
 * @param [in,out] string: The cl_string_t object.
 * @param [in] fmt: The format of the string to be concatenated.
 * @param [in] ...: The values of the format string.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_string_cat(cl_string_t *string, const char *fmt, ...)
                  __attribute__((format(printf, 2, 3)));

/**
 * @name cl_string_upper
 * @brief Converts all letters from a cl_string_t object to uppercase.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_upper(cl_string_t *string);

/**
 * @name cl_string_lower
 * @brief Converts all letters from a cl_string_t object to lowercase.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_lower(cl_string_t *string);

/**
 * @name cl_string_capitalize
 * @brief Convert the first letter of a cl_string_t object to uppercase.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_capitalize(cl_string_t *string);

/**
 * @name cl_string_find
 * @brief Searches for a specific char inside a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the char index inside the cl_string_t object or
 *         -1 otherwise.
 */
int cl_string_find(const cl_string_t *string, char c);

/**
 * @name cl_string_rfind
 * @brief Searches for a specific char inside a cl_string_t object from the end.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the char index inside the cl_string_t object or
 *         -1 otherwise.
 */
int cl_string_rfind(const cl_string_t *string, char c);

/**
 * @name cl_string_cchr
 * @brief Searches and count the number of occurrences of a char inside a
 *        cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] c: The searched char.
 *
 * @return On success returns the number of occurrences of the char inside the
 *         cl_string_t object or -1 otherwise.
 */
int cl_string_cchr(const cl_string_t *string, char c);

/**
 * @name cl_string_ltrim
 * @brief Removes all white spaces from the beginning of a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_ltrim(cl_string_t *string);

/**
 * @name cl_string_rtrim
 * @brief Removes all white spaces from the end of a cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_rtrim(cl_string_t *string);

/**
 * @name cl_string_alltrim
 * @brief Removes all white spaces from the beginning and from the end of a
 *        cl_string_t object.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_alltrim(cl_string_t *string);

/**
 * @name cl_string_cmp
 * @brief Compare two cl_string_t objects.
 *
 * @param [in] s1: The first string.
 * @param [in] s2: The second string.
 *
 * @return Returns less than, equal to, or greater than zero if \a s1 is found,
 *         respectively, to be less than, to match, or be greater than \a s2.
 */
int cl_string_cmp(const cl_string_t *s1, const cl_string_t *s2);

/**
 * @name cl_string_ncmp
 * @brief Compare two cl_string_t objects.
 *
 * @param [in] s1: The fist string.
 * @param [in] s2: The second string.
 * @param [in] n: The number of bytes to compare.
 *
 * @return Returns less than, equal to, or greater than zero if the first \a n
 *         bytes of \a s1 is found, respectively, to be less than, to match, or
 *         be greater than \a s2.
 */
int cl_string_ncmp(const cl_string_t *s1, const cl_string_t *s2, size_t n);

/**
 * @name cl_string_rplchr
 * @brief Replaces all occurrences of char \a c1 by \a c2 in a cl_string_t
 *        object.
 *
 * @param [in,out] string: The cl_string_t object.
 * @param [in] c1: The char which will be replaced.
 * @param [in] c2: The new char.
 *
 * @return On success returns the number of replaced occurrences or -1 otherwise.
 */
int cl_string_rplchr(cl_string_t *string, char c1, char c2);

/**
 * @name cl_string_rplsubstr
 * @brief Replaces all occurrences of a specific substring inside a cl_string_t
 *        object.
 *
 * @param [in,out] string: The cl_string_t object.
 * @param [in] old: The old substring.
 * @param [in] new_: The new substring.
 *
 * @return On success returns the number of replaced occurrences or -1 otherwise.
 */
int cl_string_rplsubstr(cl_string_t *string, const char *old, const char *new_);

/**
 * @name cl_string_isempty
 * @brief Checks if a cl_string_t object has any valid data.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return Returns true if cl_string_t object has valid data or false
 *         otherwise.
 */
bool cl_string_isempty(const cl_string_t *string);

/**
 * @name cl_string_clear
 * @brief Clears the content of a cl_string_t object.
 *
 * @param [in,out] string: The cl_string_t object.
 *
 * @return Returns 0 on success or -1 otherwise.
 */
int cl_string_clear(cl_string_t *string);

/**
 * @name cl_string_substr
 * @brief Searches for a specific substring inside a cl_string_t object and
 *        extract it.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] needle: The substring.
 *
 * @return On success returns the new cl_string_t object or NULL otherwise.
 */
cl_string_t *cl_string_substr(const cl_string_t *string, const char *needle);

/**
 * @name cl_string_to_int
 * @brief Converts the value of a cl_string_t object to int.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns the int value of a cl_string_t object or -1
 *         otherwise.
 */
int cl_string_to_int(const cl_string_t *string);

/**
 * @name cl_string_to_long
 * @brief Converts the value of a cl_string_t object to long.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns the long value of a cl_string_t object or
 *         -1 otherwise.
 */
long cl_string_to_long(const cl_string_t *string);

/**
 * @name cl_string_to_long_long
 * @brief Converts the value of a cl_string_t object to long long.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns the long long value of a cl_string_t object or
 *         -1 otherwise.
 */
long long cl_string_to_long_long(const cl_string_t *string);

/**
 * @name cl_string_to_float
 * @brief Converts the value of a cl_string_t object to float.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns the float value of a cl_string_t object or -1
 *         otherwise.
 */
float cl_string_to_float(const cl_string_t *string);

/**
 * @name cl_string_to_double
 * @brief Converts the value of a cl_string_t object to double.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return On success returns the double value of a cl_string_t object ot -1
 *         otherwise.
 */
double cl_string_to_double(const cl_string_t *string);

/**
 * @name cl_string_split
 * @brief Splits the cl_string_t object around matches of the given tokens.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] delim: The list of tokens.
 *
 * @return Returns a cl_stringlist_t object containing all substrings splitted
 *         on success or NULL otherwise
 */
cl_stringlist_t *cl_string_split(const cl_string_t *string, const char *delim);

/**
 * @name cl_string_is_number
 * @brief Checks if a cl_string_t content represents a number.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return If the content of the object is a number returns true otherwise
 *         returns false.
 */
bool cl_string_is_number(const cl_string_t *string);

/**
 * @name cl_string_is_float_number
 * @brief Checks if a cl_string_t content represents a float number.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return If the content of the object is a float number returns true
 *         otherwise returns false.
 */
bool cl_string_is_float_number(const cl_string_t *string);

/**
 * @name cl_string_is_alphanumeric
 * @brief Checks if a cl_string_t content represents an alphanumeric text.
 *
 * @param [in] string: The cl_string_t object.
 *
 * @return If the content of the object has an alphanumeric content returns
 *         true otherwise returns false.
 */
bool cl_string_is_alphanumeric(const cl_string_t *string);

/**
 * @name cl_string_ref
 * @brief Increases the reference count for a cl_string_t item.
 *
 * @param [in,out] string: The cl_string_t item.
 *
 * @return On success returns the item itself with its reference count
 *         increased or NULL otherwise.
 */
cl_string_t *cl_string_ref(cl_string_t *string);

/**
 * @name cl_string_unref
 * @brief Decreases the reference count for a cl_string_t item.
 *
 * When its reference count drops to 0, the item is finalized (its memory is
 * freed).
 *
 * @param [in,out] string: The cl_string_t item.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_unref(cl_string_t *string);

/**
 * @name cl_string_dchr
 * @brief Deletes a character from a cl_string_t.
 *
 * This function removes the first entry of the respective character.
 *
 * @param [in] string: The cl_string_t.
 * @param [in] c: The character which will be removed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_dchr(cl_string_t *string, char c);

/**
 * @name cl_string_idchr
 * @brief Deletes a character from a cl_string_t from a specific position.
 *
 * @param [in] string: Ths cl_string_t.
 * @param [in] p: The character position which will be removed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_idchr(cl_string_t *string, unsigned int p);

/**
 * @name cl_string_cpy
 * @brief Copies a cl_string_t object content to another cl_string_t object.
 *
 * @param [out] dest: The cl_string_t destination object.
 * @param [in] src: The cl_string_t source object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_cpy(cl_string_t *dest, const cl_string_t *src);

/**
 * @name cl_string_contains
 * @brief Searches for a specific substring inside a cl_string_t object and
 *        extract it.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] needle: The substring.
 *
 * @return Returns true or false if the substring was found or not inside the
 *         original string.
 */
bool cl_string_contains(const cl_string_t *string, const char *needle);

/**
 * @name cl_string_count_matches
 * @brief Count the number of occurrences of a substring.
 *
 * @param [in] string: The cl_string_t object.
 * @param [in] needle: The substring.
 *
 * @return Returns the number of occurrences of the substring in the string
 *         or -1 otherwise.
 */
int cl_string_count_matches(const cl_string_t *string, const char *needle);

/**
 * @name cl_string_set_content
 * @brief Sets a string content to a previously created buffer.
 *
 * This function receives a previously created buffer and take its ownership.
 * After its call, the user must not free this buffer.
 *
 * @param [in] s: The cl_string_t object.
 * @param [in] content: The previously created string.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_set_content(cl_string_t *s, const char *content);

/**
 * @name cl_string_update_length
 * @brief Updates the string internal length information with its current
 *        content.
 *
 * @param [in] s: The cl_string_t object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_string_update_length(cl_string_t *s);

#endif

