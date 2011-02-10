#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "common/debug_priv.h"
#include "common/public/alloc.h"
#include "seap.h"
#include "probe-thread.h"
#include "probe-cache.h"
#include "public/probe-api.h"

extern volatile int OSCAP_GSYM(sigexit);
extern SEAP_CTX_t  *OSCAP_GSYM(ctx);
extern int          OSCAP_GSYM(sd);
extern pcache_t    *OSCAP_GSYM(pcache);

static void *probe_signal_thread(void *arg)
{
	/* probe_threadmgr_t *mgr = (probe_threadmgr_t *)arg; */

	siginfo_t siinf;
	sigset_t  siset;

	sigemptyset(&siset);
	sigaddset(&siset, SIGUSR1);
        sigaddset(&siset, SIGINT);
        sigaddset(&siset, SIGTERM);
	sigaddset(&siset, SIGQUIT);

	dI("signal handler ready\n");

	while (sigwaitinfo(&siset, &siinf) != -1) {
		dI("received signal %d from %u (%s)\n",
		   siinf.si_signo, (unsigned int)siinf.si_pid,
		   getppid() == siinf.si_pid ? "parent" : "not my parent");

#if defined(PROBE_SIGNAL_PARENTONLY)
		/* Listen only to signals sent from the parent process */
		if (getppid() != siinf.si_pid)
			continue;
#endif

		switch(siinf.si_signo) {
		case SIGUSR1: /* probe abort */
			/* TODO: clean exit */
			exit(ECONNABORTED);
			break;
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			/* TODO: clean exit */
			exit(0);
			break;
		}
	}

	return (NULL);
}

probe_threadmgr_t *probe_threadmgr_new(uint32_t max_threads, uint32_t max_chdepth, probe_threadmgr_t *dst)
{
	sigset_t sigmask, sigmask_old;
	bool restore_sigmask = false;

	if (dst == NULL) {
		dst = oscap_talloc(probe_threadmgr_t);
		dst->flags = PROBE_THREADMGR_FREE;
	} else
		dst->flags = 0;

	dst->threads     = rbt_i32_new();
	dst->max_threads = max_threads == 0 ? PROBE_THREAD_DEFAULT_MAX_THREADS : max_threads;
	dst->max_chdepth = max_chdepth == 0 ? PROBE_THREAD_DEFAULT_MAX_CHDEPTH : max_chdepth;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
        sigaddset(&sigmask, SIGINT);
        sigaddset(&sigmask, SIGTERM);
	sigaddset(&sigmask, SIGQUIT);

	if (pthread_sigmask(SIG_BLOCK, &sigmask, &sigmask_old)) {
		dE("pthread_sigmask(SIG_BLOCK, ...) failed\n");
		goto __fail;
	}

	restore_sigmask = true;

	if (pthread_create(&dst->sigthr, NULL, &probe_signal_thread, dst)) {
		dE("pthread_create(&probe_signal_thread) failed\n");
		goto __fail;
	}

	return (dst);
__fail:
	rbt_i32_free(dst->threads);
	oscap_free(dst);

	if (restore_sigmask)
		pthread_sigmask(SIG_SETMASK, &sigmask_old, NULL);

	return (NULL);
}

void probe_threadmgr_free(probe_threadmgr_t *mgr)
{
	if (mgr != NULL) {
		/*
		 * TODO: delete the threads tree
		 */

		if (mgr->flags & PROBE_THREADMGR_FREE)
			oscap_free(mgr);
	}
}

static void *probe_thread_runfn(void *arg)
{
	probe_pmpair_t *pair = (probe_pmpair_t *)arg;

	SEXP_t *probe_res, *obj, *oid;
	int     probe_ret;

	dI("handling SEAP message ID %u\n", pair->pth->sid);
	//
	probe_ret = -1;
	probe_res = pair->pth->msg_handler(pair->pth->msg, &probe_ret);
	//
	dI("handler result = %p, return code = %d\n", probe_res, probe_ret);

	/* Assuming that the red-black tree API is doing locking for us... */
	if (rbt_i32_del(pair->mgr->threads, pair->pth->sid, NULL) != 0) {
		dW("thread not found in the probe thread tree, probably canceled by an external signal\n");
		/*
		 * XXX: this is a possible deadlock; we can't send anything from
		 * here because the signal handler replied to the message
		 */

		/* TODO: free the result & exit */
		arg = NULL;
	} else {
		dI("probe thread deleted\n");

		obj = SEAP_msg_get(pair->pth->msg);
		oid = probe_obj_getattrval(obj, "id");

		if (pcache_sexp_add(OSCAP_GSYM(pcache), oid, probe_res) != 0) {
			/* TODO */
			abort();
		}

		SEXP_vfree(obj, oid, NULL);
	}

	if (probe_ret != 0) {
		/*
		 * Something bad happened. A hint of the cause is stored as a error code in
		 * probe_ret (should be). We'll send it to the library using a SEAP error packet.
		 */
		if (SEAP_replyerr(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), pair->pth->msg, probe_ret) == -1) {
			int ret = errno;

			dE("An error ocured while sending error status. errno=%u, %s.\n", errno, strerror(errno));
			SEXP_free(probe_res);

			/* FIXME */
			exit(ret);
		}
	} else {
		SEAP_msg_t *seap_reply;
		/*
		 * OK, the probe actually returned something, let's send it to the library.
		 */
		seap_reply = SEAP_msg_new();
		SEAP_msg_set(seap_reply, probe_res);

		if (SEAP_reply(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), seap_reply, pair->pth->msg) == -1) {
			int ret = errno;

			SEAP_msg_free(seap_reply);
			SEXP_free(probe_res);

			exit(ret);
		}

		SEAP_msg_free(seap_reply);
                SEXP_free(probe_res);
	}

	oscap_free(pair);

	return (NULL);
}

probe_thread_t *probe_thread_new(void)
{
	probe_thread_t *pth = oscap_talloc(probe_thread_t);

	pth->sid = 0;
	pth->tid = 0;
	pth->msg_handler = NULL;
	pth->msg = NULL;

	return (pth);
}

/**
 * Create a new thread for handling a message with ID `sid'. This thread will be registered by
 * the thread manager. The handler is defined by the function `function' and `arg' is the user
 * pointer which will be passed to this function. The `arg_free' function is called only if the
 * thread is canceled before returing from the handler function.
 */
int probe_thread_create(probe_threadmgr_t *mgr, SEAP_msgid_t sid, SEXP_t * (*msg_handler)(SEAP_msg_t *, int *), SEAP_msg_t *msg)
{
	pthread_attr_t   pth_attr;
	probe_pmpair_t  *pair;

	if (mgr == NULL || msg_handler == NULL)
		return (-1);

	if (pthread_attr_init(&pth_attr) != 0)
		return (-1);

	if (pthread_attr_setdetachstate(&pth_attr, PTHREAD_CREATE_DETACHED) != 0) {
		pthread_attr_destroy(&pth_attr);
		return (-1);
	}

	pair = oscap_talloc(probe_pmpair_t);

	pair->mgr      = mgr;
	pair->pth      = probe_thread_new();
	pair->pth->sid = sid;
	pair->pth->msg = msg;
	pair->pth->msg_handler = msg_handler;

	if (rbt_i32_add(mgr->threads, sid, pair->pth, NULL) != 0) {
		/*
		 * Getting here means that there is already a
		 * thread handling the message with the given
		 * ID.
		 */
		pthread_attr_destroy(&pth_attr);
		oscap_free(pair->pth);
		oscap_free(pair);

		return (-1);
	}

	if (pthread_create(&pair->pth->tid, &pth_attr, &probe_thread_runfn, pair) != 0) {
		pthread_attr_destroy(&pth_attr);

		if (rbt_i32_del(pair->mgr->threads, pair->pth->sid, NULL) != 0) {
			/* ... do something ... */
		}

		oscap_free(pair);
		return (-1);
	}

	pthread_attr_destroy(&pth_attr);

	return (0);
}
