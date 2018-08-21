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
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ds_common.h"
#include "common/_error.h"
#include "common/list.h"
#include "common/oscap_acquire.h"
#include "source/oscap_source_priv.h"
#include "source/public/oscap_source.h"
#include "oscap_helpers.h"

#include <libxml/tree.h>

xmlDoc *ds_doc_from_foreign_node(xmlNode *node, xmlDoc *parent)
{
	xmlDOMWrapCtxtPtr wrap_ctxt = xmlDOMWrapNewCtxt();
	xmlDocPtr new_doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr res_node = NULL;
	if (xmlDOMWrapCloneNode(wrap_ctxt, parent, node, &res_node, new_doc, NULL, 1, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Error when cloning node '%s' while dumping component "
				"from DataStream", node->name);
			xmlFreeDoc(new_doc);
			xmlDOMWrapFreeCtxt(wrap_ctxt);
			return NULL;
	}
	xmlDocSetRootElement(new_doc, res_node);
	if (xmlDOMWrapReconcileNamespaces(wrap_ctxt, res_node, 0) != 0)
	{
		oscap_seterr(OSCAP_EFAMILY_XML, "Internal libxml error when reconciling namespaces "
				"for node '%s' while dumping component.", node->name);
		xmlFreeDoc(new_doc);
		xmlDOMWrapFreeCtxt(wrap_ctxt);
		return NULL;
	}
	xmlDOMWrapFreeCtxt(wrap_ctxt);
	return new_doc;
}

int ds_dump_component_sources(struct oscap_htable *component_sources, const char *target_dir)
{
	struct oscap_htable_iterator *hit = oscap_htable_iterator_new(component_sources);
	while (oscap_htable_iterator_has_more(hit)) {
		struct oscap_source *s = oscap_htable_iterator_next_value(hit);
		char *filename = target_dir == NULL ? oscap_strdup(oscap_source_readable_origin(s))
				: oscap_sprintf("%s/%s", target_dir, oscap_source_readable_origin(s));
		int ret = oscap_acquire_ensure_parent_dir(filename);
		if (ret != 0) {
			oscap_htable_iterator_free(hit);
			return ret;
		}
		ret = oscap_source_save_as(s, filename);
		free(filename);
		if (ret != 0) {
			oscap_htable_iterator_free(hit);
			return ret;
		}
	}
	oscap_htable_iterator_free(hit);
	return 0;
}
