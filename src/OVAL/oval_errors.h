
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
 *	"Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef OVAL_ERRORS_H
#define OVAL_ERRORS_H

#define OVAL_EINVAL            1
#define OVAL_EUNKNOWN        255

#define OVAL_EPROBEINIT      256
#define OVAL_EPROBECONN      257
#define OVAL_EPROBENOTSUPP   258
#define OVAL_EPROBEOBJINVAL  259
#define OVAL_EPROBEITEMINVAL 260
#define OVAL_EPROBENODATA    261
#define OVAL_EPROBECLOSE     262
#define OVAL_EPROBESEND      263
#define OVAL_EPROBERECV      264
#define OVAL_EPROBEUNKNOWN   511

#endif /* OVAL_ERRORS_H */
