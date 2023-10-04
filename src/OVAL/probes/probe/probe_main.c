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
 *      Tomas Heinrich <theinric@redhat.com>
 */
#include <config.h>
#if !defined(_GNU_SOURCE)
# if defined(HAVE_PTHREAD_TIMEDJOIN_NP) && defined(HAVE_CLOCK_GETTIME)
#  define _GNU_SOURCE
# endif
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include "_seap.h"
#include "common/bfind.h"
#include "probe.h"
#include "ncache.h"
#include "rcache.h"
#include "icache.h"
#include "worker.h"
#include "input_handler.h"
#include "probe-api.h"
#include "option.h"
#include <oscap_debug.h>
#include "debug_priv.h"

#if defined(OS_FREEBSD)
#include <pthread_np.h>
#endif

#ifdef OS_WINDOWS
#define STDIN_FILENO _fileno(stdin)
#define STDOUT_FILENO _fileno(stdout)
#else
#include <unistd.h>
#endif

#include "probe_main.h"
#include "seap-descriptor.h"
#include "probe-table.h"

static int fail(int err, const char *who, int line)
{
	fprintf(stderr, "FAIL: %d:%s: %d, %s\n", line, who, err, strerror(err));
	exit(err);
}

void  *OSCAP_GSYM(probe_arg)          = NULL;
bool   OSCAP_GSYM(varref_handling)    = true;
char **OSCAP_GSYM(no_varref_ents)     = NULL;
size_t OSCAP_GSYM(no_varref_ents_cnt) = 0;

pthread_barrier_t OSCAP_GSYM(th_barrier);

extern probe_ncache_t *OSCAP_GSYM(ncache);

static int probe_optecmp(char **a, char **b)
{
	return strcmp(*a, *b);
}

static SEXP_t *probe_reset(SEXP_t *arg0, void *arg1)
{
        probe_t *probe = (probe_t *)arg1;
        /*
         * FIXME: implement main loop locking & worker waiting
         */
	probe_rcache_free(probe->rcache);

        probe->rcache = probe_rcache_new();
        probe_ncache_clear(OSCAP_GSYM(ncache));
        probe->ncache = OSCAP_GSYM(ncache);

        return(NULL);
}

static int probe_opthandler_varref(int option, int op, va_list args)
{
	bool  o_switch;
	char *o_name;
	char *o_temp;

	if (op == PROBE_OPTION_GET)
		return -1;

	o_switch = va_arg(args, int);
	o_name   = va_arg(args, char *);

	if (o_name == NULL) {
		/* switch varref handling on/off globally */
		OSCAP_GSYM(varref_handling) = o_switch;
		return (0);
	}

	o_temp = oscap_bfind (OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
			      sizeof(char *), o_name, (int(*)(void *, void *)) &probe_optecmp);

	if (o_temp != NULL)
		return (0);

	void *new_no_varref_ents = realloc(OSCAP_GSYM(no_varref_ents),
						   sizeof (char *) * (OSCAP_GSYM(no_varref_ents_cnt)+1));
	if (new_no_varref_ents == NULL)
		return -2;
	OSCAP_GSYM(no_varref_ents_cnt)++;
	OSCAP_GSYM(no_varref_ents) = new_no_varref_ents;
	OSCAP_GSYM(no_varref_ents)[OSCAP_GSYM(no_varref_ents_cnt) - 1] = strdup(o_name);

	qsort(OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
              sizeof (char *), (int(*)(const void *, const void *))&probe_optecmp);

	return (0);
}

static int probe_opthandler_rcache(int option, int op, va_list args)
{
	return (0);
}

static void probe_common_main_cleanup(void *arg)
{
	dD("probe_common_main_cleanup started");

	probe_t *probe = (probe_t *)arg;
	/* Cancel probe_input_handler thread */
	if (pthread_cancel(probe->th_input) != 0) {
		dE("Cannot cancel the probe input thread.");
		return;
	}

	void *status;
	int ret = pthread_join(probe->th_input, &status);
	if (ret != 0) {
		dE("pthread_join of probe_input_handler thread has failed: %d", ret);
	}
	dD("probe_input_handler thread has joined with status %ld", (long) status);

	probe_fini_function_t fini_function = probe_table_get_fini_function(probe->subtype);
	if (fini_function != NULL) {
		fini_function(probe->probe_arg);
	}

	probe_rcache_free(probe->rcache);
	probe_icache_free(probe->icache);
	rbt_i32_free(probe->workers);
	SEAP_CTX_free(probe->SEAP_ctx);
	free(probe->option);

	dD("probe_common_main_cleanup finished");
}

void *probe_common_main(void *arg)
{
	pthread_attr_t th_attr;
	probe_t        probe;
	struct probe_common_main_argument *probe_argument = (struct probe_common_main_argument *) arg;
	sch_queuedata_t *data = probe_argument->queuedata;
	oval_subtype_t subtype = probe_argument->subtype;
	probe.subtype = subtype;
	probe.real_root_fd = -1;
	probe.real_cwd_fd = -1;

#if defined(HAVE_PTHREAD_SETNAME_NP)
# if defined(OS_APPLE)
	pthread_setname_np("common_main");
# else
	pthread_setname_np(pthread_self(), "common_main");
# endif
#endif

	dD("probe_common_main started");

	const unsigned thread_count = 2; // input and icache threads
	if ((errno = pthread_barrier_init(&OSCAP_GSYM(th_barrier), NULL, thread_count)) != 0) {
		fail(errno, "pthread_barrier_init", __LINE__ - 6);
	}

	probe.offline_mode = false;
	probe.selected_offline_mode = PROBE_OFFLINE_NONE;
	probe.flags = 0;
	probe.pid   = getpid();
        probe.probe_exitcode = 0;

	/*
	 * Initialize SEAP stuff
	 */
	probe.SEAP_ctx = SEAP_CTX_new();
	probe.sd = SEAP_add_probe(probe.SEAP_ctx, data);

	if (probe.sd < 0)
		fail(errno, "SEAP_openfd2", __LINE__ - 3);

	if (SEAP_cmd_register(probe.SEAP_ctx, PROBECMD_RESET, SEAP_CMDREG_USEARG, &probe_reset, &probe) != 0)
		fail(errno, "SEAP_cmd_register", __LINE__ - 1);

	/*
	 * Initialize result & name caching
	 */
	probe.rcache = probe_rcache_new();
	probe.icache = probe_icache_new();
	probe_ncache_clear(OSCAP_GSYM(ncache));
	probe.ncache = OSCAP_GSYM(ncache);

	/*
	 * Initialize probe option handlers
	 */
#define PROBE_OPTION_INITCOUNT 2

	probe.option = malloc(sizeof(probe_option_t) * PROBE_OPTION_INITCOUNT);
	probe.optcnt = PROBE_OPTION_INITCOUNT;

	probe.option[0].option  = PROBEOPT_VARREF_HANDLING;
	probe.option[0].handler = &probe_opthandler_varref;
	probe.option[1].option  = PROBEOPT_RESULT_CACHING;
	probe.option[1].handler = &probe_opthandler_rcache;

	OSCAP_GSYM(probe_optdef) = probe.option;
	OSCAP_GSYM(probe_optdef_count) = probe.optcnt;


	/*
	 * Create input handler (detached)
	 */
        probe.workers   = rbt_i32_new();

	probe_init_function_t init_function = probe_table_get_init_function(probe.subtype);
	if (init_function != NULL) {
		probe.probe_arg = init_function();
	}

	pthread_cleanup_push(probe_common_main_cleanup, (void *) &probe);

	pthread_attr_init(&th_attr);

	if (pthread_create(&probe.th_input, &th_attr, &probe_input_handler, &probe))
		fail(errno, "pthread_create(probe_input_handler)", __LINE__ - 1);

	pthread_attr_destroy(&th_attr);

	void *status;
	if (pthread_join(probe.th_input, &status) != 0) {
		dD("pthread_join of probe_input_handler thread has failed");
	}
	dD("probe_input_handler thread has joined with status %ld", (long) status);

	pthread_cleanup_pop(1);

	return NULL;
}
