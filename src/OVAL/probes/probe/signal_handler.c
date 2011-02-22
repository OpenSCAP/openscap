#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <seap.h>
#include "probe.h"
#include "worker.h"
#include "common/debug_priv.h"
#include "signal_handler.h"

typedef struct {
	probe_worker_t **thr;
	size_t           cnt;
} __thr_collection;

static int __abort_cb(void *n, void *u)
{
	__thr_collection    *coll = (__thr_collection *)u;
	struct rbt_i32_node *node = (struct rbt_i32_node *)n;
	probe_worker_t      *thr  = (probe_worker_t *)(node->data);

	coll->thr = oscap_realloc(coll->thr, sizeof(SEAP_msg_t *) * ++coll->cnt);
	coll->thr[coll->cnt - 1] = thr;

	return (0);
}

void *probe_signal_handler(void *arg)
{
        probe_t  *probe = (probe_t *)arg;
	siginfo_t siinf;
	sigset_t  siset;

	sigemptyset(&siset);
	sigaddset(&siset, SIGUSR1);
        sigaddset(&siset, SIGINT);
        sigaddset(&siset, SIGTERM);
	sigaddset(&siset, SIGQUIT);

	dI("Signal handler ready\n");

	while (sigwaitinfo(&siset, &siinf) != -1) {

		dI("Received signal %d from %u (%s)\n",
		   siinf.si_signo, (unsigned int)siinf.si_pid,
		   getppid() == siinf.si_pid ? "parent" : "not my parent");

#if defined(PROBE_SIGNAL_PARENTONLY)
		/* Listen only to signals sent from the parent process */
		if (getppid() != siinf.si_pid)
			continue;
#endif

		switch(siinf.si_signo) {
		case SIGUSR1:/* probe abort */
		{
			__thr_collection coll;

			coll.thr = NULL;
			coll.cnt = 0;

			/* collect IDs and cancel threads */
			rbt_walk_inorder2(probe->workers, __abort_cb, &coll, 0);

			/* reply to all messages with an error */
			for (; coll.cnt > 0; --coll.cnt) {
				SEAP_replyerr(probe->SEAP_ctx, probe->sd, coll.thr[coll.cnt - 1]->msg, PROBE_ECONNABORTED);
				SEAP_msg_free(coll.thr[coll.cnt - 1]->msg);
			}

			oscap_free(coll.thr);

			/* TODO: clean exit */
			exit(ECONNABORTED);
			break;
		}
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			/* TODO: clean exit */
			exit(0);
			break;
                case SIGUSR2:
                case SIGHUP:
                        /* ignore */
                        break;
                }
	}

	return (NULL);
}
