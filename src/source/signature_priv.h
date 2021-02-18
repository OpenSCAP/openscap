/*
 * Copyright 2021 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 *     Jan Černý <jcerny@redhat.com>
 */

#ifndef OSCAP_SIGNATURE_PRIV_H
#define OSCAP_SIGNATURE_PRIV_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

struct oscap_signature_ctx;

struct oscap_signature_ctx *oscap_signature_ctx_new(void);
void oscap_signature_ctx_free(struct oscap_signature_ctx *ctx);
void oscap_signature_ctx_set_pubkey_pem(struct oscap_signature_ctx *ctx, const char *path);
void oscap_signature_ctx_set_pubkey_cert_pem(struct oscap_signature_ctx *ctx, const char *path);
int oscap_signature_validate(struct oscap_source *source, struct oscap_signature_ctx *ctx);

#endif /* OSCAP_SIGNATURE_PRIV_H */