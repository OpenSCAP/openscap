/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */
#ifndef PROBE_H
#define PROBE_H

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <pthread.h>
#include <seap.h>
#include "ncache.h"
#include "rcache.h"
#include "icache.h"
#include "probe-common.h"
#include "option.h"
#include "common/util.h"
#include "common/compat_pthread_barrier.h"

typedef struct {
	pthread_rwlock_t rwlock;
	uint32_t         flags;

	char       *name;
	pid_t       pid;

        void       *probe_arg;
        int         probe_exitcode;

	SEAP_CTX_t *SEAP_ctx; /**< SEAP context */
	int         sd;       /**< SEAP descriptor */

	pthread_t th_input;
	pthread_t th_signal;

        rbt_t    *workers;
        uint32_t  max_threads;
        uint32_t  max_chdepth;

	probe_rcache_t *rcache; /**< probe result cache */
	probe_ncache_t *ncache; /**< probe name cache */
        probe_icache_t *icache; /**< probe item cache */

	probe_option_t *option; /**< probe option handlers */
	size_t          optcnt; /**< number of defined options */
} probe_t;

struct probe_ctx {
        SEXP_t         *probe_in;  /**< S-exp representation of the input object */
        SEXP_t         *probe_out; /**< collected object */
        SEXP_t         *filters;   /**< object filters (OVAL 5.8 and higher) */
        probe_icache_t *icache;    /**< item cache */
};

typedef enum {
	PROBE_OFFLINE_NONE = 0x00,
	PROBE_OFFLINE_CHROOT = 0x01,
	PROBE_OFFLINE_RPMDB = 0x02,
	PROBE_OFFLINE_OWN = 0x04,
	PROBE_OFFLINE_ALL = 0x0f
} probe_offline_flags;

extern pthread_barrier_t th_barrier;
extern probe_offline_flags offline_mode;
extern probe_offline_flags offline_mode_supported;
extern int offline_mode_cobjflag;

#endif /* PROBE_H */
