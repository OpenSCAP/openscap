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
 *
 */
#ifndef OSCAP_SOURCE_DOC_TYPE_H
#define OSCAP_SOURCE_DOC_TYPE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxml/xmlreader.h>

#include "common/public/oscap.h"
#include "common/util.h"

OSCAP_HIDDEN_START;

/**
 * Determines the SCAP type of the document xmlTextReder. This function is deemed
 * to be private forever, as it moves with the reader context.
 * @param reader xmlTextReader to determine document type
 * @param doc_type determined document type (output parameter)
 * @returns -1 on error, 0 otherwise
 */
int oscap_determine_document_type_reader(xmlTextReader *reader, oscap_document_type_t *doc_type);

OSCAP_HIDDEN_END;
#endif
