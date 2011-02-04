#ifndef PROBE_THREAD
#define PROBE_THREAD

#ifndef PROBE_THREAD_DEFAULT_MAX_THREADS
#define PROBE_THREAD_DEFAULT_MAX_THREADS 64 /**< maximum number of thread that will be created */
#endif

#ifndef PROBE_THREAD_DEFAULT_MAX_CHDEPTH
#define PROBE_THREAD_DEFAULT_MAX_CHDEPTH 8 /**< maximum depth of a thread chain */
#endif

#include <pthread.h>
#include <stdbool.h>
#include "seap.h"
#include "SEAP/generic/rbt/rbt.h"

typedef struct {
	SEAP_msgid_t sid; /**< SEAP message handled by this thread */
	pthread_t    tid; /**< thread ID */

	SEXP_t * (*msg_handler)(SEAP_msg_t *, int *); /**< input message (object) handler */
	SEAP_msg_t        *msg; /**< the message being handled */
} probe_thread_t;

typedef struct {
	uint32_t  flags;
	rbt_t    *threads;
	uint32_t  max_threads; /**< maximum threads in total */
	uint32_t  max_chdepth; /**< maximum depth of the dependency chain */
	pthread_t sigthr;
} probe_threadmgr_t;

typedef struct {
	probe_threadmgr_t *mgr;
	probe_thread_t    *pth;
} probe_pmpair_t;

#define PROBE_THREADMGR_FREE 0x00000001


probe_threadmgr_t *probe_threadmgr_new(uint32_t max_threads, uint32_t max_chdepth, probe_threadmgr_t *dst);
void probe_threadmgr_free(probe_threadmgr_t *mgr);
probe_thread_t *probe_thread_new(void);
int probe_thread_create(probe_threadmgr_t *mgr, SEAP_msgid_t sid, SEXP_t * (*msg_handler)(SEAP_msg_t *, int *), SEAP_msg_t *msg);



#endif /* PROBE_THREAD */
