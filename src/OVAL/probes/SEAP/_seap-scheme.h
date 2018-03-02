/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#pragma once
#ifndef _SEAP_SCHEME_H
#define _SEAP_SCHEME_H

#include <stddef.h>
#include <stdint.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include "_sexp-types.h"
#include "_seap-types.h"
#include "seap-descriptor.h"
#include "generic/common.h"
#include "../../../common/util.h"

#define SEAP_IO_EVREAD  0x01

#define SEAP_RECVBUF_SIZE 4*4096
#define SEAP_SENDBUF_SIZE 4*4096

/* queue */
#include "sch_queue.h"
#define SCH_QUEUE    4

#endif /* _SEAP_SCHEME_H */
