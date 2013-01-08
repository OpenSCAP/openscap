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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <oscap.h>
#include <oscap_acquire.h>
#include <common/alloc.h>
#include "public/xccdf_session.h"

struct xccdf_session *xccdf_session_new(const char *filename)
{
	struct xccdf_session *session = (struct xccdf_session *) oscap_calloc(1, sizeof(struct xccdf_session));
	session->filename = strdup(filename);

	if (oscap_determine_document_type(filename, &(session->doc_type)) != 0) {
		xccdf_session_free(session);
		return NULL;
	}
	session->validate = true;
	return session;
}

void xccdf_session_free(struct xccdf_session *session)
{
	oscap_free(session->filename);
	oscap_free(session);
}

bool xccdf_session_is_sds(const struct xccdf_session *session)
{
	return session->doc_type == OSCAP_DOCUMENT_SDS;
}

void xccdf_session_set_validation(struct xccdf_session *session, bool validate, bool full_validation)
{
	session->validate = validate;
	session->full_validation = full_validation;
}
