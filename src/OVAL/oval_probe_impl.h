
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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
#ifndef OVAL_PROBE_IMPL_H
#define OVAL_PROBE_IMPL_H

#include <seap-types.h>
#include <config.h>

#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include "public/oval_system_characteristics.h"
#include "../common/util.h"

#include "public/oval_probe.h"

OSCAP_HIDDEN_START;

#define OVAL_PROBE_SCHEME "pipe"
#ifndef OVAL_PROBE_DIR
# define OVAL_PROBE_DIR    "/usr/libexec/openscap"
#endif

typedef struct {
	oval_subtype_t subtype;
	char *subtype_name;
	char *filename;
} oval_pdsc_t;

typedef struct {
	oval_subtype_t subtype;
	int sd;
	char *uri;
} oval_pd_t;

typedef struct {
	oval_pd_t *memb;
	size_t count;
	SEAP_CTX_t *ctx;
} oval_pdtbl_t;

#define OVALP_SDTBL_CMDDONE 0x01
#define OVALP_SDTBL_INITIALIZER { NULL, 0, NULL, 0 }

#define OVAL_PROBE_MAXRETRY 3

const oval_pdsc_t *oval_pdsc_lookup(oval_subtype_t subtype);
oval_subtype_t oval_pdsc_lookup_type(const char *name);

struct oval_pctx {
	oval_pdsc_t *pdsc_table;
	oval_pdtbl_t *pd_table;
	char *p_dir;
	struct oval_syschar_model *model;
	uint32_t p_flags;
};

#define OVAL_PCTX_SETFLAG(ctx, flg)   ((ctx)->p_flags |=  (flg))
#define OVAL_PCTX_UNSETFLAG(ctx, flg) ((ctx)->p_flags &= ~(flg))
#define OVAL_PCTX_ISSETFLAG(ctx, flg) ((ctx)->p_flags & (flg) == (flg))

OSCAP_HIDDEN_END;

#endif				/* OVAL_PROBE_IMPL_H */
