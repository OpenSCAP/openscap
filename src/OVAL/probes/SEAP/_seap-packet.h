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
#include "public/seap-packet.h"
#include "../../../common/util.h"


#define SEAP_SYM_PREFIX "seap."
#define SEAP_SYM_MSG    SEAP_SYM_PREFIX"msg"
#define SEAP_SYM_CMD    SEAP_SYM_PREFIX"cmd"
#define SEAP_SYM_ERR    SEAP_SYM_PREFIX"err"

struct SEAP_packet {
        uint8_t type;
        union {
                SEAP_msg_t msg;
                SEAP_err_t err;
                SEAP_cmd_t cmd;
        } data;
};


#endif /* _SEAP_PACKET_H */
