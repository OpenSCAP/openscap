/*! \file cvss_priv.h
 *  \brief Interface to Common Vulnerability Scoring System Version 2
 * 
 *  See details at http://nvd.nist.gov/cvss.cfm
 *  
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

#ifndef CVSS_PRIV_H_
#define CVSS_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "../common/list.h"
#include "../common/util.h"
#include "../common/elements.h"

OSCAP_HIDDEN_START;

/**
 * @struct cvss_entry
 * Struct holding CVSS entry data
 */
struct cvss_entry;

/**
 * Parse CVSS entry
 * @param reader XML Text Reader representing XML model
 * @relates cvss_entry
 * @return new parsed CVSS entry
 */
struct cvss_entry * cvss_entry_parse(xmlTextReaderPtr reader);

/**
 * Export CVSS entry
 * @param entry CVSS entry
 * @param writer XML Text Writer representing XML model
 * @relates cvss_entry
 */
void cvss_entry_export(const struct cvss_entry * entry, xmlTextWriterPtr writer);

OSCAP_HIDDEN_END;

#endif
