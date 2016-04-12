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
#include <libgen.h>
#include <seap.h>
#include "common/bfind.h"
#include "probe.h"
#include "ncache.h"
#include "rcache.h"
#include "icache.h"
#include "worker.h"
#include "signal_handler.h"
#include "input_handler.h"
#include "probe-api.h"
#include "option.h"

static int fail(int err, const char *who, int line)
{
	fprintf(stderr, "FAIL: %d:%s: %d, %s\n", line, who, err, strerror(err));
	exit(err);
}

void  *OSCAP_GSYM(probe_arg)          = NULL;
bool   OSCAP_GSYM(varref_handling)    = true;
char **OSCAP_GSYM(no_varref_ents)     = NULL;
size_t OSCAP_GSYM(no_varref_ents_cnt) = 0;
probe_offline_flags OSCAP_GSYM(offline_mode) = PROBE_OFFLINE_NONE;
probe_offline_flags OSCAP_GSYM(offline_mode_supported) = PROBE_OFFLINE_NONE;
int OSCAP_GSYM(offline_mode_cobjflag) = SYSCHAR_FLAG_NOT_APPLICABLE;

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
        probe_ncache_free(probe->ncache);

        probe->rcache = probe_rcache_new();
        probe->ncache = probe_ncache_new();

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

	OSCAP_GSYM(no_varref_ents) = oscap_realloc(OSCAP_GSYM(no_varref_ents),
						   sizeof (char *) * ++OSCAP_GSYM(no_varref_ents_cnt));
	OSCAP_GSYM(no_varref_ents)[OSCAP_GSYM(no_varref_ents_cnt) - 1] = strdup(o_name);

	qsort(OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
              sizeof (char *), (int(*)(const void *, const void *))&probe_optecmp);

	return (0);
}

static int probe_opthandler_rcache(int option, int op, va_list args)
{
	return (0);
}

static int probe_opthandler_offlinemode(int option, int op, va_list args)
{
	if (op == PROBE_OPTION_SET) {
		probe_offline_flags o_offline_mode;
		int o_cobjflag = OSCAP_GSYM(offline_mode_cobjflag);

		o_offline_mode = va_arg(args, int);
		if (o_offline_mode != PROBE_OFFLINE_ALL) {
			/*
			 * If the probe doesn't support offline mode, then probe_main()
			 * won't be called in offline modee and a collected object with
			 * the following flag will be generated for all queries.
			 *
			 * We have hardcoded not_collected here as the best fit for majority
			 * of offline use cases. The test result will get the unknown result
			 * which is pretty descriptive of the state.
			 *
			 * Other option would be to return not applicable. That would, however,
			 * make the test result not_applicable as well. Which in turn may hide
			 * underlying problems.
			 */
			o_cobjflag = SYSCHAR_FLAG_NOT_COLLECTED;
		}
		OSCAP_GSYM(offline_mode_supported) = o_offline_mode;
		OSCAP_GSYM(offline_mode_cobjflag) = o_cobjflag;
	} else if (op == PROBE_OPTION_GET) {
		int *offline_mode_supported = va_arg(args, int *);
		int *offline_mode = va_arg(args, int *);

		if (offline_mode_supported != NULL) {
		  *offline_mode_supported = OSCAP_GSYM(offline_mode_supported);
		}
		if (offline_mode != NULL) {
		  *offline_mode = OSCAP_GSYM(offline_mode);
		}
	}
	return 0;
}

// Dummy pthread routine
static void * dummy_routine(void *dummy_param)
{
	return NULL;
}

static void preload_libraries_before_chroot()
{
	// Force to load dynamic libraries used by pthread_cancel
	pthread_t t;
	if (pthread_create(&t, NULL, dummy_routine, NULL))
		fail(errno, "pthread_create(probe_preload)", __LINE__ - 1);
	pthread_cancel(t);
	pthread_join(t, NULL);
}

int main(int argc, char *argv[])
{
	pthread_attr_t th_attr;
	sigset_t       sigmask;
	probe_t        probe;
	char *rootdir = NULL;

	if ((errno = pthread_barrier_init(&OSCAP_GSYM(th_barrier), NULL,
	                                  1 + // signal thread
	                                  1 + // input thread
	                                  1 + // icache thread
	                                  0)) != 0)
	{
		fail(errno, "pthread_barrier_init", __LINE__ - 6);
	}

	/*
	 * Block signals, any signals received will be
	 * handled by the signal handler thread.
	 */
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGHUP);
	sigaddset(&sigmask, SIGUSR1);
	sigaddset(&sigmask, SIGUSR2);
	sigaddset(&sigmask, SIGINT);
	sigaddset(&sigmask, SIGTERM);
	sigaddset(&sigmask, SIGQUIT);
        sigaddset(&sigmask, SIGPIPE);

	if (pthread_sigmask(SIG_BLOCK, &sigmask, NULL))
		fail(errno, "pthread_sigmask", __LINE__ - 1);

	probe.flags = 0;
	probe.pid   = getpid();
	probe.name  = basename(argv[0]);
        probe.probe_exitcode = 0;

	/*
	 * Initialize SEAP stuff
	 */
	probe.SEAP_ctx = SEAP_CTX_new();
	probe.sd       = SEAP_openfd2(probe.SEAP_ctx, STDIN_FILENO, STDOUT_FILENO, 0);

	if (probe.sd < 0)
		fail(errno, "SEAP_openfd2", __LINE__ - 3);

	if (SEAP_cmd_register(probe.SEAP_ctx, PROBECMD_RESET, 0, &probe_reset) != 0)
		fail(errno, "SEAP_cmd_register", __LINE__ - 1);

	/*
	 * Initialize result & name caching
	 */
	probe.rcache = probe_rcache_new();
	probe.ncache = probe_ncache_new();
        probe.icache = probe_icache_new();

        OSCAP_GSYM(ncache) = probe.ncache;

	/*
	 * Initialize probe option handlers
	 */
#define PROBE_OPTION_INITCOUNT 3

	probe.option = oscap_alloc(sizeof(probe_option_t) * PROBE_OPTION_INITCOUNT);
	probe.optcnt = PROBE_OPTION_INITCOUNT;

	probe.option[0].option  = PROBEOPT_VARREF_HANDLING;
	probe.option[0].handler = &probe_opthandler_varref;
	probe.option[1].option  = PROBEOPT_RESULT_CACHING;
	probe.option[1].handler = &probe_opthandler_rcache;
	probe.option[2].option  = PROBEOPT_OFFLINE_MODE_SUPPORTED;
	probe.option[2].handler = &probe_opthandler_offlinemode;

	OSCAP_GSYM(probe_optdef) = probe.option;
	OSCAP_GSYM(probe_optdef_count) = probe.optcnt;

	/*
	 * Create signal handler
	 */
	pthread_attr_init(&th_attr);

	if (pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_JOINABLE))
		fail(errno, "pthread_attr_setdetachstate", __LINE__ - 1);

	if (pthread_create(&probe.th_signal, &th_attr, &probe_signal_handler, &probe))
		fail(errno, "pthread_create(probe_signal_handler)", __LINE__ - 1);

	pthread_attr_destroy(&th_attr);

	/*
	 * Setup offline mode(s)
	 */
	if ((rootdir = getenv("OSCAP_PROBE_ROOT")) != NULL) {
		if(strlen(rootdir) > 0) {
			if (chdir(rootdir) != 0) {
				fail(errno, "chdir", __LINE__ -1);
			}

			preload_libraries_before_chroot();
			if (chroot(rootdir) != 0) {
				fail(errno, "chroot", __LINE__ - 1);
			}
			/* NOTE: We're running in a different root directory.
			 * Unless /proc, /sys are somehow emulated for the new
			 * environment, they are not relevant and so are other
			 * runtime only things (e.g. getenv, uname, ...).
			 * Switch to offline mode. We may add a separate
			 * mechanism to control this behaviour in the future.
			 */
			OSCAP_GSYM(offline_mode) |= PROBE_OFFLINE_CHROOT;
		}
	}
	if (getenv("OSCAP_PROBE_RPMDB_PATH") != NULL) {
		OSCAP_GSYM(offline_mode) |= PROBE_OFFLINE_RPMDB;
	}

	/*
	 * Create input handler (detached)
	 */
        probe.workers   = rbt_i32_new();
        probe.probe_arg = probe_init();

	pthread_attr_init(&th_attr);

	if (pthread_create(&probe.th_input, &th_attr, &probe_input_handler, &probe))
		fail(errno, "pthread_create(probe_input_handler)", __LINE__ - 1);

	pthread_attr_destroy(&th_attr);

	/*
	 * Wait until the signal handler exits
	 */
	if (pthread_join(probe.th_signal, NULL))
		fail(errno, "pthread_join", __LINE__ - 1);

	/*
	 * Wait for the input_handler thread
	 */
#if defined(HAVE_PTHREAD_TIMEDJOIN_NP) && defined(HAVE_CLOCK_GETTIME)
	{
		struct timespec j_tm;

		if (clock_gettime(CLOCK_REALTIME, &j_tm) == -1)
			fail(errno, "clock_gettime", __LINE__ - 1);

		j_tm.tv_sec += 3;

		if (pthread_timedjoin_np(probe.th_input, NULL, &j_tm) != 0)
			fail(errno, "pthread_timedjoin_np", __LINE__ - 1);
	}
#else
	if (pthread_join(probe.th_input, NULL))
		fail(errno, "pthread_join", __LINE__ - 1);
#endif
	/*
	 * Cleanup
	 */
        probe_fini(probe.probe_arg);

	probe_ncache_free(probe.ncache);
	probe_rcache_free(probe.rcache);
        probe_icache_free(probe.icache);

        rbt_i32_free(probe.workers);

        if (probe.sd != -1)
                SEAP_close(probe.SEAP_ctx, probe.sd);

	SEAP_CTX_free(probe.SEAP_ctx);
        oscap_free(probe.option);

	return (probe.probe_exitcode);
}
