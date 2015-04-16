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
 *      Jan Černý <jcerny@redhat.com>
 */

#ifndef _OVAL_GLOB_TO_REGEX
#define _OVAL_GLOB_TO_REGEX

#include "common/util.h"

OSCAP_HIDDEN_START;

/**
 * Converts unix shell glob to Perl 5 regular expression
 * @param glob input glob
 * @param noescape Tells if backslash is treated as an escape character.
 * @return regular expression
 */
char *oval_glob_to_regex(const char *glob, int noescape);

OSCAP_HIDDEN_END;

#endif
