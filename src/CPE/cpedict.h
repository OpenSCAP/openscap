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

#include <stdlib.h>

#include "cpeuri.h"
#include "../common/util.h"
#include "../common/oscap.h"


// forward declaration
struct oscap_title;

/**
 * @struct cpe_dict_check
 * Structure representing single CPE check.
 */
struct cpe_check;

/**
 * @struct cpe_dict_reference
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

struct cpe_item_metadata;
struct cpe_item_title;
struct cpe_generator;
struct cpe_vendor;
struct cpe_product;
struct cpe_version;
struct cpe_update;
struct cpe_edition;
struct cpe_language;

/** @struct cpe_item_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_item_iterator;
struct cpe_item* cpe_item_iterator_next(struct cpe_item_iterator* it);
bool cpe_item_iterator_has_more(struct cpe_item_iterator* it);
void cpe_item_iterator_free(struct cpe_item_iterator* it);

/** @struct cpe_dict_reference_iterator
 * Iterator over CPE dictionary references.
 * @see oscap_iterator
 */
struct cpe_reference_iterator;
struct cpe_reference* cpe_reference_iterator_next(struct cpe_reference_iterator* it);
bool cpe_reference_iterator_has_more(struct cpe_reference_iterator* it);
void cpe_reference_iterator_free(struct cpe_reference_iterator* it);

/** @struct cpe_check_iterator
 * Iterator over CPE dictionary checks.
 * @see oscap_iterator
 */
struct cpe_check_iterator;
struct cpe_check* cpe_check_iterator_next(struct cpe_check_iterator* it);
bool cpe_check_iterator_has_more(struct cpe_check_iterator* it);
void cpe_check_iterator_free(struct cpe_check_iterator* it);

/** @struct cpe_note_iterator
 * Iterator over CPE dictionary notes.
 * @see oscap_iterator
 */
struct cpe_note_iterator;
struct cpe_note* cpe_note_iterator_next(struct cpe_note_iterator* it);
bool cpe_note_iterator_has_more(struct cpe_note_iterator* it);
void cpe_note_iterator_free(struct cpe_note_iterator* it);

/** @struct cpe_vendor_iterator
 * Iterator over CPE dictionary item vendors.
 * @see oscap_iterator
 */
struct cpe_vendor_iterator;
struct cpe_vendor* cpe_vendor_iterator_next(struct cpe_vendor_iterator* it);
bool cpe_vendor_iterator_has_more(struct cpe_vendor_iterator* it);
void cpe_vendor_iterator_free(struct cpe_vendor_iterator* it);

/** @struct cpe_product_iterator
 * Iterator over CPE dictionary item products.
 * @see oscap_iterator
 */
struct cpe_product_iterator;
struct cpe_product* cpe_product_iterator_next(struct cpe_product_iterator* it);
bool cpe_product_iterator_has_more(struct cpe_product_iterator* it);
void cpe_product_iterator_free(struct cpe_product_iterator* it);

/** @struct cpe_version_iterator
 * Iterator over CPE dictionary item versions.
 * @see oscap_iterator
 */
struct cpe_version_iterator;
struct cpe_version* cpe_version_iterator_next(struct cpe_version_iterator* it);
bool cpe_version_iterator_has_more(struct cpe_version_iterator* it);
void cpe_version_iterator_free(struct cpe_version_iterator* it);

/** @struct cpe_update_iterator
 * Iterator over CPE dictionary item updates.
 * @see oscap_iterator
 */
struct cpe_update_iterator;
struct cpe_update* cpe_update_iterator_next(struct cpe_update_iterator* it);
bool cpe_update_iterator_has_more(struct cpe_update_iterator* it);
void cpe_update_iterator_free(struct cpe_update_iterator* it);

/** @struct cpe_edition_iterator
 * Iterator over CPE dictionary item editions.
 * @see oscap_iterator
 */
struct cpe_edition_iterator;
struct cpe_edition* cpe_edition_iterator_next(struct cpe_edition_iterator* it);
bool cpe_edition_iterator_has_more(struct cpe_edition_iterator* it);
void cpe_edition_iterator_free(struct cpe_edition_iterator* it);

/** @struct cpe_language_iterator
 * Iterator over CPE dictionary item languages.
 * @see oscap_iterator
 */
struct cpe_language_iterator;
struct cpe_language* cpe_language_iterator_next(struct cpe_language_iterator* it);
bool cpe_language_iterator_has_more(struct cpe_language_iterator* it);
void cpe_language_iterator_free(struct cpe_language_iterator* it);

/**
 * cpe_item_metadata functions
 */
const char * cpe_item_metadata_get_modification_date(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_status(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_nvd_id(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_deprecated_by_nvd_id(const struct cpe_item_metadata *item);

/**
 * cpe_check functions to get variable members
 */
const char * cpe_check_get_system(const struct cpe_check *item);
const char * cpe_check_get_href(const struct cpe_check *item);
const char * cpe_check_get_identifier(const struct cpe_check *item);

/**
 * cpe_reference functions to get variable members
 */
const char * cpe_reference_get_href(const struct cpe_reference *item);
const char * cpe_reference_get_content(const struct cpe_reference *item);

/**
 * oscap_title functions to get variable members
 */
const char * oscap_title_get_content(const struct oscap_title *item);

struct cpe_dict_model * cpe_dict_model_parse_xml(const struct oscap_import_source * source);

/**
 * cpe_item functions to get variable members
 */
struct cpe_name * cpe_item_get_name(const struct cpe_item *item);
struct cpe_name * cpe_item_get_deprecated(const struct cpe_item *item);
const char * cpe_item_get_deprecation_date(const struct cpe_item *item);
struct cpe_item_metadata * cpe_item_get_metadata(const struct cpe_item *item);
struct cpe_reference_iterator * cpe_item_get_references(const struct cpe_item* item);
struct cpe_check_iterator * cpe_item_get_checks(const struct cpe_item* item);
struct oscap_title_iterator * cpe_item_get_titles(const struct cpe_item* item);
struct oscap_title_iterator * cpe_item_get_notes(const struct cpe_item* item);

/**
 * cpe_generator functions to get variable members
 */
const char * cpe_generator_get_product_name(const struct cpe_generator *item);
const char * cpe_generator_get_product_version(const struct cpe_generator *item);
const char * cpe_generator_get_schema_version(const struct cpe_generator *item);
const char * cpe_generator_get_timestamp(const struct cpe_generator *item);

/**
 * cpe_dict_model functions to get variable members
 */
struct cpe_generator * cpe_dict_model_get_generator(const struct cpe_dict_model *item);
struct cpe_item_iterator * cpe_dict_model_get_items(const struct cpe_dict_model *item);
struct cpe_vendor_iterator * cpe_dict_model_get_vendors(const struct cpe_dict_model *item);

/**
 * cpe_vendor functions to get variable members
 */
const char * cpe_vendor_get_value(const struct cpe_vendor *item);
struct oscap_title_iterator * cpe_vendor_get_titles(const struct cpe_vendor *item);
struct cpe_product_iterator * cpe_vendor_get_products(const struct cpe_vendor *item);

/**
 * cpe_product functions to get variable members
 */
const char * cpe_product_get_value(const struct cpe_product *item);
int cpe_product_get_part(const struct cpe_product *item);
struct cpe_version_iterator * cpe_product_get_versions(const struct cpe_product *item);

/**
 * cpe_version functions to get variable members
 */
const char * cpe_version_get_value(const struct cpe_version *item);
struct cpe_update_iterator * cpe_version_get_updates(const struct cpe_version *items);

/**
 * cpe_update functions to get variable members
 */
const char * cpe_update_get_value(const struct cpe_update *item);
struct cpe_edition_iterator * cpe_update_get_editions(const struct cpe_update *items);

/**
 * cpe_edition functions to get variable members
 */
const char * cpe_edition_get_value(const struct cpe_edition *item);
struct cpe_language_iterator * cpe_edition_get_languages(const struct cpe_edition *items);

/**
 * cpe_language functions to get variable members
 */
const char * cpe_language_get_value(const struct cpe_language *item);

/**
 * Load new CPE onary from file
 * @relates cpe_dict
 * @param fname file name of dictionary to import
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict_model *cpe_dict_model_import(const struct oscap_import_source * source);

/**
 * Frees CPE dictionary and its contents
 * @relates cpe_dict
 * @param dict dictionary to be deleted
 */
void cpe_dict_model_free(struct cpe_dict_model * dict);

/**
 * Verify wether given CPE is known according to specified dictionary
 * @relates cpe_name
 * @relates cpe_dict
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict_model * dict);

/**
 * Verify if CPE given by string is known according to specified dictionary
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
void cpe_dict_free(struct cpe_dict_model * dict);
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

/// @relates cpe_dict
bool cpe_dict_add_vendor(struct cpe_dict_model *dict, struct cpe_vendor *new_vendor);

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
