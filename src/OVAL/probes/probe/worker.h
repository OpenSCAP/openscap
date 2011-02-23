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
#ifndef WORKER_H
#define WORKER_H

#include <seap.h>
#include <sexp.h>
#include <pthread.h>
#include "probe.h"

#ifndef PROBE_WORKER_DEFAULT_MAX_THREADS
# define PROBE_WORKER_DEFAULT_MAX_THREADS 64 /**< maximum number of worker threads that will be created */
#endif

#ifndef PROBE_WORKER_DEFAULT_MAX_CHDEPTH
# define PROBE_WORKER_DEFAULT_MAX_CHDEPTH 8 /**< maximum depth of a worker thread chain */
#endif

typedef struct {
	SEAP_msgid_t sid; /**< SEAP message handled by this thread */
	pthread_t    tid; /**< thread ID */
	SEXP_t * (*msg_handler)(probe_t *, SEAP_msg_t *, int *); /**< input message (object) handler */
	SEAP_msg_t  *msg; /**< the message being handled */
} probe_worker_t;

typedef struct {
	probe_t        *probe;
	probe_worker_t *pth;
} probe_pwpair_t;

probe_worker_t *probe_worker_new(void);
void *probe_worker_runfn(void *arg);
SEXP_t *probe_worker(probe_t *probe, SEAP_msg_t *msg_in, int *ret);

#endif /* WORKER_H */
