/**
 * @file cce.c
 * \brief Interface to Common Configuration Enumeration (CCE)
 *
 * See more details at http://cce.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#include "cce.h"
#include "cce_priv.h"

void cce_init(struct cce *cce)
{
	cce->id = NULL;
	cce->description = NULL;
	cce->parameters =
	    (struct list_cstring *)malloc(sizeof(struct list_cstring));
	cce->technicalmechanisms =
	    (struct list_cstring *)malloc(sizeof(struct list_cstring));
	cce->parameters->next = NULL;
	cce->parameters->value = NULL;
	cce->technicalmechanisms->next = NULL;
	cce->technicalmechanisms->value = NULL;
	cce->references = (struct list_refs *)malloc(sizeof(struct list_refs));
	cce->references->next = NULL;
	cce->references->value = NULL;
	cce->references->source = NULL;
	return;
}

void cce_clear(struct cce *cce)
{
	if (cce->id)
		xmlFree(cce->id);
	if (cce->description)
		xmlFree(cce->description);
	list_cstring_clear(cce->parameters);
	list_cstring_clear(cce->technicalmechanisms);
	list_refs_clear(cce->references);
}

void cce_parse(char *docname, struct cce *cce, char *id)
{
	xmlTextReaderPtr reader;
	xmlDocPtr doc;
	int ret;
	doc = xmlParseFile(docname);
	reader = xmlNewTextReaderFilename(docname);
	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			process_node(reader, cce, id);
			ret = xmlTextReaderRead(reader);

		}
	}
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
