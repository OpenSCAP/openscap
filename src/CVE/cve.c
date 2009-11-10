/*! \file cve.c
 *  \brief Interface to Common Vulnerability and Exposure dictionary
 * 
 *   See details at:
 *     http://cve.mitre.org/
 *     http://nvd.nist.gov/download.cfm
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
 *      Maros Barabas  <mbarabas@redhat.com>
 *      Tomas Heinrich <theinric@redhat.com>
 *      Brian Kolbay   <Brian.Kolbay@g2-inc.com>
 */

#include "public/cve.h"
#include "../common/util.h"
#include "../common/list.h"


struct cve_model * cve_model_import(const struct oscap_import_source * source) {

    if (oscap_import_source_get_filename(source) == NULL) return NULL;

    struct cve_model *cve;

    cve = cve_model_parse_xml(source);

    return cve;
}

void cve_model_export(struct cve_model * cve, const struct oscap_export_target * target) {

    if (oscap_export_target_get_filename(target) == NULL) return;

    cve_model_export_xml(cve, target);

}
