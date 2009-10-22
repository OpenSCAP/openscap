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
 #include "../../src/CVE/public/cve.h"
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
void cve_info_iterator_free(struct cve_info_iterator* it);
struct cve_reference_iterator;
struct cve_reference* cve_reference_iterator_next(struct cve_reference_iterator* it);
bool cve_reference_iterator_has_more(struct cve_reference_iterator* it);
void cve_reference_iterator_free(struct cve_reference_iterator* it);
struct cve* cve_new(const char* fname);
void cve_free(struct cve* cve);
struct cve_info_iterator* cve_get_entries(const struct cve* cve);
struct cve_info* cve_get_entry(const struct cve* cve, const char* id);
const char* cve_info_get_id(const struct cve_info* info);
const char* cve_info_get_pub(const struct cve_info* info);
const char* cve_info_get_mod(const struct cve_info* info);
const char* cve_info_get_cwe(const struct cve_info* info);
const char* cve_info_get_summary(const struct cve_info* info);
const char* cve_info_get_score(const struct cve_info* info);
const char* cve_info_get_vector(const struct cve_info* info);
const char* cve_info_get_complexity(const struct cve_info* info);
const char* cve_info_get_authentication(const struct cve_info* info);
const char* cve_info_get_confidentiality(const struct cve_info* info);
const char* cve_info_get_integrity(const struct cve_info* info);
const char* cve_info_get_availability(const struct cve_info* info);
const char* cve_info_get_source(const struct cve_info* info);
const char* cve_info_get_generated(const struct cve_info* info);
struct cve_reference_iterator* cve_info_get_references(const struct cve_info* info);
const char* cve_reference_get_summary(const struct cve_reference* ref);
const char* cve_reference_get_href(const struct cve_reference* ref);
const char* cve_reference_get_type(const struct cve_reference* ref);
const char* cve_reference_get_source(const struct cve_reference* ref);
 /* End of cve.h */
