#ifndef _SEAP_PACKETQ_H
#define _SEAP_PACKETQ_H

#include <stdint.h>
#include <pthread.h>
#include "_seap-packet.h"

struct SEAP_packetq_item {
	struct SEAP_packetq_item *next;
	struct SEAP_packetq_item *prev;
	SEAP_packet_t            *packet;
};

#define SEAP_PACKETQ_DONTFREE 0x00000001

typedef struct {
	pthread_mutex_t lock;
	uint32_t        flags;
	int             count;

	struct SEAP_packetq_item *first;
	struct SEAP_packetq_item *last;
} SEAP_packetq_t;

int  SEAP_packetq_init(SEAP_packetq_t *queue);
void SEAP_packetq_free(SEAP_packetq_t *queue);

struct SEAP_packetq_item *SEAP_packetq_item_new(void);
void SEAP_packetq_item_free(struct SEAP_packetq_item *i, bool freepacket);

int  SEAP_packetq_get(SEAP_packetq_t *queue, SEAP_packet_t **packet_dst);
int  SEAP_packetq_put(SEAP_packetq_t *queue, SEAP_packet_t *packet);

int SEAP_packetq_count(SEAP_packetq_t *queue);

#endif /* _SEAP_PACKETQ_H */
