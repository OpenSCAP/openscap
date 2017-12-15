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
#ifndef SEXP_PARSE_H
#define SEXP_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sexp-types.h>
#include "oscap_export.h"

typedef struct SEXP_psetup SEXP_psetup_t;

OSCAP_API SEXP_psetup_t *SEXP_psetup_new  (void);
OSCAP_API void           SEXP_psetup_free (SEXP_psetup_t *);

typedef uint8_t SEXP_pflags_t;

#define SEXP_PFLAG_EOFOK   0x01
#define SEXP_PFLAG_FREEBUF 0x02
#define SEXP_PFLAG_ALL     0x03

OSCAP_API int SEXP_psetup_setflags (SEXP_psetup_t *psetup, SEXP_pflags_t flags);
OSCAP_API int SEXP_psetup_unsetflags (SEXP_psetup_t *psetup, SEXP_pflags_t flags);

typedef struct SEXP_pstate SEXP_pstate_t;

OSCAP_API SEXP_pstate_t *SEXP_pstate_new (void);
OSCAP_API void           SEXP_pstate_free (SEXP_pstate_t *);
OSCAP_API SEXP_pstate_t *SEXP_pstate_init (SEXP_pstate_t *);

OSCAP_API SEXP_t *SEXP_parse (const SEXP_psetup_t *psetup, char *buffer, size_t buflen, SEXP_pstate_t **pstate);

OSCAP_API bool SEXP_pstate_errorp(SEXP_pstate_t *pstate);

#ifdef __cplusplus
}
#endif

#endif /* SEXP_PARSE_H */
