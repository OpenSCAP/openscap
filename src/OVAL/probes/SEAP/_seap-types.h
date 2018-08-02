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
#include "../../../common/util.h"
#include "generic/rbt/rbt_common.h"
#include "generic/bitmap.h"
#include "oval_types.h"

typedef uint8_t SEAP_cflags_t;

typedef struct {
	uint8_t flags;
	void *table;
	size_t maxcnt;
#if defined(SEAP_THREAD_SAFE)
	pthread_rwlock_t lock;
#endif
} SEAP_cmdtbl_t;

typedef struct {
	rbt_t *tree;
	bitmap_t *bmap;
} SEAP_desctable_t;

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


#endif /* _SEAP_TYPES_H */
