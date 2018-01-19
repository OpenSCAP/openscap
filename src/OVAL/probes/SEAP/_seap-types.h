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
#ifndef _SEAP_TYPES_H
#define _SEAP_TYPES_H

#include <stdint.h>
#include "public/seap-types.h"
#include "_sexp-types.h"
#include "_seap-command.h"
#include "seap-descriptor.h"
#include "../../../common/util.h"


/* SEAP context */
struct SEAP_CTX {
        SEXP_t * (*parser) (SEXP_psetup_t *, const char *, size_t, SEXP_pstate_t **);
        SEXP_pflags_t pflags;
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t *sd_table;
        SEAP_cmdtbl_t *cmd_c_table;
        SEAP_cflags_t  cflags;

        uint16_t recv_timeout;
        uint16_t send_timeout;
};


#endif /* _SEAP_TYPES_H */
