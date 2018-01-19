/*
 * @file cce_priv.h
 * \brief Interface to Common Configuration Enumeration (CCE) - internal definitions.
 *
 * See more details at http://cce.mitre.org/
 */

/*
 * Copyright 2008-2009 Red Hat Inc., Durham, North Carolina.
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

#include <libxml/xmlreader.h>

#include "cce.h"
#include "../common/list.h"
#include "../common/util.h"

#ifndef _CCE_PRIV_H
#define _CCE_PRIV_H


struct cce {
	struct oscap_list *entries;
	struct oscap_htable *entry;
};

struct cce_entry {
	char *id;
	char *description;
	struct oscap_list *params;	// list of C-strings
	struct oscap_list *tech_mechs;	// list of C-strings
	struct oscap_list *references;	// list of 'struct cce_reference'
};

struct cce_reference {
	char *source;
	char *value;
};

void process_node(xmlTextReaderPtr reader, struct cce *cce);
void process_description(xmlTextReaderPtr reader, struct cce_entry *cce);
void process_parameter(xmlTextReaderPtr reader, struct cce_entry *cce);
void process_tech_mech(xmlTextReaderPtr reader, struct cce_entry *cce);
void process_refs(xmlTextReaderPtr reader, struct cce_entry *cce);

struct cce_entry *cce_entry_new_empty(void);
void cce_reference_free(struct cce_reference *ref);
void cce_entry_free(struct cce_entry *ref);


#endif
