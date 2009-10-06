#pragma once
#ifndef _SEAP_ERROR_H
#define _SEAP_ERROR_H

#include "public/sexp.h"
#include "_seap-message.h"
#include "public/seap-error.h"
#include "../../../common/util.h"

OSCAP_HIDDEN_START;

struct SEAP_err {
        SEAP_msgid_t id;
        uint32_t     code;
        uint8_t      type;
        SEXP_t      *data;
};

OSCAP_HIDDEN_END;

#endif /* _SEAP_ERROR_H */
