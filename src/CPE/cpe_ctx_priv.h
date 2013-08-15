/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *	Šimon Lukašík
 */

#pragma once
#ifndef _OSCAP_CPE_CTX_PRIV_H
#define _OSCAP_CPE_CTX_PRIV_H

#include "common/util.h"

OSCAP_HIDDEN_START;

/**
 * Internal structure to hold information useful for CPE parser
 */
struct cpe_parser_ctx;

/**
 * Constructor for cpe_parser_ctx
 * @param filename File path to CPE file to parse
 * @returns parser's context
 */
struct cpe_parser_ctx *cpe_parser_ctx_new(const char *filename);

/**
 * Destructor for cpe_parser_ctx
 * @param parser's context to dispose
 */
void cpe_parser_ctx_free(struct cpe_parser_ctx *ctx);

/**
 * Get the xmlTextReader property from the context
 * @returns the XML text reader
 */
xmlTextReaderPtr cpe_parser_ctx_get_reader(const struct cpe_parser_ctx *ctx);

OSCAP_HIDDEN_END;

#endif
