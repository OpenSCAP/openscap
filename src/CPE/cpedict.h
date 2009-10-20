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
#include "../common/oscap.h"


// forward declaration
struct oscap_title;

/**
 * @struct cpe_dict_check
 * Structure representing single CPE check.
 */
struct cpe_dict_check;

/**
 * @struct cpe_dict_reference
 * CPE dictionary item reference.
 */
struct cpe_dict_reference;

/**
 * @struct cpe_item
 * Structure representing single CPE dictionary item.
 */
struct cpe_item;

/**
 * @struct cpe_dict
 * Structure representing a CPE dictionary.
 */
struct cpe_dict;

struct cpe_item_metadata;
struct cpe_item_title;
struct cpe_generator;
struct cpe_dict_vendor;
struct cpe_dict_product;
struct cpe_dict_version;
struct cpe_dict_update;
struct cpe_dict_edition;
struct cpe_dict_language;

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
 * cpe_dict functions to get variable members
 */
struct cpe_generator * cpe_dict_get_generator(const struct cpe_dict *item);
struct cpe_item_iterator * cpe_dict_get_items(const struct cpe_dict *item);
struct cpe_dict_vendor * cpe_dict_get_vendors(const struct cpe_dict *item);

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
struct cpe_dict *cpe_dict_import(const char *fname);

/**
 * Frees CPE dictionary and its contents
 * @relates cpe_dict
 * @param dict dictionary to be deleted
 */
void cpe_dict_free(struct cpe_dict * dict);

/**
 * Verify wether given CPE is known according to specified dictionary
 * @relates cpe_name
 * @relates cpe_dict
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
bool cpe_name_match_dict(struct cpe_name * cpe, struct cpe_dict * dict);

/**
 * Verify if CPE given by string is known according to specified dictionary
 * @relates cpe_name
 * @relates cpe_dict
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
bool cpe_name_match_dict_str(const char *cpe, struct cpe_dict * dict);

/** Free functions
 * */
//void oscap_title_free(struct oscap_title * title);
void cpe_check_free(struct cpe_dict_check * check);
void cpe_reference_free(struct cpe_dict_reference * ref);
void cpe_vendor_free(struct cpe_dict_vendor * vendor);
void cpe_product_free(struct cpe_dict_product * product);
void cpe_version_free(struct cpe_dict_version * version);
void cpe_update_free(struct cpe_dict_update * update);
void cpe_edition_free(struct cpe_dict_edition * edition);
void cpe_language_free(struct cpe_dict_language * language);
void cpe_itemmetadata_free(struct cpe_item_metadata * meta);
void cpe_dict_free(struct cpe_dict * dict);
void cpe_generator_free(struct cpe_generator * generator);
void cpe_item_free(struct cpe_item * item);

/**
 * New functions
 * */
struct cpe_dict * cpe_dict_new();
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

#endif				/* _CPEDICT_H_ */

/** @} */
