/**
 * @file cpedict_priv.h
 * \brief Interface to Common Platform Enumeration (CPE) Language
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
 *      Maros Barabas <mbarabas@redhat.com>
 */


#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <stdlib.h>

#include "cpeuri.h"
#include "../common/oscap.h"
#include "../common/util.h"

OSCAP_HIDDEN_START;

char *str_trim(char *str);
struct cpe_dict * cpe_dict_parse(const char *fname);
struct cpe_generator * cpe_generator_parse(xmlTextReaderPtr reader);

/*
 * New dictionary item from XML
 * @param node cpe-item node
 * @return new dictionary item
 * @retval NULL on failure
 */
struct cpe_dictitem * cpe_dictitem_parse(xmlTextReaderPtr reader);
struct cpe_dict_vendor * cpe_dict_vendor_parse(xmlTextReaderPtr reader);
struct cpe_dictitem *cpe_dictitem_new_empty();
struct cpe_dict_vendor * cpe_dictvendor_new_empty();
struct cpe_dictitem * cpe_dictitem_new_empty();

/**
 * Create new empty CPE dictionary
 * @relates cpe_dict
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict *cpe_dict_new_empty();

/**
 * Add @a item to dictionary @a dict
 *
 * @relates cpe_dict
 * @note The item will be deleted as soon as you call cpe_dict_free on the dictionary.
 * @param dict dictionary to add new item to
 * @param item pointer to item to add
 * @return true on success
 */
bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item);

/*
 * Load new CPE dictionary from XML node
 * @param node file name of dictionary to load
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict * parse_cpedict(xmlTextReaderPtr reader);

void dict_export(struct cpe_dict * dict, const char * fname);
void cpe_dict_export(const struct cpe_dict * dict, xmlTextWriterPtr writer);
void cpe_generator_export(const struct cpe_generator * generator, xmlTextWriterPtr writer);
void cpe_dictitem_export(const struct cpe_dictitem * item, xmlTextWriterPtr writer);
void cpe_dict_vendor_export(const struct cpe_dict_vendor * vendor, xmlTextWriterPtr writer);

void cpe_dict_free(struct cpe_dict * dict);

OSCAP_HIDDEN_END;
