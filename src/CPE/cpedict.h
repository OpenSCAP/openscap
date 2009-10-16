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

#ifndef _CPEDICT_H_
#define _CPEDICT_H_

#include <stdlib.h>

#include "cpeuri.h"
#include "cpedict_priv.h"
#include "../common/oscap.h"

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
 * @struct cpe_dictitem
 * Structure representing single CPE dictionary item.
 */
struct cpe_dictitem;

/**
 * @struct cpe_dict
 * Structure representing a CPE dictionary.
 */
struct cpe_dict;

struct cpe_item_metadata;
struct cpe_dictitem_title;
struct cpe_generator;
struct cpe_dict_vendor;
struct cpe_dict_product;
struct cpe_dict_version;
struct cpe_dict_update;
struct cpe_dict_edition;
struct cpe_dict_language;

/** @struct cpe_dictitem_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_dictitem_iterator;
struct cpe_dictitem* cpe_dictitem_iterator_next(struct cpe_dictitem_iterator* it);
bool cpe_dictitem_iterator_has_more(struct cpe_dictitem_iterator* it);
void cpe_dictitem_iterator_free(struct cpe_dictitem_iterator* it);

/** @struct cpe_dict_reference_iterator
 * Iterator over CPE dictionary references.
 * @see oscap_iterator
 */
struct cpe_dict_reference_iterator;
struct cpe_dict_reference* cpe_dict_reference_iterator_next(struct cpe_dict_reference_iterator* it);
bool cpe_dict_reference_iterator_has_more(struct cpe_dict_reference_iterator* it);
void cpe_dict_reference_iterator_free(struct cpe_dict_reference_iterator* it);

/** @struct cpe_dict_check_iterator
 * Iterator over CPE dictionary checks.
 * @see oscap_iterator
 */
struct cpe_dict_check_iterator;
struct cpe_dict_check* cpe_dict_check_iterator_next(struct cpe_dict_check_iterator* it);
bool cpe_dict_check_iterator_has_more(struct cpe_dict_check_iterator* it);
void cpe_dict_check_iterator_free(struct cpe_dict_check_iterator* it);

/** @struct cpe_dict_note_iterator
 * Iterator over CPE dictionary notes.
 * @see oscap_iterator
 */
struct cpe_dict_note_iterator;
struct cpe_dict_note* cpe_dict_note_iterator_next(struct cpe_dict_note_iterator* it);
bool cpe_dict_note_iterator_has_more(struct cpe_dict_note_iterator* it);
void cpe_dict_note_iterator_free(struct cpe_dict_note_iterator* it);

/** @struct cpe_dictitem_title_iterator
 * Iterator over CPE dictionary item titles.
 * @see oscap_iterator
 */
struct cpe_dictitem_title_iterator;
struct cpe_dictitem_title* cpe_dictitem_title_iterator_next(struct cpe_dictitem_title_iterator* it);
bool cpe_dictitem_title_iterator_has_more(struct cpe_dictitem_title_iterator* it);
void cpe_dictitem_title_iterator_free(struct cpe_dictitem_title_iterator* it);

/** @struct cpe_dict_vendor_iterator
 * Iterator over CPE dictionary item vendors.
 * @see oscap_iterator
 */
struct cpe_dict_vendor_iterator;
struct cpe_dict_vendor* cpe_dict_vendor_iterator_next(struct cpe_dict_vendor_iterator* it);
bool cpe_dict_vendor_iterator_has_more(struct cpe_dict_vendor_iterator* it);
void cpe_dict_vendor_iterator_free(struct cpe_dict_vendor_iterator* it);

/** @struct cpe_dict_product_iterator
 * Iterator over CPE dictionary item products.
 * @see oscap_iterator
 */
struct cpe_dict_product_iterator;
struct cpe_dict_product* cpe_dict_product_iterator_next(struct cpe_dict_product_iterator* it);
bool cpe_dict_product_iterator_has_more(struct cpe_dict_product_iterator* it);
void cpe_dict_product_iterator_free(struct cpe_dict_product_iterator* it);

/** @struct cpe_dict_version_iterator
 * Iterator over CPE dictionary item versions.
 * @see oscap_iterator
 */
struct cpe_dict_version_iterator;
struct cpe_dict_version* cpe_dict_version_iterator_next(struct cpe_dict_version_iterator* it);
bool cpe_dict_version_iterator_has_more(struct cpe_dict_version_iterator* it);
void cpe_dict_version_iterator_free(struct cpe_dict_version_iterator* it);

/** @struct cpe_dict_update_iterator
 * Iterator over CPE dictionary item updates.
 * @see oscap_iterator
 */
struct cpe_dict_update_iterator;
struct cpe_dict_update* cpe_dict_update_iterator_next(struct cpe_dict_update_iterator* it);
bool cpe_dict_update_iterator_has_more(struct cpe_dict_update_iterator* it);
void cpe_dict_update_iterator_free(struct cpe_dict_update_iterator* it);

/** @struct cpe_dict_edition_iterator
 * Iterator over CPE dictionary item editions.
 * @see oscap_iterator
 */
struct cpe_dict_edition_iterator;
struct cpe_dict_edition* cpe_dict_edition_iterator_next(struct cpe_dict_edition_iterator* it);
bool cpe_dict_edition_iterator_has_more(struct cpe_dict_edition_iterator* it);
void cpe_dict_edition_iterator_free(struct cpe_dict_edition_iterator* it);

/** @struct cpe_dict_language_iterator
 * Iterator over CPE dictionary item languages.
 * @see oscap_iterator
 */
struct cpe_dict_language_iterator;
struct cpe_dict_language* cpe_dict_language_iterator_next(struct cpe_dict_language_iterator* it);
bool cpe_dict_language_iterator_has_more(struct cpe_dict_language_iterator* it);
void cpe_dict_language_iterator_free(struct cpe_dict_language_iterator* it);

/**
 * cpe_item_metadata functions
 */
const char * cpe_item_metadata_get_modification_date(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_status(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_nvd_id(const struct cpe_item_metadata *item);
const char * cpe_item_metadata_get_deprecated_by_nvd_id(const struct cpe_item_metadata *item);

/**
 * cpe_dict_check functions to get variable members
 */
const char * cpe_dict_check_get_system(const struct cpe_dict_check *item);
const char * cpe_dict_check_get_href(const struct cpe_dict_check *item);
const char * cpe_dict_check_get_identifier(const struct cpe_dict_check *item);

/**
 * cpe_dict_reference functions to get variable members
 */
const char * cpe_dict_reference_get_href(const struct cpe_dict_reference *item);
const char * cpe_dict_reference_get_content(const struct cpe_dict_reference *item);

/**
 * cpe_dictitem_title functions to get variable members
 */
const char * cpe_dictitem_title_get_content(const struct cpe_dictitem_title *item);

/**
 * cpe_dictitem functions to get variable members
 */
struct cpe_name * cpe_dictitem_get_name(const struct cpe_dictitem *item);
struct cpe_name * cpe_dictitem_get_deprecated(const struct cpe_dictitem *item);
const char * cpe_dictitem_get_deprecation_date(const struct cpe_dictitem *item);
struct cpe_item_metadata * cpe_dictitem_get_metadata(const struct cpe_dictitem *item);
struct cpe_dict_reference_iterator * cpe_dictitem_get_references(const struct cpe_dictitem* item);
struct cpe_dict_check_iterator * cpe_dictitem_get_checks(const struct cpe_dictitem* item);
struct cpe_dictitem_title_iterator * cpe_dictitem_get_titles(const struct cpe_dictitem* item);
struct cpe_dictitem_title_iterator * cpe_dictitem_get_notes(const struct cpe_dictitem* item);

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
struct cpe_dictitem_iterator * cpe_dict_get_items(const struct cpe_dict *item);
struct cpe_dict_vendor * cpe_dict_get_vendors(const struct cpe_dict *item);

/**
 * cpe_dict_vendor functions to get variable members
 */
const char * cpe_dict_vendor_get_value(const struct cpe_dict_vendor *item);
struct cpe_dictitem_title_iterator * cpe_dict_vendor_get_titles(const struct cpe_dict_vendor *item);
struct cpe_dict_product_iterator * cpe_dict_vendor_get_products(const struct cpe_dict_vendor *item);

/**
 * cpe_dict_product functions to get variable members
 */
const char * cpe_dict_product_get_value(const struct cpe_dict_product *item);
int cpe_dict_product_get_part(const struct cpe_dict_product *item);
struct cpe_dict_version_iterator * cpe_dict_product_get_versions(const struct cpe_dict_product *item);

/**
 * cpe_dict_version functions to get variable members
 */
const char * cpe_dict_version_get_value(const struct cpe_dict_version *item);
struct cpe_dict_update_iterator * cpe_dict_version_get_updates(const struct cpe_dict_version *items);

/**
 * cpe_dict_update functions to get variable members
 */
const char * cpe_dict_update_get_value(const struct cpe_dict_update *item);
struct cpe_dict_edition_iterator * cpe_dict_update_get_editions(const struct cpe_dict_update *items);

/**
 * cpe_dict_edition functions to get variable members
 */
const char * cpe_dict_edition_get_value(const struct cpe_dict_edition *item);
struct cpe_dict_language_iterator * cpe_dict_edition_get_languages(const struct cpe_dict_edition *items);

/**
 * cpe_dict_language functions to get variable members
 */
const char * cpe_dict_language_get_value(const struct cpe_dict_language *item);

/**
 * Load new CPE dictionary from file
 * @relates cpe_dict
 * @param fname file name of dictionary to load
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict *cpe_dict_load(const char *fname);

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


#endif				/* _CPEDICT_H_ */

/** @} */
