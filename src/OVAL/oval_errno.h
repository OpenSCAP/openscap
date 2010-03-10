
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

/*
 * oval_errno.h
 *
 *  Created on: Sep 8, 2009
 *      Author: tom.greaves
 */

#ifndef OVAL_ERRNO_H_
#define OVAL_ERRNO_H_

#include "../common/util.h"

OSCAP_HIDDEN_START;

extern int oval_errno;
#define OVAL_INVALID_ARGUMENT     1
#define OVAL_INTERNAL_ERROR       2
#define OVAL_UNSUPPORTED_DATATYPE 3
#define OVAL_INVALID_COMPARISON   4
#define OVAL_NO_RESULT_CALCULATED 5

OSCAP_HIDDEN_END;

#endif				/* OVAL_ERRNO_H_ */
