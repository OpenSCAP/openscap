/*
 * Common Product Enumeration
 * (http://nvd.nist.gov/cpe.cfm)
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

%module cpe
%{
 #include "../../src/CPE/cpedict.h"
 #include "../../src/CPE/cpelang.h"
 #include "../../src/CPE/cpematch.h"
 #include "../../src/CPE/cpeuri.h"
%}

cpe_dict_t *cpe_dict_new(const char *fname);
cpe_dict_t *cpe_dict_new_empty();
bool cpe_dict_add_item(cpe_dict_t * dict, cpe_dict_item_t * item);
void cpe_dict_delete(cpe_dict_t * dict);
cpe_dict_item_t *cpe_dictitem_new_empty();
void cpe_dictitem_delete(cpe_dict_item_t * item);
void cpe_dictcheck_delete(cpe_dict_check_t * check);

cpe_platform_spec_t *cpe_platformspec_new(const char *fname);
bool cpe_platformspec_add(cpe_platform_spec_t * platformspec, cpe_platform_t * platform);
void cpe_platformspec_delete(cpe_platform_spec_t * platformspec);
bool cpe_language_match_cpe(cpe_t ** cpe, size_t n, const cpe_platform_t * platform);
void cpe_platform_delete(cpe_platform_t * platform);
void cpe_langexpr_delete(cpe_lang_expr_t * expr);

bool cpe_name_match_dict(cpe_t * cpe, cpe_dict_t * dict);
bool cpe_name_match_dict_str(const char *cpe, cpe_dict_t * dict);

cpe_t *cpe_new(const char *cpe);
char **cpe_split(char *str, const char *delim);
bool cpe_urldecode(char *str);
bool cpe_name_match_one(const cpe_t * cpe, const cpe_t * against);
bool cpe_name_match_cpes(const cpe_t * name, size_t n, cpe_t ** namelist);
int cpe_name_match_strs(const char *candidate, size_t n, char **targets);
bool cpe_check(const char *str);
char *cpe_get_uri(const cpe_t * cpe);
int cpe_write(const cpe_t * cpe, FILE * f);
bool cpe_assign_values(cpe_t * cpe, char **fields);
void cpe_delete(cpe_t * cpe);
size_t ptrarray_length(void **arr);

