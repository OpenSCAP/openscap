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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#if defined(__linux__)
# include <sys/prctl.h>
#endif
#include <errno.h>
#include <seap.h>
#include "probe.h"
#include "worker.h"
#include "common/debug_priv.h"
#include "signal_handler.h"
#include "common/compat_pthread_barrier.h"

typedef struct {
	probe_worker_t **thr;
	size_t           cnt;
} __thr_collection;

static int __abort_cb(void *n, void *u)
{
	__thr_collection    *coll = (__thr_collection *)u;
	struct rbt_i32_node *node = (struct rbt_i32_node *)n;
	probe_worker_t      *thr  = (probe_worker_t *)(node->data);

        pthread_cancel(thr->tid);

	coll->thr = oscap_realloc(coll->thr, sizeof(SEAP_msg_t *) * ++coll->cnt);
	coll->thr[coll->cnt - 1] = thr;

	return (0);
}

void *probe_signal_handler(void *arg)
{
        probe_t  *probe = (probe_t *)arg;
	siginfo_t siinf;
	sigset_t  siset;

#if defined(HAVE_PTHREAD_SETNAME_NP)
# if defined(__APPLE__)
	pthread_setname_np("signal_handler");
# else
	pthread_setname_np(pthread_self(), "signal_handler");
# endif
#endif

	sigemptyset(&siset);
	sigaddset(&siset, SIGHUP);
	sigaddset(&siset, SIGUSR1);
	sigaddset(&siset, SIGUSR2);
	sigaddset(&siset, SIGINT);
	sigaddset(&siset, SIGTERM);
	sigaddset(&siset, SIGQUIT);
        sigaddset(&siset, SIGPIPE);

#if defined(__linux__)
        if (prctl(PR_SET_PDEATHSIG, SIGTERM) != 0)
                dW("prctl(PR_SET_PDEATHSIG, SIGTERM) failed");
#endif
       
	dD("Signal handler ready");
	switch (errno = pthread_barrier_wait(&OSCAP_GSYM(th_barrier)))
	{
	case 0:
	case PTHREAD_BARRIER_SERIAL_THREAD:
		break;
	default:
		dE("pthread_barrier_wait: %d, %s.", errno, strerror(errno));
		return (NULL);
	}

	while (sigwaitinfo(&siset, &siinf) != -1) {

		dD("Received signal %d from %u (%s)",
		   siinf.si_signo, (unsigned int)siinf.si_pid,
		   getppid() == siinf.si_pid ? "parent" : "not my parent");

#if defined(PROBE_SIGNAL_PARENTONLY)
		/* Listen only to signals sent from the parent process */
		if (getppid() != siinf.si_pid)
			continue;
#endif

		switch(siinf.si_signo) {
		case SIGUSR1:/* probe abort */
                        probe->probe_exitcode = ECONNABORTED;
			/* FALLTHROUGH */
                case SIGINT:
                case SIGTERM:
                case SIGQUIT:
                case SIGPIPE:
		{
			__thr_collection coll;

			coll.thr = NULL;
			coll.cnt = 0;

                        pthread_cancel(probe->th_input);

			/* collect IDs and cancel threads */
			rbt_walk_inorder2(probe->workers, __abort_cb, &coll, 0);

			/*
			 * Wait till all threads are canceled (they may temporarily disable
			 * cancelability), but at most 60 seconds per thread.
			 */
			for (; coll.cnt > 0; --coll.cnt) {
				probe_worker_t *thr = coll.thr[coll.cnt - 1];
#if defined(HAVE_PTHREAD_TIMEDJOIN_NP) && defined(HAVE_CLOCK_GETTIME)
				struct timespec j_tm;

				if (clock_gettime(CLOCK_REALTIME, &j_tm) == -1) {
					dE("clock_gettime(CLOCK_REALTIME): %d, %s.", errno, strerror(errno));
					continue;
				}

				j_tm.tv_sec += 60;

				if ((errno = pthread_timedjoin_np(thr->tid, NULL, &j_tm)) != 0) {
					dE("[%llu] pthread_timedjoin_np: %d, %s.", (uint64_t)thr->sid, errno, strerror(errno));
					/*
					 * Memory will be leaked here by continuing to the next thread. However, we are in the
					 * process of shutting down the whole probe. We're just nice and gave the probe_main()
					 * thread a chance to finish it's critical section which shouldn't take that long...
					 */
					continue;
				}
#else
				if ((errno = pthread_join(thr->tid, NULL)) != 0) {
					dE("pthread_join: %d, %s.", errno, strerror(errno));
					continue;
				}
#endif
				SEAP_msg_free(coll.thr[coll.cnt - 1]->msg);
                                oscap_free(coll.thr[coll.cnt - 1]);
			}

			oscap_free(coll.thr);
			goto exitloop;
		}
                case SIGUSR2:
                case SIGHUP:
                        /* ignore */
                        break;
                }
	}
exitloop:
	return (NULL);
}
