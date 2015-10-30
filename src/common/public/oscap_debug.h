/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 *       Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_DEBUG_H_
#define OSCAP_DEBUG_H_

typedef enum {
	DBG_E = 1,
	DBG_W,
	DBG_I,
	DBG_D,
	DBG_UNKNOWN = -1
} oscap_verbosity_levels;

/**
 * Turn on debugging information
 * @param verbosity_level Verbosity level
 * @param filename Name of file used as output file for store debugging
 *                 and other additional information.
 * @param is_probe Determines whether the function is called
 *                 from a probe (true) or from the base library (false).
 * @return When the verbose mode has been turned on it returns true, otherwise false.
 */
bool oscap_set_verbose(const char *verbosity_level, const char *filename, bool is_probe);

/**
 * Parse verbosity level from a string.
 * @param level_name Verbosity level as a C string.
 * @return Verbosity level number (or -1 on error).
 */
int oscap_verbosity_level_from_cstr(const char *level_name);

#endif
