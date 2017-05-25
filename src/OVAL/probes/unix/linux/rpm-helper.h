/*
 * Copyright 2016 Red Hat Inc., Durham, North Carolina.
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
#ifndef __RPM_HELPER__
#define __RPM_HELPER__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rpm/rpmdb.h>
#include <rpm/rpmfi.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmmacro.h>
#include <rpm/rpmlog.h>
#include <rpm/header.h>

#include <pthread.h>
#include "common/util.h"
#include "common/debug_priv.h"
#include "pthread.h"

struct rpm_probe_global {
	rpmts rpmts;
	pthread_mutex_t mutex;
};

#ifndef HAVE_HEADERFORMAT
# define HAVE_LIBRPM44 1 /* hack */
//# define headerFormat(_h, _fmt, _emsg) headerSprintf((_h),( _fmt), rpmTagTable, rpmHeaderFormats, (_emsg))
#endif

#ifndef HAVE_RPMFREECRYPTO
# define rpmFreeCrypto() while(0)
#endif

#ifndef HAVE_RPMFREEFILESYSTEMS
# define rpmFreeFilesystems() while(0)
#endif

#define RPM_MUTEX_LOCK(mutex_ptr) \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_lock(mutex_ptr) != 0) { \
			dE("Can't lock mutex"); \
			return (-1); \
		} \
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &prev_cancel_state); \
	} while(0)

#define RPM_MUTEX_UNLOCK(mutex_ptr) \
	do { \
		int prev_cancel_state = -1; \
		if (pthread_mutex_unlock(mutex_ptr) != 0) { \
			dE("Can't unlock mutex. Aborting..."); \
			abort(); \
		} \
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &prev_cancel_state); \
	} while(0)

#ifdef HAVE_RPM46
int rpmErrorCb (rpmlogRec rec, rpmlogCallbackData data);
#endif

// todo: HAVE_RPM412 needs to be set by configure,
// although fallback solution should have same result
#ifdef HAVE_RPM412
#define DISABLE_PLUGINS(ts) rpmtsSetFlags(ts, RPMTRANS_FLAG_NOPLUGINS)
#else
#define DISABLE_PLUGINS(ts) rpmDefineMacro(NULL,"__plugindir \"\"", 0);
#endif

/**
 * Preload libraries required by rpm
 * It destroy error callback!
 */
void rpmLibsPreload(void);

#endif
