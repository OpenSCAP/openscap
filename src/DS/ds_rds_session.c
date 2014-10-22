/*
 * Copyright 2014 Red Hat Inc., Durham, North Carolina.
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
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/oscapxml.h"
#include "common/public/oscap.h"
#include "common/util.h"
#include "ds_rds_session.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include "source/xslt_priv.h"

struct ds_rds_session {
	struct oscap_source *source;            ///< Result DataStream raw representation
};

struct ds_rds_session *ds_rds_session_new_from_source(struct oscap_source *source)
{
	if (oscap_source_get_scap_type(source) != OSCAP_DOCUMENT_ARF) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not create Result DataStream "
				"session: File is not Result DataStream.");
		return NULL;
	}
	struct ds_rds_session *rds_session = (struct ds_rds_session *) oscap_calloc(1, sizeof(struct ds_rds_session));
	rds_session->source = source;
	return rds_session;
}

void ds_rds_session_free(struct ds_rds_session *rds_session)
{
	if (rds_session != NULL) {
		oscap_free(rds_session);
	}
}

char *ds_rds_session_get_html_report(struct ds_rds_session *rds_session)
{
	const char *params[] = {
		"show",              "",
		"verbosity",         "",
		"hide-profile-info", NULL,
		"oscap-version",     oscap_get_version(),
		"pwd",               NULL,
		NULL
        };
	return oscap_source_apply_xslt_path_mem(rds_session->source, "xccdf-report.xsl", params, oscap_path_to_xslt());
}
