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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Daniel Kopecek <dkopecek@redhat.com>
 */
#ifndef PROBE_H
#define PROBE_H

#include "oscap_platforms.h"

#include <sys/types.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <pthread.h>
#include "_seap.h"
#include "ncache.h"
#include "rcache.h"
#include "icache.h"
#include "probe-common.h"
#include "option.h"
#include "common/util.h"
#include "common/compat_pthread_barrier.h"

/* default max. memory usage ratio - used/total */
/* can be overridden by environment variable OSCAP_PROBE_MEMORY_USAGE_RATIO */
#define OSCAP_PROBE_MEMORY_USAGE_RATIO_DEFAULT 0.33

/* By default, probes can collect unlimited amount of items. Ths behavior can
 * be overridden by environment variable OSCAP_PROBE_MAX_COLLECTED_ITEMS.
 */
#define OSCAP_PROBE_COLLECT_UNLIMITED 0

typedef struct {
	pthread_rwlock_t rwlock;
	uint32_t         flags;

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
	bool offline_mode;
	int supported_offline_mode;
	int selected_offline_mode;
	oval_subtype_t subtype;

	int real_root_fd;
	int real_cwd_fd;
} probe_t;

struct probe_ctx {
        SEXP_t         *probe_in;  /**< S-exp representation of the input object */
        SEXP_t         *probe_out; /**< collected object */
        SEXP_t         *filters;   /**< object filters (OVAL 5.8 and higher) */
        probe_icache_t *icache;    /**< item cache */
	int offline_mode;
	double max_mem_ratio;
	size_t collected_items;
	size_t max_collected_items;
	struct oscap_list *blocked_paths;
};

typedef enum {
	PROBE_OFFLINE_NONE = 0x00,
	PROBE_OFFLINE_CHROOT = 0x01,
	PROBE_OFFLINE_OWN = 0x04,
	PROBE_OFFLINE_ALL = 0x0f
} probe_offline_flags;

extern pthread_barrier_t OSCAP_GSYM(th_barrier);

#endif /* PROBE_H */
