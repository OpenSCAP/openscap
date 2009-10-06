#pragma once
#ifndef _SEAP_MESSAGE_H
#define _SEAP_MESSAGE_H

#include <stdint.h>
#include "public/sexp-types.h"
#include "public/seap-message.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

struct SEAP_attr {
        char   *name;
        SEXP_t *value;
};

struct SEAP_msg {
        SEAP_msgid_t id;
        SEAP_attr_t *attrs;
        uint16_t     attrs_cnt;
        SEXP_t      *sexp;
};

OSCAP_HIDDEN_END;

#endif /* _SEAP_MESSAGE_H */
