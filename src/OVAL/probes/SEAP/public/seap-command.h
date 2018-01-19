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
#ifndef SEAP_COMMAND_H
#define SEAP_COMMAND_H

#include <stdint.h>
#include <stdarg.h>
#include <sexp-types.h>
#include <seap-types.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SEXP_t * (*SEAP_cmdfn_t) (SEXP_t *, void *);

typedef uint16_t SEAP_cmdcode_t;
typedef uint16_t SEAP_cmdid_t;
typedef uint8_t  SEAP_cmdtype_t;

#define SEAP_CMDTYPE_SYNC  1
#define SEAP_CMDTYPE_ASYNC 2

#define SEAP_CMDREG_LOCAL  0x00000001
#define SEAP_CMDREG_USEARG 0x00000002
#define SEAP_CMDREG_THREAD 0x00000004

OSCAP_API int SEAP_cmd_register   (SEAP_CTX_t *ctx, SEAP_cmdcode_t code, uint32_t flags, SEAP_cmdfn_t func, ...);
OSCAP_API int SEAP_cmd_unregister (SEAP_CTX_t *ctx, SEAP_cmdcode_t code);

#define SEAP_EXEC_LOCAL  0x01
#define SEAP_EXEC_LONLY  0x02
#define SEAP_EXEC_GFIRST 0x04
#define SEAP_EXEC_THREAD 0x08
#define SEAP_EXEC_WQUEUE 0x10
#define SEAP_EXEC_RECV   0x20

SEXP_t *SEAP_cmd_exec (SEAP_CTX_t    *ctx,
                       int            sd,
                       uint32_t       flags,
                       SEAP_cmdcode_t code,
                       SEXP_t        *args,
                       SEAP_cmdtype_t type,
                       SEAP_cmdfn_t   func,
                       void          *funcarg);

#ifdef __cplusplus
}
#endif

#endif /* SEAP_COMMAND_H */
