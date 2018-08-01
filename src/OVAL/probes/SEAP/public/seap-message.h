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
#ifndef SEAP_MESSAGE_H
#define SEAP_MESSAGE_H

#include <stdint.h>
#include <stdbool.h>
#include <sexp-types.h>
#include "oscap_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SEAP_MSGID_BITS == 64
typedef uint64_t SEAP_msgid_t;
#else
typedef uint32_t SEAP_msgid_t;
#endif

typedef struct SEAP_msg  SEAP_msg_t;
typedef struct SEAP_attr SEAP_attr_t;

OSCAP_API SEAP_msg_t *SEAP_msg_new (void);
OSCAP_API SEAP_msg_t *SEAP_msg_clone (SEAP_msg_t *msg);
OSCAP_API void        SEAP_msg_free (SEAP_msg_t *msg);

OSCAP_API SEAP_msgid_t SEAP_msg_id (SEAP_msg_t *msg);

OSCAP_API int     SEAP_msg_set (SEAP_msg_t *msg, SEXP_t *sexp);
OSCAP_API void    SEAP_msg_unset (SEAP_msg_t *msg);
OSCAP_API SEXP_t *SEAP_msg_get (SEAP_msg_t *msg);

OSCAP_API int     SEAP_msgattr_set (SEAP_msg_t *msg, const char *name, SEXP_t *value);
OSCAP_API bool    SEAP_msgattr_exists (SEAP_msg_t *msg, const char *name);

#include <stdio.h>
OSCAP_API void SEAP_msg_print (FILE *fp, SEAP_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* SEAP_MESSAGE_H */
