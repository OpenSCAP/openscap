/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef _SEAP_PACKET_H
#define _SEAP_PACKET_H

#include <stdint.h>
#include "_seap-message.h"
#include "_seap-command.h"
#include "_seap-error.h"
#include "../../../common/util.h"


#define SEAP_SYM_PREFIX "seap."
#define SEAP_SYM_MSG    SEAP_SYM_PREFIX"msg"
#define SEAP_SYM_CMD    SEAP_SYM_PREFIX"cmd"
#define SEAP_SYM_ERR    SEAP_SYM_PREFIX"err"

#define SEAP_PACKET_INV 0x00 /* Invalid packet */
#define SEAP_PACKET_MSG 0x01 /* Message packet */
#define SEAP_PACKET_ERR 0x02 /* Error packet */
#define SEAP_PACKET_CMD 0x03 /* Command packet */
#define SEAP_PACKET_RAW 0x04 /* Raw packet */

struct SEAP_packet {
        uint8_t type;
        union {
                SEAP_msg_t msg;
                SEAP_err_t err;
                SEAP_cmd_t cmd;
        } data;
};
typedef struct SEAP_packet SEAP_packet_t;

SEAP_packet_t *SEAP_packet_new(void);
void SEAP_packet_free(SEAP_packet_t *packet);

void *SEAP_packet_settype(SEAP_packet_t *packet, uint8_t type);
uint8_t SEAP_packet_gettype(SEAP_packet_t *packet);

SEAP_msg_t *SEAP_packet_msg(SEAP_packet_t *packet);
SEAP_cmd_t *SEAP_packet_cmd(SEAP_packet_t *packet);
SEAP_err_t *SEAP_packet_err(SEAP_packet_t *packet);

int SEAP_packet_recv(SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet);
int SEAP_packet_recv_bytype(SEAP_CTX_t *ctx, int sd, SEAP_packet_t **packet, uint8_t type);
int SEAP_packet_send(SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet);
int SEAP_packet_enqueue(SEAP_CTX_t *ctx, int sd, SEAP_packet_t *packet);

#endif /* _SEAP_PACKET_H */
