/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpedict CPE Dictionary
 * @{
 *
 *
 * @file cpedict.h
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef CPEDICT_H_
#define CPEDICT_H_

#include "cpeuri.h"
#include "oscap.h"

// forward declaration
struct oscap_title;

/**
 * @struct cpe_check
 * Structure representing single CPE check.
 */
struct cpe_check;

/**
 * @struct cpe_reference
 * CPE dictionary item reference.
 */
struct cpe_reference;

/**
 * @struct cpe_item
 * Structure representing single CPE dictionary item.
 */
struct cpe_item;

/**
 * @struct cpe_dict_model
 * Structure representing a CPE dictionary.
 */
struct cpe_dict_model;

/**
 * @struct cpe_item_metadata
 * Structure representing metadata of CPE item
 */
struct cpe_item_metadata;

/**
 * @struct cpe_generator
 * Structure with information about document
 */
struct cpe_generator;

/**
 * @struct cpe_vendor
 * Structure with information about vendor
 */
struct cpe_vendor;
/**
 * @struct cpe_product
 * Product of some vendor
 */
struct cpe_product;
/**
 * @struct cpe_version
 * Version of product
 */
struct cpe_version;
/**
 * @struct cpe_update
 * Update of product version
 */
struct cpe_update;
/**
 * @struct cpe_edition
 * Edition of product update
 */
struct cpe_edition;
/**
 * @struct cpe_language
 * Language of product edition
 */
struct cpe_language;

/** @struct cpe_item_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_item_iterator;

/** Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @relates cpe_item_iterator
 */
struct cpe_item* cpe_item_iterator_next(struct cpe_item_iterator* it);

/** Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @relates cpe_item_iterator
 */
bool cpe_item_iterator_has_more(struct cpe_item_iterator* it);

/** Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @relates cpe_item_iterator
 */
void cpe_item_iterator_free(struct cpe_item_iterator* it);

/** @struct cpe_reference_iterator
 * Iterator over CPE dictionary references.
 * @see oscap_iterator
 */
struct cpe_reference_iterator;

/** Iterator over CPE item reference items.
 * @see oscap_iterator
 * @relates cpe_reference_iterator
 */
struct cpe_reference* cpe_reference_iterator_next(struct cpe_reference_iterator* it);

/** Iterator over CPE item reference items.
 * @see oscap_iterator
 * @relates cpe_reference_iterator
 */
bool cpe_reference_iterator_has_more(struct cpe_reference_iterator* it);

/** Iterator over CPE item reference items.
 * @see oscap_iterator
 * @relates cpe_reference_iterator
 */
void cpe_reference_iterator_free(struct cpe_reference_iterator* it);

/** @struct cpe_check_iterator
 * Iterator over CPE dictionary checks.
 * @see oscap_iterator
 */
struct cpe_check_iterator;

/** Iterator over CPE item check items.
 * @see oscap_iterator
 * @relates cpe_check_iterator
 */
struct cpe_check* cpe_check_iterator_next(struct cpe_check_iterator* it);

/** Iterator over CPE item check items.
 * @see oscap_iterator
 * @relates cpe_check_iterator
 */
bool cpe_check_iterator_has_more(struct cpe_check_iterator* it);

/** Iterator over CPE item check items.
 * @see oscap_iterator
 * @relates cpe_check_iterator
 */
void cpe_check_iterator_free(struct cpe_check_iterator* it);

/** @struct cpe_vendor_iterator
 * Iterator over CPE dictionary item vendors.
 * @see oscap_iterator
 */
struct cpe_vendor_iterator;

/** Iterator over CPE vendor items.
 * @see oscap_iterator
 * @relates cpe_vendor_iterator
 */
struct cpe_vendor* cpe_vendor_iterator_next(struct cpe_vendor_iterator* it);

/** Iterator over CPE vendor items.
 * @see oscap_iterator
 * @relates cpe_vendor_iterator
 */
bool cpe_vendor_iterator_has_more(struct cpe_vendor_iterator* it);

/** Iterator over CPE vendor items.
 * @see oscap_iterator
 * @relates cpe_vendor_iterator
 */
void cpe_vendor_iterator_free(struct cpe_vendor_iterator* it);

/** @struct cpe_product_iterator
 * Iterator over CPE dictionary item products.
 * @see oscap_iterator
 */
struct cpe_product_iterator;

/** Iterator over CPE product items.
 * @see oscap_iterator
 * @relates cpe_product_iterator
 */
struct cpe_product* cpe_product_iterator_next(struct cpe_product_iterator* it);

/** Iterator over CPE product items.
 * @see oscap_iterator
 * @relates cpe_product_iterator
 */
bool cpe_product_iterator_has_more(struct cpe_product_iterator* it);

/** Iterator over CPE product items.
 * @see oscap_iterator
 * @relates cpe_product_iterator
 */
void cpe_product_iterator_free(struct cpe_product_iterator* it);

/** @struct cpe_version_iterator
 * Iterator over CPE dictionary item versions.
 * @see oscap_iterator
 */
struct cpe_version_iterator;

/** Iterator over CPE version items.
 * @see oscap_iterator
 * @relates cpe_version_iterator
 */
struct cpe_version* cpe_version_iterator_next(struct cpe_version_iterator* it);

/** Iterator over CPE version items.
 * @see oscap_iterator
 * @relates cpe_version_iterator
 */
bool cpe_version_iterator_has_more(struct cpe_version_iterator* it);

/** Iterator over CPE version items.
 * @see oscap_iterator
 * @relates cpe_version_iterator
 */
void cpe_version_iterator_free(struct cpe_version_iterator* it);

/** @struct cpe_update_iterator
 * Iterator over CPE dictionary item updates.
 * @see oscap_iterator
 */
struct cpe_update_iterator;

/** Iterator over CPE update items.
 * @see oscap_iterator
 * @relates cpe_update_iterator
 */
struct cpe_update* cpe_update_iterator_next(struct cpe_update_iterator* it);

/** Iterator over CPE update items.
 * @see oscap_iterator
 * @relates cpe_update_iterator
 */
bool cpe_update_iterator_has_more(struct cpe_update_iterator* it);

/** Iterator over CPE update items.
 * @see oscap_iterator
 * @relates cpe_update_iterator
 */
void cpe_update_iterator_free(struct cpe_update_iterator* it);

/** @struct cpe_edition_iterator
 * Iterator over CPE dictionary item editions.
 * @see oscap_iterator
 */
struct cpe_edition_iterator;

/** Iterator over CPE edition items.
 * @see oscap_iterator
 * @relates cpe_edition_iterator
 */
struct cpe_edition* cpe_edition_iterator_next(struct cpe_edition_iterator* it);

/** Iterator over CPE edition items.
 * @see oscap_iterator
 * @relates cpe_edition_iterator
 */
bool cpe_edition_iterator_has_more(struct cpe_edition_iterator* it);

/** Iterator over CPE edition items.
 * @see oscap_iterator
 * @relates cpe_edition_iterator
 */
void cpe_edition_iterator_free(struct cpe_edition_iterator* it);

/** @struct cpe_language_iterator
 * Iterator over CPE dictionary item languages.
 * @see oscap_iterator
 */
struct cpe_language_iterator;

/** Iterator over CPE language items.
 * @see oscap_iterator
 * @relates cpe_language_iterator
 */
struct cpe_language* cpe_language_iterator_next(struct cpe_language_iterator* it);

/** Iterator over CPE language items.
 * @see oscap_iterator
 * @relates cpe_language_iterator
 */
bool cpe_language_iterator_has_more(struct cpe_language_iterator* it);

/** Iterator over CPE language items.
 * @see oscap_iterator
 * @relates cpe_language_iterator
 */
void cpe_language_iterator_free(struct cpe_language_iterator* it);

/** cpe_item_metadata function to get date
 * @relates cpe_item_metadata
 * @param item metadata of CPE item
 */
const char * cpe_item_metadata_get_modification_date(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get status
 * @relates cpe_item_metadata
 * @param item metadata of CPE item
 */
const char * cpe_item_metadata_get_status(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get nvd ID
 * @relates cpe_item_metadata
 * @param item metadata of CPE item
 */
const char * cpe_item_metadata_get_nvd_id(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get NVD ID of deprecated item
 * @relates cpe_item_metadata
 * @param item metadata of CPE item
 */
const char * cpe_item_metadata_get_deprecated_by_nvd_id(const struct cpe_item_metadata *item);

/** cpe_check functions to get system
 * @relates cpe_check
 * @param item CPE check item
 */
const char * cpe_check_get_system(const struct cpe_check *item);

/** cpe_check functions to get href
 * @relates cpe_check
 * @param item CPE check item
 */
const char * cpe_check_get_href(const struct cpe_check *item);

/** cpe_check functions to get identifier
 * @relates cpe_check
 * @param item CPE check item
 */
const char * cpe_check_get_identifier(const struct cpe_check *item);

/** cpe_reference functions to get href of reference
 * @relates cpe_reference
 * @param item CPE reference item
 */
const char * cpe_reference_get_href(const struct cpe_reference *item);

/** cpe_reference functions to get content of reference
 * @relates cpe_reference
 * @param item CPE reference item
 */
const char * cpe_reference_get_content(const struct cpe_reference *item);

/** oscap_title functions to get variable members
 * @relates oscap_title
 * @param item title elements
 */
const char * oscap_title_get_content(const struct oscap_title *item);

/**
 * Write the dict_model to a file.
 * @relates cpe_dict_model
 */ 
void cpe_dict_model_export(struct cpe_dict_model * dict, const struct oscap_export_target * target);

/** cpe_item functions to get variable member name
 * @relates cpe_item
 * @param item CPE item
 */
struct cpe_name * cpe_item_get_name(const struct cpe_item *item);

/** cpe_item functions to get variable member deprecated
 * @relates cpe_item
 * @param item CPE item
 */
struct cpe_name * cpe_item_get_deprecated(const struct cpe_item *item);

/** cpe_item functions to get variable member date
 * @relates cpe_item
 * @param item CPE item
 */
const char * cpe_item_get_deprecation_date(const struct cpe_item *item);

/** cpe_item functions to get metadata of cpe_item
 * @relates cpe_item
 * @param item CPE item
 */
struct cpe_item_metadata * cpe_item_get_metadata(const struct cpe_item *item);

/** cpe_item functions to get CPE references
 * @relates cpe_item
 * @param item CPE item
 */
struct cpe_reference_iterator * cpe_item_get_references(const struct cpe_item* item);

/** cpe_item functions to get CPE checks
 * @relates cpe_item
 * @param item CPE item
 */
struct cpe_check_iterator * cpe_item_get_checks(const struct cpe_item* item);

/** cpe_item functions to get CPE titles
 * @relates cpe_item
 * @param item CPE item
 */
struct oscap_title_iterator * cpe_item_get_titles(const struct cpe_item* item);

/** cpe_item functions to get CPE notes
 * @relates cpe_item
 * @param item CPE item
 */
struct oscap_title_iterator * cpe_item_get_notes(const struct cpe_item* item);

/** cpe_generator functions to get product name
 * @relates cpe_generator
 * @param item document generator
 */ 
const char * cpe_generator_get_product_name(const struct cpe_generator *item);

/** cpe_generator functions to get product version
 * @relates cpe_generator
 * @param item document generator
 */ 
const char * cpe_generator_get_product_version(const struct cpe_generator *item);

/** cpe_generator functions to get document schema version
 * @relates cpe_generator
 * @param item document generator
 */ 
const char * cpe_generator_get_schema_version(const struct cpe_generator *item);

/** cpe_generator functions to get timestamp from generator
 * @relates cpe_generator
 * @param item document generator
 */ 
const char * cpe_generator_get_timestamp(const struct cpe_generator *item);

/** cpe_dict_model functions to get generator from CPE dictionary model
 * @relates cpe_dict_model
 * @relates cpe_generator
 * @param item dictionary model
 */
struct cpe_generator * cpe_dict_model_get_generator(const struct cpe_dict_model *item);

/** cpe_dict_model functions to get CPE items
 * @relates cpe_dict_model
 * @param item dictionary model
 */
struct cpe_item_iterator * cpe_dict_model_get_items(const struct cpe_dict_model *item);

/** cpe_dict_model functions to get vendors
 * @relates cpe_dict_model
 * @param item dictionary model
 */
struct cpe_vendor_iterator * cpe_dict_model_get_vendors(const struct cpe_dict_model *item);

/** cpe_vendor functions to get vendor value
 * @relates cpe_vendor
 * @param item cpe_vendor
 */
const char * cpe_vendor_get_value(const struct cpe_vendor *item);

/** cpe_vendor functions to get vendor titles
 * @relates cpe_vendor
 * @param item cpe_vendor
 */ 
struct oscap_title_iterator * cpe_vendor_get_titles(const struct cpe_vendor *item);

/** cpe_vendor functions to get vendor products
 * @relates cpe_vendor
 * @param item cpe_vendor
 */
struct cpe_product_iterator * cpe_vendor_get_products(const struct cpe_vendor *item);

/** cpe_product functions to get product value
 * @relates cpe_product
 * @param item cpe_product
 */
const char * cpe_product_get_value(const struct cpe_product *item);

/** cpe_product functions to get product part
 * @relates cpe_product
 * @param item cpe_product
 */
int cpe_product_get_part(const struct cpe_product *item);

/** cpe_product functions to get versions of product
 * @relates cpe_product
 * @relates cpe_version
 * @param item cpe_product
 */
struct cpe_version_iterator * cpe_product_get_versions(const struct cpe_product *item);

/** cpe_version functions to get vupdates of versions
 * @relates cpe_version
 * @param item cpe_version
 */
const char * cpe_version_get_value(const struct cpe_version *item);

/** cpe_version functions to get value of version
 * @relates cpe_version
 * @relates cpe_update
 * @param item cpe_version 
 */
struct cpe_update_iterator * cpe_version_get_updates(const struct cpe_version *items);

/** cpe_update functions to get updates of version
 * @relates cpe_update
 * @param item
 */
const char * cpe_update_get_value(const struct cpe_update *item);

/** cpe_update functions to get editions of update
 * @relates cpe_update
 * @relates cpe_edition
 * @param item cpe_update of product element
 */
struct cpe_edition_iterator * cpe_update_get_editions(const struct cpe_update *items);

/** cpe_edition functions to get value of edition
 * @relates cpe_edition
 * @param item cpe_edition of update
 */
const char * cpe_edition_get_value(const struct cpe_edition *item);

/** cpe_edition functions to get languages of edition
 * @relates cpe_edition
 * @relates cpe_language
 * @param item cpe_edition of update
 */
struct cpe_language_iterator * cpe_edition_get_languages(const struct cpe_edition *items);

/** cpe_language functions to get value of language
 * @relates cpe_language
 * @param item language
 */
const char * cpe_language_get_value(const struct cpe_language *item);

/** Load new CPE onary from file
 * @relates cpe_dict
 * @param fname file name of dictionary to import
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model *cpe_dict_model_import(const struct oscap_import_source * source);

/** Frees CPE dictionary and its contents
 * @relates cpe_dict
 * @param dict dictionary to be deleted
 */
void cpe_dict_model_free(struct cpe_dict_model * dict);

/** Verify wether given CPE is known according to specified dictionary
 * @relates cpe_name
 * @relates cpe_dict
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict_model * dict);

/** Verify if CPE given by string is known according to specified dictionary
 * @relates cpe_name
 * @relates cpe_dict_model
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
bool cpe_name_match_dict_str(const char *cpe, struct cpe_dict_model * dict);

/*
 * Free functions
 */
//void oscap_title_free(struct oscap_title * title);
/// @relates cpe_check
void cpe_check_free(struct cpe_check * check);
/// @relates cpe_reference
void cpe_reference_free(struct cpe_reference * ref);
/// @relates cpe_vendor
void cpe_vendor_free(struct cpe_vendor * vendor);
/// @relates cpe_product
void cpe_product_free(struct cpe_product * product);
/// @relates cpe_version
void cpe_version_free(struct cpe_version * version);
/// @relates cpe_update
void cpe_update_free(struct cpe_update * update);
/// @relates cpe_edition
void cpe_edition_free(struct cpe_edition * edition);
/// @relates cpe_language
void cpe_language_free(struct cpe_language * language);
/// @relates cpe_itemmetadata
void cpe_itemmetadata_free(struct cpe_item_metadata * meta);
/// @relates cpe_dict_model
void cpe_dict_model_free(struct cpe_dict_model * dict);
/// @relates cpe_generator
void cpe_generator_free(struct cpe_generator * generator);
/// @relates cpe_item
void cpe_item_free(struct cpe_item * item);

/*
 * New functions
 */
/// @relates cpe_dict_model
struct cpe_dict_model  *cpe_dict_model_new();
/// @relates cpe_generator
struct cpe_generator *cpe_generator_new();
/// @relates cpe_check
struct cpe_check     *cpe_check_new();
/// @relates cpe_reference
struct cpe_reference *cpe_reference_new();
/// @relates cpe_item
struct cpe_item      *cpe_item_new();
/// @relates cpe_vendor
struct cpe_vendor    *cpe_vendor_new();
/// @relates cpe_product
struct cpe_product   *cpe_product_new();
/// @relates cpe_version
struct cpe_version   *cpe_version_new();
/// @relates cpe_update
struct cpe_update    *cpe_update_new();
/// @relates cpe_edition
struct cpe_edition   *cpe_edition_new();
/// @relates cpe_language
struct cpe_language  *cpe_language_new();


/*
 * Setter functions.
 */

/// @relates cpe_item
bool cpe_item_set_deprecation_date(struct cpe_item *item, const char *new_deprecation_date);

/// @relates cpe_item_metadata
bool cpe_item_metadata_set_modification_date(struct cpe_item_metadata *item_metadata, const char *new_modification_date);

/// @relates cpe_item_metadata
bool cpe_item_metadata_set_status(struct cpe_item_metadata *item_metadata, const char *new_status);

/// @relates cpe_item_metadata
bool cpe_item_metadata_set_nvd_id(struct cpe_item_metadata *item_metadata, const char *new_nvd_id);

/// @relates cpe_item_metadata
bool cpe_item_metadata_set_deprecated_by_nvd_id(struct cpe_item_metadata *item_metadata, const char *new_deprecated_by_nvd_id);

/// @relates cpe_check
bool cpe_check_set_system(struct cpe_check *check, const char *new_system);

/// @relates cpe_check
bool cpe_check_set_href(struct cpe_check *check, const char *new_href);

/// @relates cpe_check
bool cpe_check_set_identifier(struct cpe_check *check, const char *new_identifier);

/// @relates cpe_reference
bool cpe_reference_set_href(struct cpe_reference *reference, const char *new_href);

/// @relates cpe_reference
bool cpe_reference_set_content(struct cpe_reference *reference, const char *new_content);

/// @relates cpe_generator
bool cpe_generator_set_product_name(struct cpe_generator *generator, const char *new_product_name);

/// @relates cpe_generator
bool cpe_generator_set_product_version(struct cpe_generator *generator, const char *new_product_version);

/// @relates cpe_generator
bool cpe_generator_set_schema_version(struct cpe_generator *generator, const char *new_schema_version);

/// @relates cpe_generator
bool cpe_generator_set_timestamp(struct cpe_generator *generator, const char *new_timestamp);

/// @relates cpe_vendor
bool cpe_vendor_set_value(struct cpe_vendor *vendor, const char *new_value);

/// @relates cpe_product
bool cpe_product_set_value(struct cpe_product *product, const char *new_value);

/// @relates cpe_product
bool cpe_product_set_part(struct cpe_product *product, cpe_part_t new_part);

/// @relates cpe_version
bool cpe_version_set_value(struct cpe_version *version, const char *new_value);

/// @relates cpe_update
bool cpe_update_set_value(struct cpe_update *update, const char *new_value);

/// @relates cpe_edition
bool cpe_edition_set_value(struct cpe_edition *edition, const char *new_value);

/// @relates cpe_language
bool cpe_language_set_value(struct cpe_language *language, const char *new_value);


/*
 * Add functions
 */

/// @relates cpe_item
bool cpe_item_add_reference(struct cpe_item *item, struct cpe_reference *new_reference);

/// @relates cpe_item
bool cpe_item_add_check(struct cpe_item *item, struct cpe_check *new_check);

/// @relates cpe_item
bool cpe_item_add_title(struct cpe_item *item, struct oscap_title *new_title);

/// @relates cpe_item
bool cpe_item_add_note(struct cpe_item *item, struct oscap_title *new_title);

/// @relates cpe_dict_model
bool cpe_dict_model_add_vendor(struct cpe_dict_model *dict, struct cpe_vendor *new_vendor);

/// @relates cpe_vendor
bool cpe_vendor_add_title(struct cpe_vendor *vendor, struct oscap_title *new_title);

/// @relates cpe_vendor
bool cpe_vendor_add_product(struct cpe_vendor *vendor, struct cpe_product *new_product);

/// @relates cpe_product
bool cpe_product_add_version(struct cpe_product *product, struct cpe_version *new_version);

/// @relates cpe_version
bool cpe_version_add_update(struct cpe_version *version, struct cpe_update *new_update);

/// @relates cpe_update
bool cpe_update_add_edition(struct cpe_update *update, struct cpe_edition *new_edition);

/// @relates cpe_edition
bool cpe_edition_add_language(struct cpe_edition *edition, struct cpe_language *new_language);

/** @} */

#endif				/* _CPEDICT_H_ */
