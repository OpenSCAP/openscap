#ifndef PROBE_H
#define PROBE_H

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <pthread.h>
#include <seap.h>
#include "ncache.h"
#include "rcache.h"
#include "probe-common.h"

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

typedef struct {
	int option;
	int (*handler)(int, va_list);
} probe_option_t;

typedef struct {
	pthread_rwlock_t rwlock;
	uint32_t         flags;

	char       *name;
	pid_t       pid;

        void       *probe_arg;

	SEAP_CTX_t *SEAP_ctx; /**< SEAP context */
	int         sd;       /**< SEAP descriptor */

	pthread_t th_input;
	pthread_t th_signal;

        rbt_t    *workers;
        uint32_t  max_threads;
        uint32_t  max_chdepth;

	probe_rcache_t *rcache; /**< probe result cache */
	probe_ncache_t *ncache; /**< probe name cache */

	probe_option_t *option; /**< probe option handlers */
	size_t          optcnt; /**< number of defined options */
} probe_t;

#endif /* PROBE_H */
