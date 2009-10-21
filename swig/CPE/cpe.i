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
} cpe_part_t;

struct cpe_name;
struct cpe_name* cpe_name_new(const char *cpe);
void cpe_name_free(struct cpe_name * cpe);
cpe_part_t cpe_name_get_part(const struct cpe_name * cpe);
const char* cpe_name_get_product(const struct cpe_name * cpe);
const char* cpe_name_get_version(const struct cpe_name * cpe);
const char* cpe_name_get_update(const struct cpe_name * cpe);
const char* cpe_name_get_edition(const struct cpe_name * cpe);
const char* cpe_name_get_language(const struct cpe_name * cpe);
bool cpe_name_set_part(struct cpe_name * cpe, cpe_part_t newval);
bool cpe_name_set_vendor(struct cpe_name * cpe, const char *newval);
bool cpe_name_set_product(struct cpe_name * cpe, const char *newval);
bool cpe_name_set_version(struct cpe_name * cpe, const char *newval);
bool cpe_name_set_update(struct cpe_name * cpe, const char *newval);
bool cpe_name_set_edition(struct cpe_name * cpe, const char *newval);
bool cpe_name_set_language(struct cpe_name * cpe, const char *newval);
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

struct cpe_check;
struct cpe_reference;
struct cpe_item;
struct cpe_dict_model;
struct cpe_item_metadata;
struct cpe_item_title;
struct cpe_generator;
struct cpe_vendor;
struct cpe_product;
struct cpe_version;
struct cpe_update;
struct cpe_edition;
struct cpe_language;
struct cpe_item_iterator;
struct cpe_item* cpe_item_iterator_next(struct cpe_item_iterator* it);
bool cpe_item_iterator_has_more(struct cpe_item_iterator* it);
void cpe_item_iterator_free(struct cpe_item_iterator* it);
struct cpe_reference_iterator;
struct cpe_reference* cpe_reference_iterator_next(struct cpe_reference_iterator* it);
bool cpe_reference_iterator_has_more(struct cpe_reference_iterator* it);
void cpe_reference_iterator_free(struct cpe_reference_iterator* it);
struct cpe_check_iterator;
struct cpe_check* cpe_check_iterator_next(struct cpe_check_iterator* it);
bool cpe_check_iterator_has_more(struct cpe_check_iterator* it);
void cpe_check_iterator_free(struct cpe_check_iterator* it);
struct cpe_note_iterator;
struct cpe_note* cpe_note_iterator_next(struct cpe_note_iterator* it);
bool cpe_note_iterator_has_more(struct cpe_note_iterator* it);
void cpe_note_iterator_free(struct cpe_note_iterator* it);
struct cpe_vendor_iterator;
struct cpe_vendor* cpe_vendor_iterator_next(struct cpe_vendor_iterator* it);
bool cpe_vendor_iterator_has_more(struct cpe_vendor_iterator* it);
void cpe_vendor_iterator_free(struct cpe_vendor_iterator* it);
struct cpe_product_iterator;
struct cpe_product* cpe_product_iterator_next(struct cpe_product_iterator* it);
bool cpe_product_iterator_has_more(struct cpe_product_iterator* it);
void cpe_product_iterator_free(struct cpe_product_iterator* it);
struct cpe_version_iterator;
struct cpe_version* cpe_version_iterator_next(struct cpe_version_iterator* it);
bool cpe_version_iterator_has_more(struct cpe_version_iterator* it);
void cpe_version_iterator_free(struct cpe_version_iterator* it);
struct cpe_update_iterator;
struct cpe_update* cpe_update_iterator_next(struct cpe_update_iterator* it);
bool cpe_update_iterator_has_more(struct cpe_update_iterator* it);
void cpe_update_iterator_free(struct cpe_update_iterator* it);
struct cpe_edition_iterator;
struct cpe_edition* cpe_edition_iterator_next(struct cpe_edition_iterator* it);
bool cpe_edition_iterator_has_more(struct cpe_edition_iterator* it);
void cpe_edition_iterator_free(struct cpe_edition_iterator* it);
struct cpe_language_iterator;
struct cpe_language* cpe_language_iterator_next(struct cpe_language_iterator* it);
bool cpe_language_iterator_has_more(struct cpe_language_iterator* it);
void cpe_language_iterator_free(struct cpe_language_iterator* it);
const char * cpe_item_metadata_get_modification_date(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_status(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_nvd_id(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_deprecated_by_nvd_id(const struct cpe_item_metadata *item);
const char * cpe_check_get_system(const struct cpe_check *item);
const char * cpe_check_get_href(const struct cpe_check *item);
const char * cpe_check_get_identifier(const struct cpe_check *item);
const char * cpe_reference_get_href(const struct cpe_reference *item);
const char * cpe_reference_get_content(const struct cpe_reference *item);
const char * oscap_title_get_content(const struct oscap_title *item);
struct cpe_name * cpe_item_get_name(const struct cpe_item *item);
struct cpe_name * cpe_item_get_deprecated(const struct cpe_item *item);
const char * cpe_item_get_deprecation_date(const struct cpe_item *item);
struct cpe_item_metadata * cpe_item_get_metadata(const struct cpe_item *item);
struct cpe_reference_iterator * cpe_item_get_references(const struct cpe_item* item);
struct cpe_check_iterator * cpe_item_get_checks(const struct cpe_item* item);
struct oscap_title_iterator * cpe_item_get_titles(const struct cpe_item* item);
struct oscap_title_iterator * cpe_item_get_notes(const struct cpe_item* item);
const char * cpe_generator_get_product_name(const struct cpe_generator *item);
const char * cpe_generator_get_product_version(const struct cpe_generator *item);
const char * cpe_generator_get_schema_version(const struct cpe_generator *item);
const char * cpe_generator_get_timestamp(const struct cpe_generator *item);
struct cpe_generator * cpe_dict_model_get_generator(const struct cpe_dict_model *item);
struct cpe_item_iterator * cpe_dict_model_get_items(const struct cpe_dict_model *item);
struct cpe_vendor * cpe_dict_model_get_vendors(const struct cpe_dict_model *item);
const char * cpe_vendor_get_value(const struct cpe_vendor *item);
struct oscap_title_iterator * cpe_vendor_get_titles(const struct cpe_vendor *item);
struct cpe_product_iterator * cpe_vendor_get_products(const struct cpe_vendor *item);
const char * cpe_product_get_value(const struct cpe_product *item);
int cpe_product_get_part(const struct cpe_product *item);
struct cpe_version_iterator * cpe_product_get_versions(const struct cpe_product *item);
const char * cpe_version_get_value(const struct cpe_version *item);
struct cpe_update_iterator * cpe_version_get_updates(const struct cpe_version *items);
const char * cpe_update_get_value(const struct cpe_update *item);
struct cpe_edition_iterator * cpe_update_get_editions(const struct cpe_update *items);
const char * cpe_edition_get_value(const struct cpe_edition *item);
struct cpe_language_iterator * cpe_edition_get_languages(const struct cpe_edition *items);
const char * cpe_language_get_value(const struct cpe_language *item);
struct cpe_dict_model *cpe_dict_model_import(const struct oscap_import_source * source);
bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict_model * dict);
bool cpe_name_match_dict_str(const char *cpe, struct cpe_dict_model * dict);
//void oscap_title_free(struct oscap_title * title);
void cpe_check_free(struct cpe_check * check);
void cpe_reference_free(struct cpe_reference * ref);
void cpe_vendor_free(struct cpe_vendor * vendor);
void cpe_product_free(struct cpe_product * product);
void cpe_version_free(struct cpe_version * version);
void cpe_update_free(struct cpe_update * update);
void cpe_edition_free(struct cpe_edition * edition);
void cpe_language_free(struct cpe_language * language);
void cpe_itemmetadata_free(struct cpe_item_metadata * meta);
void cpe_dict_model_free(struct cpe_dict_model * dict);
void cpe_generator_free(struct cpe_generator * generator);
void cpe_item_free(struct cpe_item * item);
struct cpe_dict_mdoel * cpe_dict_model_new();
struct cpe_generator * cpe_generator_new();
struct cpe_check * cpe_check_new();
struct cpe_reference * cpe_reference_new();
struct cpe_item * cpe_item_new(); 
struct cpe_vendor *cpe_vendor_new();
struct cpe_product *cpe_product_new();
struct cpe_version *cpe_version_new();
struct cpe_update *cpe_update_new();
struct cpe_edition *cpe_edition_new();
struct cpe_language *cpe_language_new();

/* End of cpedict.h */

/* Included file: ../../src/CPE/cpelang.h
 * Read cpelang.h for more information and 
 * documentation
 *
 */

typedef enum {
        CPE_LANG_OPER_HALT  =  0x00,
        CPE_LANG_OPER_AND   =  0x01,
        CPE_LANG_OPER_OR    =  0x02,
        CPE_LANG_OPER_MATCH =  0x03,

        CPE_LANG_OPER_MASK  =  0xFF,
        CPE_LANG_OPER_NOT   = 0x100,

        CPE_LANG_OPER_NAND = CPE_LANG_OPER_AND | CPE_LANG_OPER_NOT,
        CPE_LANG_OPER_NOR = CPE_LANG_OPER_OR | CPE_LANG_OPER_NOT,
} cpe_lang_oper_t;

struct cpe_lang_model * cpe_lang_model_import(const struct oscap_import_source * source);
struct cpe_testexpr;
struct cpe_lang_model;
struct cpe_platform;
struct cpe_platform_iterator;
/// @relates cpe_platform_iterator
struct cpe_platform* cpe_platform_iterator_next(struct cpe_platform_iterator* it);
/// @relates cpe_platform_iterator
bool cpe_platform_iterator_has_more(struct cpe_platform_iterator* it);
/// @relates cpe_platform_iterator
void cpe_platform_iterator_free(struct cpe_platform_iterator* it);
cpe_lang_oper_t cpe_testexpr_get_oper(const struct cpe_testexpr *item);

struct cpe_testexpr * cpe_testexpr_get_meta_expr(const struct cpe_testexpr *item);
struct cpe_name * cpe_testexpr_get_meta_cpe(const struct cpe_testexpr *item);
const char * cpe_platform_get_id(const struct cpe_platform *item);
const char * cpe_platform_get_remark(const struct cpe_platform *item);
struct oscap_title_iterator * cpe_platform_get_titles(const struct cpe_platform *item);
struct cpe_testexpr * cpe_platform_get_expr(const struct cpe_platform *item);
const char * cpe_lang_model_get_ns_href(const struct cpe_lang_model *item);
const char * cpe_lang_model_get_ns_prefix(const struct cpe_lang_model *item);
struct cpe_platform_iterator * cpe_lang_model_get_platforms(const struct cpe_lang_model *item);
struct cpe_platform * cpe_lang_model_get_item(const struct cpe_lang_model *item, const char *key);
struct cpe_lang_model * cpe_lang_model_new();
struct cpe_testexpr * cpe_testexpr_new();
struct cpe_platform * cpe_platform_new();
void cpe_langexpr_free(struct cpe_testexpr * expr);
void cpe_lang_model_free(struct cpe_lang_model * platformspec);
void cpe_platform_free(struct cpe_platform * platform);

/* End of cpelang.h */
