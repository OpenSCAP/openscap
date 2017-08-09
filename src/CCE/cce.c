/**
 * @file cce.c
 * \brief Interface to Common Configuration Enumeration (CCE)
 *
 * See more details at http://cce.mitre.org/
 */

/*
 * Copyright 2008-2014 Red Hat Inc., Durham, North Carolina.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 *      Riley C. Porter <Riley.Porter@g2-inc.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "public/cce.h"
#include "cce_priv.h"

#include "common/public/oscap.h"
#include "common/elements.h"
#include "oscap_source.h"
#include "source/oscap_source_priv.h"

#define CCE_SUPPORTED "5"

static struct cce *cce_new_empty(void)
{
	struct cce *cce = oscap_calloc(1, sizeof(struct cce));
	cce->entries = oscap_list_new();
	cce->entry = oscap_htable_new();
	return cce;
}

void cce_free(struct cce *cce)
{
	if (cce) {
		oscap_htable_free0(cce->entry);
		oscap_list_free(cce->entries, (oscap_destruct_func) cce_entry_free);
		free(cce);
	}
}

void cce_entry_free(struct cce_entry *cce)
{
	if (cce) {
		free(cce->id);
		free(cce->description);
		oscap_list_free(cce->params, free);
		oscap_list_free(cce->tech_mechs, free);
		oscap_list_free(cce->references, (oscap_destruct_func) cce_reference_free);
		free(cce);
	}
}

struct cce_entry *cce_entry_new_empty(void)
{
	struct cce_entry *cce = oscap_calloc(1, sizeof(struct cce_entry));
	cce->params = oscap_list_new();
	cce->tech_mechs = oscap_list_new();
	cce->references = oscap_list_new();
	return cce;
}

static void cce_parse(const char *docname, struct cce *cce)
{
	struct oscap_source *source = oscap_source_new_from_file(docname);
	xmlTextReaderPtr reader = oscap_source_get_xmlTextReader(source);
	int ret;
	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			process_node(reader, cce);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
	}
	oscap_source_free(source);
}

struct cce *cce_new(const char *fname)
{
	struct cce *cce = cce_new_empty();
	cce_parse(fname, cce);
	return cce;
}

const char * cce_supported(void)
{
        return CCE_SUPPORTED;
}

bool cce_validate(const char *filename)
{
	xmlParserCtxtPtr ctxt;	/* the parser context */
	xmlDocPtr doc;		/* the resulting document tree */
	bool ret = false;
	/* create a parser context */
	ctxt = xmlNewParserCtxt();
	if (ctxt == NULL)
		return false;
	/* parse the file, activating the DTD validation option */
	doc = xmlCtxtReadFile(ctxt, filename, NULL, XML_PARSE_DTDATTR);
	/* check if parsing suceeded */
	if (doc == NULL) {
		xmlFreeParserCtxt(ctxt);
		return false;
	}
	/* check if validation suceeded */
	if (ctxt->valid)
		ret = true;
	xmlFreeDoc(doc);
	/* free up the parser context */
	xmlFreeParserCtxt(ctxt);
	return ret;

}

OSCAP_IGETTER_GEN(cce_entry, cce, entries)
    OSCAP_HGETTER(struct cce_entry *, cce, entry)


OSCAP_GETTER(const char *, cce_entry, id)
OSCAP_GETTER(const char *, cce_entry, description)
OSCAP_IGETTER(oscap_string, cce_entry, params)
OSCAP_IGETTER(oscap_string, cce_entry, tech_mechs) OSCAP_IGETTER_GEN(cce_reference, cce_entry, references)
 OSCAP_GETTER(const char *, cce_reference, source) OSCAP_GETTER(const char *, cce_reference, value)
