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

#pragma once
#ifndef _PROBE_API_H
#define _PROBE_API_H

#include "public/probe-api.h"
#include "probe-cache.h"
#include "encache.h"

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

#include <sys/cdefs.h>
#define OSCAP_GSYM(s) __CONCAT(___G_, s)

extern SEAP_CTX_t *OSCAP_GSYM(ctx);
extern int         OSCAP_GSYM(sd);
extern pcache_t   *OSCAP_GSYM(pcache);
extern void       *OSCAP_GSYM(probe_arg);
extern encache_t  *OSCAP_GSYM(encache);
extern struct id_desc_t OSCAP_GSYM(id_desc);

#define SEAP_LOCK pthread_mutex_lock (&globals.seap_lock)
#define SEAP_UNLOCK pthread_mutex_unlock (&globals.seap_lock)

#endif				/* _PROBE_API_H */
