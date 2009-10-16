/**
 * @file cpedict.c
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
 *
 * See more details at http://nvd.nist.gov/cpe.cfm
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
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/tree.h>

#include "cpedict.h"
#include "cpedict_priv.h"
#include "../common/list.h"
#include "../common/util.h"


// cpe_element_metadata -> meta structure for every XML element
// with all information that are common as "XML namespace"
struct cpe_item_metadata {
        char *modification_date;
        char *status;
        char *nvd_id;
        char *deprecated_by_nvd_id;
};

struct cpe_dict_check {
	char *system;      // system check URI
	char *href;        // external file reference (NULL if not present)
	char *identifier;  // test identifier
};


struct cpe_dict_reference {
	char *href;     // reference URL
	char *content;  // reference description
};

struct cpe_dictitem_title {
	char *content;		        // human-readable name of this item
        char *xmllang;                  // property xml:lang of title
};

struct cpe_dictitem {                   // the node <cpe-item>

	struct cpe_name *name;		// CPE name as CPE URI
        struct oscap_list* titles;      // titles of cpe-item (can be in various languages)

	struct cpe_name *deprecated;   // CPE that deprecated this one (or NULL)
	char *deprecation_date;	       // date of deprecation

	struct oscap_list* references; // list of references
	struct oscap_list* checks;     // list of checks
	struct oscap_list* notes;      // list of notesa
        struct cpe_item_metadata *metadata;    // element <meta:item-metadata>
};


struct cpe_dict {                        // the main node
	struct oscap_list* items;        // dictionary items
        struct oscap_list* vendors;

	char *generator_product_name;    // generator software name
	char *generator_product_version; // generator software version
	char *generator_schema_version;	 // generator schema version
	char *generator_timestamp;       // generation date and time
};


/* **************************
 * Component-tree structures
 * **************************/
struct cpe_dict_language {
        char *value;
};

struct cpe_dict_edition {
        char *value;
        struct oscap_list* languages;
};

struct cpe_dict_update {
        char *value;
        struct oscap_list* editions;
};

struct cpe_dict_version {
        char *value;
        struct oscap_list* updates;
};

struct cpe_dict_product {
        char *value;
        cpe_part_t part;                // enum with "h" or "a" or "o", see cpeuri
        struct oscap_list* versions;

};

struct cpe_dict_vendor {
        char *value;
        struct oscap_list* titles;
        struct oscap_list* products;
};
/****************************/


/*
 * OSCAP_GETTER(      <return value>, <structure name>, <value of structure>)
 * OSCAP_IGETTER_GEN( <return value>, <structure name>, <oscap_list *>)
 */



static const size_t CPE_DICT_CPES_INITIAL_ALLOC = 8;

bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict * dict) {
	if (cpe == NULL || dict == NULL)
		return false;
	
	size_t n = dict->items->itemcount;
	struct cpe_name** cpes = oscap_alloc(sizeof(struct cpe_name*) * n);
	struct oscap_list_item* cur = dict->items->first;

	for (int i = 0; cur != NULL; ++i) {
		cpes[i] = ((struct cpe_dictitem*)cur->data)->name;
		cur = cur->next;
	}
	
	bool ret = cpe_name_match_cpes(cpe, n, cpes);

	oscap_free(cpes);

	return ret;
}

bool cpe_name_match_dict_str(const char *cpestr, struct cpe_dict * dict)
{
	bool ret;
	if (cpestr == NULL)
		return false;
	struct cpe_name *cpe = cpe_name_new(cpestr);
	if (cpe == NULL) return false;
	ret = cpe_name_match_dict(cpe, dict);
	cpe_name_free(cpe);
	return ret;
}
