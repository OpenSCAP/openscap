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


#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include "../common/util.h"

OSCAP_HIDDEN_START;

struct cpe_platformspec * cpe_platformspec_parse(xmlTextReaderPtr reader);
struct cpe_platform * cpe_platform_parse(xmlTextReaderPtr reader);
struct cpe_lang_expr * cpe_ret_expr_parse(xmlTextReaderPtr reader);
struct cpe_platformspec *  cpe_lang_parse(const char *fname);

void cpe_lang_export(struct cpe_platformspec * spec, const char * fname);
void cpe_platformspec_export(const struct cpe_platformspec * spec, xmlTextWriterPtr writer);
void cpe_platform_export(const struct cpe_platform * platform, xmlTextWriterPtr writer);
void cpe_ret_expr_export(const struct cpe_lang_expr expr, xmlTextWriterPtr writer);

void cpe_langexpr_free(struct cpe_lang_expr * expr);
void cpe_platformspec_free(struct cpe_platformspec * platformspec);
void cpe_platform_free(struct cpe_platform * platform);

OSCAP_HIDDEN_END;
