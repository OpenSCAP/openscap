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

typedef struct {
	int option;
	int (*handler)(int, va_list);
} probe_option_t;

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

extern pthread_barrier_t OSCAP_GSYM(th_barrier);

#endif /* PROBE_H */
