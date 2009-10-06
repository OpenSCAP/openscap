#pragma once
#ifndef _SEAP_PACKET_H
#define _SEAP_PACKET_H

#include <stdint.h>
#include "_seap-message.h"
#include "_seap-command.h"
#include "_seap-error.h"
#include "public/seap-packet.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

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

OSCAP_HIDDEN_END;

#endif /* _SEAP_PACKET_H */
