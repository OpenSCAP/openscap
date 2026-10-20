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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#pragma once
#ifndef _SEAP_TYPES_H
#define _SEAP_TYPES_H

#include <stdint.h>
#include "_sexp-types.h"
#include "../../../common/util.h"
#include "generic/rbt/rbt_common.h"
#include "generic/bitmap.h"
#include "oval_types.h"

typedef uint8_t SEAP_cflags_t;

typedef struct {
        rbt_t       *tree;
        bitmap_t    *bmap;
} SEAP_desctable_t;

typedef struct {
        uint8_t  flags;
        void    *table;
        size_t   maxcnt;
#if defined(SEAP_THREAD_SAFE)
        pthread_rwlock_t lock;
#endif
} SEAP_cmdtbl_t;

/* SEAP context */
struct SEAP_CTX {
        SEXP_format_t fmt_in;
        SEXP_format_t fmt_out;
        SEAP_desctable_t *sd_table;
        SEAP_cmdtbl_t *cmd_c_table;
        SEAP_cflags_t  cflags;

        uint16_t recv_timeout;
        uint16_t send_timeout;
	oval_subtype_t subtype;
};
typedef struct SEAP_CTX SEAP_CTX_t;

typedef uint8_t SEAP_cmdclass_t;
typedef uint16_t SEAP_cmdcode_t;
typedef uint16_t SEAP_cmdid_t;
typedef uint8_t SEAP_cmdtype_t;
typedef SEXP_t * (*SEAP_cmdfn_t) (SEXP_t *, void *);

#define SEAP_CTX_INITIALIZER { NULL, 0, 0, 0, SEAP_DESCTBL_INITIALIZER, SEAP_CMDTABLE_INITIALIZER }

/* SEAP errors */
#define SEAP_ETYPE_INT  0 /* Internal error */
#define SEAP_ETYPE_USER 1 /* User-defined error */

#define SEAP_EUNFIN 1  /* Can't finish parsing */
#define SEAP_EPARSE 2  /* Parsing error */
#define SEAP_ECLOSE 3  /* Connection close */
#define SEAP_EINVAL 4  /* Invalid argument */
#define SEAP_ENOMEM 5  /* Cannot allocate memory */
#define SEAP_EMSEXP 6  /* Missing required S-exp/value */
#define SEAP_EMATTR 7  /* Missing required attribute */
#define SEAP_EUNEXP 8  /* Unexpected error */
#define SEAP_EUSER  9  /* User-defined error */
#define SEAP_ENOCMD 10 /* Unknown cmd */
#define SEAP_EQFULL 11 /* Queue full */
#define SEAP_EUNKNOWN 255 /* Unknown/Unexpected error */

/* SEAP I/O flags */
#define SEAP_IOFL_RECONN   0x00000001 /* Try to reconnect */
#define SEAP_IOFL_NONBLOCK 0x00000002 /* Non-blocking mode */

#endif /* _SEAP_TYPES_H */
