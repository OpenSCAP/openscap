// SPDX-License-Identifier: LGPL-2.1-or-later

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#if defined(OS_FREEBSD)
#include <pthread.h>
#endif
#include <stdio.h>

#include "memusage.h"

#if defined(OS_FREEBSD)
#define STRESS_THREADS 4
#define STRESS_ITERATIONS 2000

struct thread_result {
	int err;
};

static void *worker(void *arg)
{
	struct thread_result *result = arg;
	int i;

	for (i = 0; i < STRESS_ITERATIONS; ++i) {
		struct proc_memusage proc_mu = {0};

		if (oscap_proc_memusage(&proc_mu) != 0) {
			result->err = errno ? errno : -1;
			return NULL;
		}
	}

	result->err = 0;
	return NULL;
}
#endif

int main(void)
{
#if defined(OS_FREEBSD)
	pthread_t threads[STRESS_THREADS];
	struct thread_result results[STRESS_THREADS];
	int i;

	for (i = 0; i < STRESS_THREADS; ++i) {
		results[i].err = 0;
		if (pthread_create(&threads[i], NULL, worker, &results[i]) != 0) {
			perror("pthread_create");
			return 1;
		}
	}

	for (i = 0; i < STRESS_THREADS; ++i) {
		if (pthread_join(threads[i], NULL) != 0) {
			perror("pthread_join");
			return 1;
		}
		if (results[i].err != 0) {
			fprintf(stderr, "oscap_proc_memusage failed in worker %d with errno=%d\n", i, results[i].err);
			return 1;
		}
	}
#endif

	return 0;
}
