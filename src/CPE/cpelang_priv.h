/**
 * @file cpelang_priv.h
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

#ifndef CPELANG_PRIV_H_
#define CPELANG_PRIV_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "cpelang.h"
#include "../common/util.h"
#include "../common/elements.h"

OSCAP_HIDDEN_START;

struct cpe_lang_model * cpe_lang_model_parse(xmlTextReaderPtr reader);
struct cpe_platform * cpe_platform_parse(xmlTextReaderPtr reader);
struct cpe_testexpr * cpe_testexpr_parse(xmlTextReaderPtr reader);
struct cpe_lang_model *  cpe_lang_parse(const char *fname);

void cpe_lang_export(struct cpe_lang_model * spec, const char * fname);
void cpe_lang_model_export(const struct cpe_lang_model * spec, xmlTextWriterPtr writer);
void cpe_platform_export(const struct cpe_platform * platform, xmlTextWriterPtr writer);
void cpe_testexpr_export(const struct cpe_testexpr expr, xmlTextWriterPtr writer);

struct cpe_lang_model * cpe_lang_model_new();
struct cpe_testexpr * cpe_testexpr_new();
struct cpe_platform * cpe_platform_new();

OSCAP_HIDDEN_END;

#endif

