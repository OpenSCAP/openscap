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
#include "worker.h"
#include "signal_handler.h"
#include "input_handler.h"
#include "probe-api.h"

static int fail(int err, const char *who, int line)
{
	fprintf(stderr, "FAIL: %d:%s: %d, %s\n", line, who, err, strerror(err));
	exit(err);
}

void  *OSCAP_GSYM(probe_arg)          = NULL;
bool   OSCAP_GSYM(varref_handling)    = true;
char **OSCAP_GSYM(no_varref_ents)     = NULL;
size_t OSCAP_GSYM(no_varref_ents_cnt) = 0;
struct id_desc_t OSCAP_GSYM(id_desc);

extern probe_ncache_t *OSCAP_GSYM(encache);

#if 0
static int probe_optekcmp(const char *a, char **b)
{
	return strcmp(a, *b);
}

static int probe_optekcmp_sexp(const SEXP_t *a, char **b)
{
	return SEXP_strcmp(a, *b);
}
#endif /* 0 */

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

	probe_item_resetidctr(&(OSCAP_GSYM(id_desc)));

        return(NULL);
}

static int probe_opthandler_varref(int option, va_list args)
{
	bool  o_switch;
	char *o_name;
	char *o_temp;

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

static int probe_opthandler_rcache(int option, va_list args)
{
	return (0);
}

int main(int argc, char *argv[])
{
	pthread_attr_t th_attr;
	sigset_t       sigmask;
	probe_t        probe;

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

	if (SEAP_cmd_register(probe.SEAP_ctx, PROBE_CMD_RESET, 0, &probe_reset) != 0)
		fail(errno, "SEAP_cmd_register", __LINE__ - 1);

	/*
	 * Initialize result & name caching
	 */
	probe.rcache = probe_rcache_new();
	probe.ncache = probe_ncache_new();
        OSCAP_GSYM(encache) = probe.ncache;

	/*
	 * Initialize probe option handlers
	 */
#define PROBE_OPTION_INITCOUNT 2

	probe.option = oscap_alloc(sizeof(probe_option_t) * PROBE_OPTION_INITCOUNT);
	probe.optcnt = PROBE_OPTION_INITCOUNT;

        probe.option[0].option  = PROBE_VARREF_HANDLING;
        probe.option[0].handler = &probe_opthandler_varref;
        probe.option[1].option  = PROBE_RESULT_CACHING;
        probe.option[1].handler = &probe_opthandler_rcache;

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
	 * Create input handler (detached)
	 */
        probe.workers   = rbt_i32_new();
        probe.probe_arg = probe_init();

	pthread_attr_init(&th_attr);

	if (pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED))
		fail(errno, "pthread_attr_setdetachstate", __LINE__ - 1);

	if (pthread_create(&probe.th_input, &th_attr, &probe_input_handler, &probe))
		fail(errno, "pthread_create(probe_input_handler)", __LINE__ - 1);

	pthread_attr_destroy(&th_attr);

	/*
	 * Wait until the signal handler exits
	 */
	if (pthread_join(probe.th_signal, NULL))
		fail(errno, "pthread_join", __LINE__ - 1);

	/*
	 * Cleanup
	 */
        probe_fini(probe.probe_arg);

	probe_ncache_free(probe.ncache);
	probe_rcache_free(probe.rcache);

        rbt_i32_free(probe.workers);

        if (probe.sd != -1)
                SEAP_close(probe.SEAP_ctx, probe.sd);

	SEAP_CTX_free(probe.SEAP_ctx);
        oscap_free(probe.option);

	return (probe.probe_exitcode);
}
