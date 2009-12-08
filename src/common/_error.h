#pragma once
#ifndef _OSCAP_ERROR_H
#define _OSCAP_ERROR_H

#include "public/error.h"

struct oscap_err_t {
        oscap_errfamily_t family;
        oscap_errcode_t   code;
        char             *desc;
        const char       *func;
        const char       *file;
        uint32_t          line;
};

#endif /* _OSCAP_ERROR_H */
