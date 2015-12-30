
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

#ifndef _COLLECTIONS_CL_FILE_H
#define _COLLECTIONS_CL_FILE_H			1

#ifndef LIBCOLLECTIONS_COMPILE
# ifndef _COLLECTIONS_H
#  error "Never use <cl_file.h> directly; include <collections.h> instead."
# endif
#endif

#ifndef _STDIO_H
# include <stdio.h>
#endif

/**
 * @name cfload
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
unsigned char *cfload(const char *filename, unsigned int *bsize);

/**
 * @name cfunload
 * @brief Unloads a buffer from memory.
 *
 * @param [in,out] buffer: The buffer which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cfunload(unsigned char *buffer);

/**
 * @name cfsave
 * @brief Write a file.
 *
 * @param [in] filename: The file name which will be created (or overwritten).
 * @param [in] buffer: The new content of the file.
 * @param [in] bsize: The size of \a buffer.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int cfsave(const char *filename, const unsigned char *buffer, unsigned int bsize);

/**
 * @name cfreadline
 * @brief Reads a line from a file until a 'new line' is found or EOF.
 *
 * @param [in] infile: A FILE pointer to previously opened file.
 *
 * @return On success returns a pointer to an allocated buffer containing the
 *         line read (which should be freed after used) or NULL otherwise.
 */
char *cfreadline(FILE *infile);

#endif

