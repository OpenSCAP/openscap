/**
 * @file _probe-api.h
 * @brief probe API private header file
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 */

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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef _PROBE_API_H
#define _PROBE_API_H

#include <stdarg.h>
#include "public/probe-api.h"
#include "probe/ncache.h"
#include "probe/rcache.h"
#include "common/util.h"

/**
 * @struct id_desc_t
 * Holds information for item ids generation.
 */
struct id_desc_t {
#ifndef HAVE_ATOMIC_FUNCTIONS
	pthread_mutex_t item_id_ctr_lock;	///< id counter lock
#endif
	int item_id_ctr;	///< id counter
};

#define SEAP_LOCK pthread_mutex_lock (&globals.seap_lock)
#define SEAP_UNLOCK pthread_mutex_unlock (&globals.seap_lock)

#endif				/* _PROBE_API_H */
