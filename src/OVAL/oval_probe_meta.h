/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
#ifndef OVAL_PROBE_META_H
#define OVAL_PROBE_META_H

#include <stdint.h>
#include "public/oval_types.h"
#include "_oval_probe_handler.h"

typedef struct {
        oval_subtype_t otype;
        const char    *stype;
        oval_probe_handler_t *handler;
        uint32_t       flags;
        const char    *pname;
} oval_probe_meta_t;

extern oval_probe_meta_t __probe_meta[];
extern size_t __probe_meta_count;
extern oval_subtypedsc_t __s2n_tbl[];
extern oval_subtypedsc_t __n2s_tbl[];

#define OVAL_PROBEMETA_EXTERNAL 0x00000001 /**< pass the `pext' structure to the handler oval_probe_session_new */

/**
 * Convenience macro for defining a probe handled by the external probe handler.
 */
#define OVAL_PROBE_EXTERNAL(subtype, strtype) {(subtype), (strtype), &oval_probe_ext_handler, OVAL_PROBEMETA_EXTERNAL, "probe_"strtype}

#endif /* OVAL_PROBE_META_H */
