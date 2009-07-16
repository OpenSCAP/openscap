/*
 * Common Vulnerability and Exposures
 * (http://cve.mitre.org/)
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
 *      Maros Barabas   <mbarabas@redhat.com>
 *
 */

%module cve
%{
 #include "../../src/CVE/cve.h"
%}

/* Included file: ../../src/CVE/cve.h
 * Read cve.h for more information and 
 * documentation
 *
 */
struct cve;
struct cve_info;
struct cve_reference;
struct cve_info_iterator;
struct cve_info* cve_info_iterator_next(struct cve_info_iterator* it);
bool cve_info_iterator_has_more(struct cve_info_iterator* it);
struct cve_reference_iterator;
struct cve_reference* cve_reference_iterator_next(struct cve_reference_iterator* it);
bool cve_reference_iterator_has_more(struct cve_reference_iterator* it);
struct cve* cve_new(const char* fname);
void cve_delete(struct cve* cve);
struct cve_info_iterator* cve_entries(const struct cve* cve);
struct cve_info* cve_entry_by_id(const struct cve* cve, const char* id);
const char* cve_info_id(const struct cve_info* info);
const char* cve_info_pub(const struct cve_info* info);
const char* cve_info_mod(const struct cve_info* info);
const char* cve_info_cwe(const struct cve_info* info);
const char* cve_info_summary(const struct cve_info* info);
const char* cve_info_score(const struct cve_info* info);
const char* cve_info_vector(const struct cve_info* info);
const char* cve_info_complexity(const struct cve_info* info);
const char* cve_info_authentication(const struct cve_info* info);
const char* cve_info_confidentiality(const struct cve_info* info);
const char* cve_info_integrity(const struct cve_info* info);
const char* cve_info_availability(const struct cve_info* info);
const char* cve_info_source(const struct cve_info* info);
const char* cve_info_generated(const struct cve_info* info);
struct cve_reference_iterator* cve_info_references(const struct cve_info* info);
const char* cve_reference_summary(const struct cve_reference* ref);
const char* cve_reference_href(const struct cve_reference* ref);
const char* cve_reference_type(const struct cve_reference* ref);
const char* cve_reference_source(const struct cve_reference* ref);
 /* End of cve.h */
