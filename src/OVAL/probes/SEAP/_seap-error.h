#pragma once
#ifndef _SEAP_ERROR_H
#define _SEAP_ERROR_H

#include "public/sexp.h"
#include "_seap-message.h"
#include "public/seap-error.h"

struct SEAP_err {
        SEAP_msgid_t id;
        uint32_t     code;
        SEXP_t      *data;
};

#endif /* _SEAP_ERROR_H */
