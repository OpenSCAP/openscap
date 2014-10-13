/*
 * Copyright 2013--2014 Red Hat Inc., Durham, North Carolina.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <libxml/xmlreader.h>

#include "cpe_ctx_priv.h"
#include "common/alloc.h"
#include "common/_error.h"
#include "common/elements.h"

struct cpe_parser_ctx {
	xmlTextReaderPtr reader;
	bool owns_reader;
	char *schema_version;
};

static inline struct cpe_parser_ctx *_cpe_parser_ctx_new()
{
	return oscap_calloc(1, sizeof(struct cpe_parser_ctx));
}

struct cpe_parser_ctx *cpe_parser_ctx_from_reader(xmlTextReaderPtr reader)
{
	struct cpe_parser_ctx *ctx = _cpe_parser_ctx_new();
	ctx->reader = reader;
	ctx->owns_reader = false;
	return ctx;
}

void cpe_parser_ctx_free(struct cpe_parser_ctx *ctx)
{
	if (ctx) {
		if (ctx->reader != NULL && ctx->owns_reader)
			xmlFreeTextReader(ctx->reader);
		oscap_free(ctx->schema_version);
		oscap_free(ctx);
	}
}

static inline const char *_cpe_parser_ctx_get_schema_version(struct cpe_parser_ctx *ctx)
{
	return (ctx->schema_version == NULL) ? "2.3" : ctx->schema_version;
}

static int _cpe_parser_ctx_version_cmp(struct cpe_parser_ctx *ctx, const char *version)
{
#ifdef __USE_GNU
        return strverscmp(_cpe_parser_ctx_get_schema_version(ctx), version);
#else
        return strcmp(_cpe_parser_ctx_get_schema_version(ctx), version);
#endif
}

bool cpe_parser_ctx_version_gt(struct cpe_parser_ctx *ctx, const char *version)
{
	return _cpe_parser_ctx_version_cmp(ctx, version) > 0;
}

OSCAP_GETTER(xmlTextReaderPtr, cpe_parser_ctx, reader);
OSCAP_SETTER_GENERIC(cpe_parser_ctx, const char *, schema_version, oscap_free, oscap_strdup);
