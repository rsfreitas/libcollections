
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

#ifndef _COLLECTIONS_API_FILE_H
#define _COLLECTIONS_API_FILE_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <file.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _STDIO_H
# include <stdio.h>
#endif

/**
 * @name cl_fload
 * @brief Loads a file to memory.
 *
 * This function will try to open file \a filename and load it to a memory
 * buffer.
 *
 * @param [in] filename: The file name.
 * @param [out] bsize: The size of the loaded file.
 *
 * @return On success returns the new allocated buffer containing the file
 *         or NULL otherwise.
 */
unsigned char *cl_fload(const char *filename, unsigned int *bsize);

/**
 * @name cl_funload
 * @brief Unloads a buffer from memory.
 *
 * @param [in,out] buffer: The buffer which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_funload(unsigned char *buffer);

/**
 * @name cl_fsave
 * @brief Write a file.
 *
 * @param [in] filename: The file name which will be created (or overwritten).
 * @param [in] buffer: The new content of the file.
 * @param [in] bsize: The size of \a buffer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cl_fsave(const char *filename, const unsigned char *buffer,
             unsigned int bsize);

/**
 * @name cl_freadline
 * @brief Reads a line from a file until a 'new line' is found or EOF.
 *
 * @param [in] infile: A FILE pointer to previously opened file.
 *
 * @return On success returns a pointer to an allocated buffer containing the
 *         line read (which should be freed after used) or NULL otherwise.
 */
char *cl_freadline(FILE *infile);

/**
 * @name cl_file_mime_type
 * @brief Gets the mime type of a file.
 *
 * @param [in] pathname: The file name.
 *
 * @return On success returns a C string with the file mime type or NULL
 *         otherwise.
 */
char *cl_file_mime_type(const char *pathname);

/**
 * @name cl_buffer_mime_type
 * @brief Gets the mime type of a file from its buffer.
 *
 * @param [in] buffer: The file buffer.
 * @param [in] size: The file buffer size.
 *
 * @return On success returns a C string with the file mime type or NULL
 *         otherwise.
 */
char *cl_buffer_mime_type(const unsigned char *buffer, unsigned int size);

#endif

