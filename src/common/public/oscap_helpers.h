/*
 * Copyright 2018 Red Hat Inc., Durham, North Carolina.
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
 * Authors:
 *      Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_HELPERS_H
#define OSCAP_HELPERS_H

#include <stdarg.h>
#include "oscap_export.h"

/**
 * Print to a newly allocated string using variable arguments.
 * @param fmt printf-like format string
 * @return a string
 */
OSCAP_API char *oscap_sprintf(const char *fmt, ...);

/**
 * Return the canonicalized absolute pathname.
 * @param path path
 * @param resolved_path pointer to a buffer
 * @return resolved_path or NULL in case of error
 */
OSCAP_API char *oscap_realpath(const char *path, char *resolved_path);

/**
 * Return filename component of a path
 * @param path path
 * The function can modify the contents of path, so the caller should pass a copy of path.
 * @return filename component of path
 * The caller is responsible to free the returned buffer.
 */
OSCAP_API char *oscap_basename(char *path);

/**
 * Return directory component of a path
 * @param path path
 * The function can modify the contents of path, so the caller should pass a copy of path.
 * @return dirname component of path
 * The caller is responsible to free the returned buffer.
 */
OSCAP_API char *oscap_dirname(char *path);

/**
 * Extract tokens from strings
 * @param str string
 * @param delim st of delimiters
 * @param saveptr Used to store position information between calls to strtok_s
 * @return token
 */
OSCAP_API char *oscap_strtok_r(char *str, const char *delim, char **saveptr);

#endif /* OSCAP_HELPERS_H */
