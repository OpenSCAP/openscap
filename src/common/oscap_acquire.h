/*
 * Copyright 2012--2014 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef OSCAP_ACQUIRE_H_
#define OSCAP_ACQUIRE_H_

#include "util.h"
#include <stdbool.h>

OSCAP_HIDDEN_START;

/**
 * Create an oscap temp dir. (While ideally all the operations are being
 * made on unliked files using file descriptors, this is bordeline impossible
 * in short term given the library interfaces.
 * This function emits a message on stderr in case of error.
 * @return filename of the temporary directory or NULL on error. Returned value
 * shall be freed by oscap_acquire_cleanup_dir() call.
 */
char *oscap_acquire_temp_dir(void);

/**
 * Unlink given directory recursivelly.
 * @param dir_path path to the directory for removal.
 */
void oscap_acquire_cleanup_dir(char **dir_path);

/**
 * Create new temp file within given directory. With filename
 * corresponding with the template.
 * @param dir directory to create file in
 * @param template scheme for newly created file name
 * @param filename name of newly created temp_file. The caller
 * is responsible for disposing it.
 * @returns file descriptors or a negative number to indicate failure.
 */
int oscap_acquire_temp_file(const char *dir, const char *template, char **filename);

/**
 * Download the given url to a random file in the given directory.
 * @param temp_dir Directory to store the result in.
 * @param url The url to acquire.
 * @return the filename of the newly created file or NULL on error.
 */
char *oscap_acquire_url_download(const char *temp_dir, const char *url);

/**
 * Is the given url supported by OpenSCAP?
 * @param url Requested url
 * @return true if the given string reminds supported url.
 */
bool oscap_acquire_url_is_supported(const char *url);

/**
 * Escape characters in the url so it can be used as POSIX filename.
 * @param url Requested url
 * @return escaped url or NULL
 */
char *oscap_acquire_url_to_filename(const char *url);

/**
 * Guess how the realpath of given file may look like. Do your best!
 * Unlike realpath() this works for non-existent files.
 * @param filepath
 * @returns the normalized filepath
 */
char *oscap_acquire_guess_realpath(const char *filepath);

/**
 * Creates the directory, if it does not already exist. Further it makes parent
 * directories as needed.
 * @param path filepath to the target directory
 * @returns the zero on success
 */
int oscap_acquire_mkdir_p(const char* path);

/**
 * Ensures the parent directory of the filepath, if it does not already exists.
 * Further it makes parent directories as needed.
 * @param filepath filepath to the descendant file or dir
 * @returns the zero on success
 */
int oscap_acquire_ensure_parent_dir(const char *filepath);

// FIXME: SCE engine uses this particular function
OSCAP_HIDDEN_END;

/**
 * Read characters from pipe, escape '&' and store result
 * in the C string buffer.
 * @param fd File descriptor to read. It will be closed afterwards
 * @returns buffer allocated on heap.
 */
char *oscap_acquire_pipe_to_string(int fd);

OSCAP_HIDDEN_START;

OSCAP_HIDDEN_END;

#endif
