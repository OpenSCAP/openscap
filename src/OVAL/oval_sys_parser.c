/**
 * @file oval_parser.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

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
 *      "Peter Vrabec" <pvrabec@redhat.com
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <libxml/xmlreader.h>
#include <stddef.h>

#include "public/oval_agent_api.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"
#include "common/util.h"
#include "common/elements.h"
#include "common/debug_priv.h"
#include "common/_error.h"

int oval_syschar_model_parse(xmlTextReaderPtr reader, struct oval_parser_context *context)
{
	int depth = xmlTextReaderDepth(reader);
	int ret = 0;

	xmlTextReaderRead(reader);
	while ((xmlTextReaderDepth(reader) > depth) && (ret != -1 )) {
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			char *tagname = (char *)xmlTextReaderLocalName(reader);
			char *namespace = (char *)xmlTextReaderNamespaceUri(reader);

			int is_ovalsys = strcmp((const char*)OVAL_SYSCHAR_NAMESPACE, namespace) == 0;
			if (is_ovalsys && (strcmp(tagname, "generator") == 0)) {
				struct oval_generator *gen;
				gen = oval_syschar_model_get_generator(context->syschar_model);
				ret = oval_parser_parse_tag(reader, context, &oval_generator_parse_tag, gen);
			} else if (is_ovalsys && (strcmp(tagname, "system_info") == 0)) {
				ret = oval_sysinfo_parse_tag(reader, context);
			} else if (is_ovalsys && (strcmp(tagname, "collected_objects") == 0)) {
				ret = oval_parser_parse_tag(reader, context, &oval_syschar_parse_tag, NULL);
			} else if (is_ovalsys && (strcmp(tagname, "system_data") == 0)) {
				ret = oval_parser_parse_tag(reader, context, &oval_sysitem_parse_tag, NULL);
			} else {
				dW("Unprocessed tag: <%s:%s>.", namespace, tagname);
				oval_parser_skip_tag(reader, context);
			}

			free(tagname);
			free(namespace);
		} else {
			if (xmlTextReaderRead(reader) != 1) {
				ret = -1;
				break;
			}
		}
	}

	return ret;
}

