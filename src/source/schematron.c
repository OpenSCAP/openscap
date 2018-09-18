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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#ifdef OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

#include "common/_error.h"
#include "common/util.h"
#include "oscap.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"
#include "source/schematron_priv.h"
#include "source/validate_priv.h"
#include "source/xslt_priv.h"

struct oscap_schema_table_entry OSCAP_SCHEMATRON_TABLE[] = {
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.3",          "oval/5.3/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.4",          "oval/5.4/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.5",          "oval/5.5/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.6",          "oval/5.6/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.7",          "oval/5.7/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.8",          "oval/5.8/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.9",          "oval/5.9/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10",         "oval/5.10/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.10.1",       "oval/5.10.1/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11",         "oval/5.11/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11.1",       "oval/5.11.1/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DEFINITIONS,       "5.11.2",       "oval/5.11.2/oval-definitions-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.3",          "oval/5.3/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.4",          "oval/5.4/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.5",          "oval/5.5/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.6",          "oval/5.6/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.7",          "oval/5.7/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.8",          "oval/5.8/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.9",          "oval/5.9/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.10",         "oval/5.10/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.10.1",       "oval/5.10.1/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11",         "oval/5.11/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11.1",       "oval/5.11.1/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_SYSCHAR,           "5.11.2",       "oval/5.11.2/oval-system-characteristics-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.3",          "oval/5.3/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.4",          "oval/5.4/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.5",          "oval/5.5/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.6",          "oval/5.6/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.7",          "oval/5.7/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.8",          "oval/5.8/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.9",          "oval/5.9/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.10",         "oval/5.10/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.10.1",       "oval/5.10.1/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11",         "oval/5.11/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11.1",       "oval/5.11.1/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_RESULTS,           "5.11.2",       "oval/5.11.2/oval-results-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.3",          "oval/5.3/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.4",          "oval/5.4/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.5",          "oval/5.5/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.6",          "oval/5.6/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.7",          "oval/5.7/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.8",          "oval/5.8/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.9",          "oval/5.9/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.10",         "oval/5.10/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.10.1",       "oval/5.10.1/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11",         "oval/5.11/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11.1",       "oval/5.11.1/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_VARIABLES,         "5.11.2",       "oval/5.11.2/oval-variables-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.8",          "oval/5.8/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.9",          "oval/5.9/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.10",         "oval/5.10/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.10.1",       "oval/5.10.1/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11",         "oval/5.11/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11.1",       "oval/5.11.1/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_OVAL_DIRECTIVES,        "5.11.2",       "oval/5.11.2/oval-directives-schematron.xsl"},
	{OSCAP_DOCUMENT_XCCDF,                  "1.2",          "xccdf/1.2/xccdf_1.2-schematron.xsl"},
};

int oscap_source_validate_schematron_priv(struct oscap_source *source, oscap_document_type_t scap_type, const char *version, const char *outfile)
{
	const char *params[] = { NULL };

	if (version == NULL) {
		oscap_seterr(OSCAP_EFAMILY_OSCAP, "Could not determine document version for %s",
			oscap_source_readable_origin(source));
		return -1;
	}

	/* find a right schematron file */
	for (struct oscap_schema_table_entry *entry = OSCAP_SCHEMATRON_TABLE; entry->doc_type != 0; ++entry) {
		if (entry->doc_type != scap_type || strcmp(entry->schema_version, version))
			continue;

		/* validate */
		return oscap_source_apply_xslt_path(source, entry->schema_path, outfile, params, oscap_path_to_schemas());
	}

	oscap_seterr(OSCAP_EFAMILY_OSCAP, "Schematron rules not found when trying to validate '%s'", oscap_source_readable_origin(source));
	return -1;
}
