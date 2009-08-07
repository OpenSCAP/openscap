#pragma once
#ifndef SEAP_PACKET_H
#define SEAP_PACKET_H

#include <seap-message.h>
#include <seap-command.h>
#include <seap-error.h>

typedef struct SEAP_packet SEAP_packet_t;

#define SEAP_PACKET_INV 0x00 /* Invalid packet */
#define SEAP_PACKET_MSG 0x01 /* Message packet */
#define SEAP_PACKET_ERR 0x02 /* Error packet */
#define SEAP_PACKET_CMD 0x03 /* Command packet */
#define SEAP_PACKET_RAW 0x04 /* Raw packet */

SEAP_packet_t *SEAP_packet_new ();
void SEAP_packet_free (SEAP_packet_t *packet);

void   *SEAP_packet_settype (SEAP_packet_t *packet, uint8_t type);
uint8_t SEAP_packet_gettype (SEAP_packet_t *packet);

SEAP_msg_t *SEAP_packet_msg (SEAP_packet_t *packet);
SEAP_cmd_t *SEAP_packet_cmd (SEAP_packet_t *packet);
SEAP_err_t *SEAP_packet_err (SEAP_packet_t *packet);

int SEAP_packet_recv (SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet);
int SEAP_packet_recv_bytype (SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet, uint8_t type);
int SEAP_packet_send (SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet);
int SEAP_packet_enqueue (SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet);

#endif /* SEAP_PACKET_H */
