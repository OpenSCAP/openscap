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
 #include "../../src/CPE/cpeuri.h"
 #include "../../src/CPE/cpedict.h"
 #include "../../src/CPE/cpelang.h"
 #include "../../src/common/oscap.h"
%}

/* Included file: ../../src/CPE/cpeuri.h
 * Read cpeuri.h for more information and 
 * documentation
 *
 */

typedef enum {
        CPE_PART_NONE,          ///< no part specified
        CPE_PART_HW,            ///< hardware
        CPE_PART_OS,            ///< operating system
        CPE_PART_APP            ///< application
};

struct cpe_name;
struct cpe_name* cpe_name_new(const char *cpe);
void cpe_name_free(struct cpe_name * cpe);
cpe_part_t cpe_name_get_part(const struct cpe_name * cpe);
const char* cpe_name_get_vendor(const struct cpe_name * cpe);
const char* cpe_name_get_product(const struct cpe_name * cpe);
const char* cpe_name_get_version(const struct cpe_name * cpe);
const char* cpe_name_get_update(const struct cpe_name * cpe);
const char* cpe_name_get_edition(const struct cpe_name * cpe);
const char* cpe_name_get_language(const struct cpe_name * cpe);
bool cpe_name_match_one(const struct cpe_name* cpe, const struct cpe_name* against);
bool cpe_name_match_cpes(const struct cpe_name* name, size_t n, struct cpe_name** namelist);
char *cpe_name_get_uri(const struct cpe_name * cpe);
int cpe_name_write(const struct cpe_name * cpe, FILE * f);
bool cpe_name_check(const char *str);
int cpe_name_match_strs(const char *candidate, size_t n, char **targets);
/* End of cpeuri.h */

/* Included file: ../../src/CPE/cpedict.h
 * Read cpedict.h for more information and 
 * documentation
 *
 */


struct cpe_dict_check;
struct cpe_dict_reference;
struct cpe_dictitem;
struct cpe_dict;
struct cpe_dictitem_iterator;
struct cpe_dictitem* cpe_dictitem_iterator_next(struct cpe_dictitem_iterator* it);
bool cpe_dictitem_iterator_has_more(struct cpe_dictitem_iterator* it);
void cpe_dictitem_iterator_free(struct cpe_dictitem_iterator* it);
struct cpe_dict_reference_iterator;
struct cpe_dict_reference* cpe_dict_reference_iterator_next(struct cpe_dict_reference_iterator* it);
bool cpe_dict_reference_iterator_has_more(struct cpe_dict_reference_iterator* it);
void cpe_dict_reference_iterator_free(struct cpe_dict_reference_iterator* it);
struct cpe_dict_check_iterator;
struct cpe_dict_check* cpe_dict_check_iterator_next(struct cpe_dict_check_iterator* it);
bool cpe_dict_check_iterator_has_more(struct cpe_dict_check_iterator* it);
void cpe_dict_check_iterator_free(struct cpe_dict_check_iterator* it);
struct cpe_dict *cpe_dict_new(const char *fname);
struct cpe_dict *cpe_dict_new_empty(void);
bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item);
void cpe_dict_free(struct cpe_dict * dict);
struct cpe_dictitem_iterator* cpe_dict_get_items(const struct cpe_dict* item);
const char* cpe_dict_get_generator_product_name(const struct cpe_dict* item);
const char* cpe_dict_get_generator_product_version(const struct cpe_dict* item);
const char* cpe_dict_get_generator_schema_version(const struct cpe_dict* item);
const char* cpe_dict_get_generator_timestamp(const struct cpe_dict* item);
const char* cpe_dict_check_get_system(const struct cpe_dict_check* item);
const char* cpe_dict_check_get_href(const struct cpe_dict_check* item);
const char* cpe_dict_check_get_identifier(const struct cpe_dict_check* item);
const char* cpe_dict_reference_get_href(const struct cpe_dict_reference* item);
const char* cpe_dict_reference_get_content(const struct cpe_dict_reference* item);
struct cpe_name* cpe_dictitem_get_name(const struct cpe_dictitem* item);
const char* cpe_dictitem_get_title(const struct cpe_dictitem* item);
struct cpe_dict_reference_iterator* cpe_dictitem_get_references(const struct cpe_dictitem* item);
struct cpe_dict_check_iterator* cpe_dictitem_get_checks(const struct cpe_dictitem* item);
struct oscap_string_iterator* cpe_dictitem_get_notes(const struct cpe_dictitem* item);
struct cpe_name* cpe_dictitem_get_deprecated(const struct cpe_dictitem* item);
bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict * dict);
bool cpe_name_match_dict_str(const char *cpe, struct cpe_dict * dict);
/* End of cpedict.h */

/* Included file: ../../src/CPE/cpelang.h
 * Read cpelang.h for more information and 
 * documentation
 *
 */


/// CPE language operators
typedef enum {
        CPE_LANG_OPER_HALT  =  0x00,  ///< end of instruction list
        CPE_LANG_OPER_AND   =  0x01,  ///< logical AND
        CPE_LANG_OPER_OR    =  0x02,  ///< logical OR
        CPE_LANG_OPER_MATCH =  0x03,  ///< match against specified CPE

        CPE_LANG_OPER_MASK  =  0xFF,  ///< mask to extract the operator w/o possible negation
        CPE_LANG_OPER_NOT   = 0x100,  ///< negate

        CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
        CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
};
struct cpe_lang_expr;
struct cpe_platformspec;
struct cpe_platform;
struct cpe_platform_iterator;
struct cpe_platform* cpe_platform_iterator_next(struct cpe_platform_iterator* it);
bool cpe_platform_iterator_has_more(struct cpe_platform_iterator* it);
void cpe_platform_iterator_free(struct cpe_platform_iterator* it);
struct cpe_platformspec *cpe_platformspec_new(const char *fname);
void cpe_platformspec_free(struct cpe_platformspec * platformspec);
struct cpe_platform_iterator* cpe_platformspec_get_items(const struct cpe_platformspec * platformspec);
struct cpe_platform* cpe_platformspec_get_item(const struct cpe_platformspec * platformspec, const char* id);
bool cpe_platform_match_cpe(struct cpe_name ** cpe, size_t n, const struct cpe_platform * platform);
const char* cpe_platform_get_id(const struct cpe_platform* platform);
const char* cpe_platform_get_title(const struct cpe_platform* platform);
const char* cpe_platform_get_remark(const struct cpe_platform* platform);
bool cpe_platformspec_export(const struct cpe_platformspec * res, const char * fname);
/* End of cpelang.h */
