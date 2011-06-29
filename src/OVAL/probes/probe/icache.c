#include <pthread.h>
#include <stddef.h>
#include <sexp.h>

#include "common/public/alloc.h"
#include "common/assume.h"
#include "../SEAP/generic/rbt/rbt.h"

#include "icache.h"

void* probe_icache_worker(void *arg)
{
        probe_icache_t *cache = (probe_icache_t *)(arg);
	probe_iqpair_t *pair;
	SEXP_ID_t       item_ID;

        assume_d(cache != NULL, NULL);

	if (pthread_mutex_lock(&cache->queue_mutex) != 0) {
		dE("An error ocured while locking the queue mutex: %u, %s\n",
		   errno, strerror(errno));
		return (NULL);
	}

        while(pthread_cond_wait(&cache->queue_notempty, &cache->queue_mutex) == 0) {
		assume_d(cache->queue_cnt > 0, NULL);

		/*
		 * Extract an item from the queue and update queue beg, end & cnt
		 */
		pair = cache->queue[cache->queue_end];

		--cache->queue_cnt;

		if (cache->queue_end != cache->queue_beg) {
			if (cache->queue_end == 0)
				cache->queue_end = cache->queue_max - 1;
			else
				--cache->queue_end;
		}

		/*
		 * Release the mutex
		 */
		if (pthread_mutex_unlock(&cache->queue_mutex) != 0) {
			dE("An error ocured while unlocking the queue mutex: %u, %s\n",
			   errno, strerror(errno));
			abort();
		}

		if (pair->cobj == NULL) {
			/*
			 * Handle NOP case (synchronization)
			 */
			assume_d(pair->p.cond != NULL, NULL);

			if (pthread_cond_signal(pair->p.cond) != 0) {
				dE("An error ocured while signaling NOP condition: %u, %s\n",
				   errno, strerror(errno));
				abort();
			}
		} else {
			probe_citem_t *cached = NULL;

			/*
			 * Compute item ID
			 */
			item_ID = SEXP_ID_v(pair->p.item);

			/*
			 * Perform cache lookup
			 */
			if (rbt_i64_get(queue->tree, (int64_t)item_ID, &cached) == 0) {
				register uint16_t i;
				/*
				 * Maybe a cache HIT
				 */
				for (i = 0; i < cached->count; ++i) {
					if (SEXP_deepcmp(pair->p.item, cached->item[i]))
						break;
				}

				if (i == cached->count) {
					/*
					 * Cache MISS
					 */
					cached->item = oscap_realloc(sizeof(SEXP_t *) * ++cached->count);
					cached->item[cached->count - 1] = pair->p.item;
				} else {
					/*
					 * Cache HIT
					 */
					SEXP_free(pair->p.item);
					pair->p.item = cached->item[i];
				}
			} else {
				/*
				 * Cache MISS
				 */
				cached = oscap_talloc(probe_citem_t);
				cached->item = oscap_talloc(sizeof(SEXP_t *));
				cached->item[0] = pair->p.item;
				cached->count = 1;

				if (rbt_i64_add(queue->tree, (int64_t)item_ID, (void *)cached) != 0) {
					dE("Can't add item (k=%"PRIi64" to the cache (%p)\n", (int64_t)item_ID, queue->tree);

					oscap_free(cached->item);
					oscap_free(cached);

					/* now what? */
					abort();
				}
			}

			probe_cobj_add_item(pair->cobj, pair->p.item);
		}

		if (pthread_mutex_lock(&cache->queue_mutex) != 0) {
			dE("An error ocured while re-locking the queue mutex: %u, %s\n",
			   errno, strerror(errno));
			return (NULL);
		}
        }

        return (NULL);
}

probe_icache_t *probe_icache_new(void)
{
        probe_icache_t *cache;

        cache = oscap_talloc(probe_icache_t);

	cache->tree  = NULL;
	cache->queue = NULL;

        cache->tree  = rbt_i64_new();

        if (pthread_mutex_init(&cache->queue_mutex, NULL) != 0) {
		dE("Can't initialize icache mutex: %u, %s\n", errno, strerror(errno));
		goto fail;
	}

	cache->queue = oscap_alloc(sizeof(probe_icache_t) * (PROBE_IQUEUE_CAPACITY));

        cache->queue_beg = 0;
        cache->queue_end = 0;
        cache->queue_cnt = 0;

	if (pthread_cond_init(&cache->queue_notempty, NULL) != 0) {
		dE("Can't initialize icache queue condition variable: %u, %s\n",
		   errno, strerror(errno));
		goto fail;
	}

	if (pthread_create(&cache->thid, NULL,
			   probe_icache_worker, (void *)cache) != 0)
	{
		dE("Can't start the icache worker: %u, %s\n", errno, strerror(errno));
		goto fail;
	}

        return (cache);
fail:
	if (cache->tree != NULL)
		rbt_i64_free(cache->tree);

	pthread_mutex_destroy(&cache->queue_mutex);
	pthread_cond_destroy(&cache->queue_notempty);

	if (cache->queue != NULL)
		oscap_free(cache->queue);

	oscap_free(cache);

	return (NULL);
}

void probe_icache_free(probe_icache_t *cache)
{
	return;
}
