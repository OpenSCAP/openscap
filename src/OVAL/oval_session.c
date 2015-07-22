/*
 * Copyright 2015 Red Hat Inc., Durham, North Carolina.
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
 * Author:
 * 		Michal Šrubař <msrubar@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/util.h"
#include "common/_error.h"
#include "public/oval_session.h"
#include "oscap_source.h"

struct oval_session {
	/* Main source assigned with the main file (SDS or OVAL) */
	struct oscap_source *source;
};

struct oval_session *oval_session_new(const char *filename)
{
	oscap_document_type_t scap_type;
	struct oval_session *session;

	session = (struct oval_session *) oscap_calloc(1, sizeof(struct oval_session));

	session->source = oscap_source_new_from_file(filename);
	if ((scap_type = oscap_source_get_scap_type(session->source)) == 0) {
		oval_session_free(session);
		return NULL;
	}

	if (scap_type != OSCAP_DOCUMENT_OVAL_DEFINITIONS &&
			scap_type != OSCAP_DOCUMENT_OVAL_VARIABLES &&
			scap_type != OSCAP_DOCUMENT_OVAL_SYSCHAR &&
			scap_type != OSCAP_DOCUMENT_OVAL_DIRECTIVES &&
			scap_type != OSCAP_DOCUMENT_OVAL_RESULTS &&
			scap_type != OSCAP_DOCUMENT_SDS) {

		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Session input file was determined but it"
				" isn't an OVAL file nor a source datastream file.");
		oval_session_free(session);
		return NULL;
	}

	return session;
}

void oval_session_free(struct oval_session *session)
{
	if (session == NULL)
		return;

	oscap_free(session->source);
	oscap_free(session);
}
