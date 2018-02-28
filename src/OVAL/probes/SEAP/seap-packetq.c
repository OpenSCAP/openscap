#include <config.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include "_seap-packet.h"
#include "_seap-packetq.h"
#include "sm_alloc.h"

int SEAP_packetq_init(SEAP_packetq_t *queue)
{
	if (queue == NULL) {
		errno = EFAULT;
		return (-1);
	}

	queue->flags = SEAP_PACKETQ_DONTFREE;
	queue->count = 0;
	queue->first = NULL;
	queue->last  = NULL;

	if (pthread_mutex_init(&queue->lock, NULL) != 0)
		return (-1);

	return (0);
}

void SEAP_packetq_free(SEAP_packetq_t *queue)
{
	if (queue == NULL)
		return;

	pthread_mutex_destroy(&queue->lock);
}

struct SEAP_packetq_item *SEAP_packetq_item_new(void)
{
	struct SEAP_packetq_item *i;

	i = sm_talloc(struct SEAP_packetq_item);

	i->next   = NULL;
	i->prev   = NULL;
	i->packet = NULL;

	return (i);
}

void SEAP_packetq_item_free(struct SEAP_packetq_item *i, bool freepacket)
{
	if (freepacket)
		SEAP_packet_free(i->packet);

	i->prev   = NULL;
	i->next   = NULL;
	i->packet = NULL;

	sm_free(i);
}

int SEAP_packetq_get(SEAP_packetq_t *queue, SEAP_packet_t **packet_dst)
{
	int count;
	struct SEAP_packetq_item *save;

	if (queue == NULL || packet_dst == NULL)
		return (-1);
	if (pthread_mutex_lock(&queue->lock) != 0)
		return (-1);

	if (queue->first == NULL) {
		if (queue->last != NULL) {
			return -1;
		}
		if (queue->count != 0) {
			return -1;
		}

		count = -1;
		goto __unlock_and_return;
	}

	if (queue->last == NULL) {
		return -1;
	}
	if (queue->count <= 0) {
		return -1;
	}

	save = queue->first->next;
	(*packet_dst) = queue->first->packet;
	SEAP_packetq_item_free(queue->first, false);
	queue->first = save;

	if (queue->first != NULL)
		queue->first->prev = NULL;
	else
		queue->last = NULL;

	count = --queue->count;

__unlock_and_return:
	if (pthread_mutex_unlock(&queue->lock) != 0)
		return (-1); /* abort()? */

	return (count);
}

int SEAP_packetq_put(SEAP_packetq_t *queue, SEAP_packet_t *packet)
{
	int count;

	if (queue == NULL)
		return (-1);
	if (pthread_mutex_lock(&queue->lock) != 0)
		return (-1);

	if (queue->first == NULL) {
		queue->first = SEAP_packetq_item_new();
		queue->first->packet = packet;
		queue->last  = queue->first;
	} else {
		if (queue->last == NULL) {
			return -1; /* XXX: unlock */
		}
		if (queue->last->next != NULL) {
			return -1;
		}

		queue->last->next = SEAP_packetq_item_new();
		queue->last->next->packet = packet;
		queue->last->next->prev   = queue->last;
	}

	count = ++queue->count;

	if (pthread_mutex_unlock(&queue->lock) != 0)
		return (-1); /* abort()? */

	return (count);
}

int SEAP_packetq_count(SEAP_packetq_t *queue)
{
	int count;

	if (queue == NULL)
		return (-1);
	if (pthread_mutex_lock(&queue->lock) != 0)
		return (-1);

	count = queue->count;

	if (pthread_mutex_unlock(&queue->lock) != 0)
		return (-1); /* abort()? */

	return (count);
}
