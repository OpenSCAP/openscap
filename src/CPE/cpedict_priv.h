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

#ifndef CPEDICT_PRIV_H_
#define CPEDICT_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <stdlib.h>

#include "cpeuri.h"
#include "../common/oscap.h"
#include "../common/util.h"
#include "../common/elements.h"

OSCAP_HIDDEN_START;

struct cpe_dict_model * cpe_dict_model_parse_xml(const struct oscap_import_source *source);
struct cpe_generator * cpe_generator_parse(xmlTextReaderPtr reader);

/*
 * New dictionary item from XML
 * @param node cpe-item node
 * @return new dictionary item
 * @retval NULL on failure
 */
struct cpe_item * cpe_item_parse(xmlTextReaderPtr reader);
struct cpe_vendor * cpe_vendor_parse(xmlTextReaderPtr reader);
struct cpe_vendor * cpe_vendor_new();
struct cpe_item * cpe_item_new();

/**
 * Create new empty CPE dictionary
 * @relates cpe_dict_model
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model *cpe_dict_model_new();

/*
 * Load new CPE dictionary from XML node
 * @param node file name of dictionary to import
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model * cpe_dict_model_parse(xmlTextReaderPtr reader);

void cpe_dict_model_export(struct cpe_dict_model * dict, const struct oscap_export_target * target);
void cpe_dict_export(const struct cpe_dict_model * dict, xmlTextWriterPtr writer);
void cpe_generator_export(const struct cpe_generator * generator, xmlTextWriterPtr writer);
void cpe_item_export(const struct cpe_item * item, xmlTextWriterPtr writer);
void cpe_vendor_export(const struct cpe_vendor * vendor, xmlTextWriterPtr writer);

OSCAP_HIDDEN_END;

#endif

