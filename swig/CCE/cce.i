/*
 * Common Configuration Enumeration
 * (http://cce.mitre.org/)
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

%module cce
%{
 #include "../../src/CCE/public/cce.h"
%}

/* Included file: ../../src/CCE/cce.h
 * Read cce.h for more information and 
 * documentation
 *
 */

struct cce;
struct cce_entry;
struct cce_reference;
struct cce_reference_iterator;
struct cce_reference* cce_reference_iterator_next(struct cce_reference_iterator* it);
bool cce_reference_iterator_has_more(struct cce_reference_iterator* it);
void cce_reference_iterator_free(struct cce_reference_iterator* it);
struct cce_entry_iterator;
struct cce_entry* cce_entry_iterator_next(struct cce_entry_iterator* it);
bool cce_entry_iterator_has_more(struct cce_entry_iterator* it);
void cce_entry_iterator_free(struct cce_entry_iterator* it);
struct cce* cce_new(const char* fname);
void cce_free(struct cce *cce);
struct cce_entry_iterator* cce_get_entries(const struct cce* cce);
struct cce_entry* cce_get_entry(const struct cce* cce, const char* id);
bool cce_validate(const char *filename);
const char* cce_entry_get_id(const struct cce_entry* cce);
const char* cce_entry_get_description(const struct cce_entry* cce);
struct oscap_string_iterator* cce_entry_get_params(const struct cce_entry* cce);
struct oscap_string_iterator* cce_entry_get_tech_mechs(const struct cce_entry* cce);
struct cce_reference_iterator* cce_entry_get_references(const struct cce_entry* cce);
const char* cce_reference_get_source(const struct cce_reference* ref);
const char* cce_reference_get_value(const struct cce_reference* ref);
 /* End of cce.h */
