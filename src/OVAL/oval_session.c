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

	struct {
		struct oscap_source *variables;
		struct oscap_source *syschars;
		struct oscap_source *directives;
	} oval;
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

int oval_session_set_variables(struct oval_session *session, const char *filename)
{
	if (session == NULL) {
		return 1;
	}

	if (session->oval.variables != NULL) {
		oscap_free(session->oval.variables);
	}

	session->oval.variables = oscap_source_new_from_file(filename);
	if (oscap_source_get_scap_type(session->oval.variables) == 0) {
		return -1;
	}

	return 0;
}

int oval_session_set_syschars(struct oval_session *session, const char *filename)
{
	if (session == NULL) {
		return 1;
	}

	if (session->oval.syschars != NULL) {
		oscap_free(session->oval.syschars);
	}

	session->oval.syschars = oscap_source_new_from_file(filename);
	if (oscap_source_get_scap_type(session->oval.syschars) == 0) {
		return -1;
	}

	return 0;
}

int oval_session_set_directives(struct oval_session *session, const char *filename)
{
	if (session == NULL) {
		return 1;
	}

	if (session->oval.directives != NULL) {
		oscap_free(session->oval.directives);
	}

	session->oval.directives = oscap_source_new_from_file(filename);
	if (oscap_source_get_scap_type(session->oval.directives) == 0) {
		return -1;
	}

	return 0;
}

void oval_session_free(struct oval_session *session)
{
	if (session == NULL)
		return;

	oscap_free(session->source);
	oscap_free(session->oval.variables);
	oscap_free(session->oval.syschars);
	oscap_free(session->oval.directives);
	oscap_free(session);
}
