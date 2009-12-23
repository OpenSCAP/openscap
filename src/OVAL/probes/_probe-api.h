#pragma once
#ifndef _PROBE_API_H
#define _PROBE_API_H

#include "public/probe-api.h"
#include "probe-cache.h"

/**
 * @struct id_desc_t
 * Holds information for item ids generation.
 */
struct id_desc_t {
#ifndef HAVE_ATOMIC_FUNCTIONS
	pthread_mutex_t item_id_ctr_lock;	///< id counter lock
#endif
	int item_id_ctr;	///< id counter
};

/// Probe's global runtime information
typedef struct {
	/* Protocol stuff */
	SEAP_CTX_t *ctx;	///< protocol context for communication with the library
	int sd;			///< file descriptor used for communication

	/* Object cache */
	pcache_t *pcache;	///< probed objects cache
	pthread_rwlock_t pcache_lock;	///< cache lock

	struct id_desc_t id_desc;	///< id generation information

	/* probe main */
	void *probe_arg;	///< optional argument to probe_main()
} globals_t;

/// Probe's global runtime information
extern globals_t global;

#if defined(HAVE_ATOMIC_FUNCTIONS)
#define GLOBALS_INITIALIZER { NULL, -1, NULL, PTHREAD_RWLOCK_INITIALIZER, {1}, NULL }
#else
#define GLOBALS_INITIALIZER { NULL, -1, NULL, PTHREAD_RWLOCK_INITIALIZER, {PTHREAD_MUTEX_INITIALIZER, 1}, NULL }
#endif

#define READER_LOCK_CACHE pthread_rwlock_rdlock (&globals.pcache_lock)
#define WRITER_LOCK_CACHE pthread_rwlock_wrlock (&globals.pcache_lock)
#define READER_UNLOCK_CACHE pthread_rwlock_unlock (&globals.pcache_lock)
#define WRITER_UNLOCK_CACHE pthread_rwlock_unlock (&globals.pcache_lock)

#define SEAP_LOCK pthread_mutex_lock (&globals.seap_lock)
#define SEAP_UNLOCK pthread_mutex_unlock (&globals.seap_lock)

#endif				/* _PROBE_API_H */
