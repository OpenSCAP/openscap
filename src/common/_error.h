#pragma once
#ifndef _OSCAP_ERROR_H
#define _OSCAP_ERROR_H

#include <errno.h>
#include <libxml/xmlerror.h>
#include "public/error.h"

#define oscap_assert_errno(cond, etype, desc) \
	{ if (!(cond)) { if ((errno)) oscap_seterr(ERR_FAMILY_GLIBC, errno, desc); \
                         else oscap_seterr(ERR_FAMILY_OSCAP, (etype), desc); } }

#define oscap_seterr_errno(etype, desc) \
	{ if ((errno)) oscap_seterr(ERR_FAMILY_GLIBC, errno, desc); \
            else oscap_seterr(ERR_FAMILY_OSCAP, (etype), desc); }

#define oscap_setxmlerr(error) __oscap_setxmlerr (__FILE__, __LINE__, __PRETTY_FUNCTION__, error)

void  __oscap_setxmlerr (const char *file, uint32_t line, const char *func,
                      xmlErrorPtr error);

struct oscap_err_t {
        oscap_errfamily_t family;
        oscap_errcode_t   code;
        char             *desc;
        const char       *func;
        const char       *file;
        uint32_t          line;
};

#endif /* _OSCAP_ERROR_H */
