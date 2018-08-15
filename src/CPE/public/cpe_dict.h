/**
 * @addtogroup CPE
 * @{
 * @addtogroup cpedict CPE Dictionary
 * @{
 *
 * @image html cpe_dictionary.png "Class diagram"
 * @file cpe_dict.h
 *
 * \brief Interface to Common Platform Enumeration (CPE) Dictionary.
 */

/*
 * Copyright 2009--2014 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 */

#ifndef CPEDICT_H_
#define CPEDICT_H_

#include "oscap.h"
#include "cpe_name.h"
#include "oscap_text.h"
#include "oscap_source.h"
#include "oscap_export.h"

/**
 * @struct cpe_dict_model
 * Structure representing a CPE dictionary.
 */
struct cpe_dict_model;

/**
 * @struct cpe_item
 * Structure representing single CPE dictionary item.
 */
struct cpe_item;

/**
 * @struct cpe_vendor
 * Structure with information about vendor
 */
struct cpe_vendor;

/**
 * @struct cpe_generator
 * Structure with information about document
 */
struct cpe_generator;

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
 * @struct cpe_item_metadata
 * Structure representing metadata of CPE item
 */
struct cpe_item_metadata;

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

/************************************************************/
/**
 * @name Getters
 * Return value is pointer to structure's member. Do not free unless you null the pointer in the structure. 
 * Use remove function otherwise.
 * @{
 * */

/** cpe_item_metadata function to get date
 * @memberof cpe_item_metadata
 * @param item metadata of CPE item
 */
OSCAP_API const char *cpe_item_metadata_get_modification_date(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get status
 * @memberof cpe_item_metadata
 * @param item metadata of CPE item
 */
OSCAP_API const char *cpe_item_metadata_get_status(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get nvd ID
 * @memberof cpe_item_metadata
 * @param item metadata of CPE item
 */
OSCAP_API const char *cpe_item_metadata_get_nvd_id(const struct cpe_item_metadata *item);

/** cpe_item_metadata function to get NVD ID of deprecated item
 * @memberof cpe_item_metadata
 * @param item metadata of CPE item
 */
OSCAP_API const char *cpe_item_metadata_get_deprecated_by_nvd_id(const struct cpe_item_metadata *item);

/** cpe_check functions to get system
 * @memberof cpe_check
 * @param item CPE check item
 */
OSCAP_API const char *cpe_check_get_system(const struct cpe_check *item);

/** cpe_check functions to get href
 * @memberof cpe_check
 * @param item CPE check item
 */
OSCAP_API const char *cpe_check_get_href(const struct cpe_check *item);

/** cpe_check functions to get identifier
 * @memberof cpe_check
 * @param item CPE check item
 */
OSCAP_API const char *cpe_check_get_identifier(const struct cpe_check *item);

/** cpe_reference functions to get href of reference
 * @memberof cpe_reference
 * @param item CPE reference item
 */
OSCAP_API const char *cpe_reference_get_href(const struct cpe_reference *item);

/** cpe_reference functions to get content of reference
 * @memberof cpe_reference
 * @param item CPE reference item
 */
OSCAP_API const char *cpe_reference_get_content(const struct cpe_reference *item);

/** cpe_item functions to get variable member name
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct cpe_name *cpe_item_get_name(const struct cpe_item *item);

/** cpe_item functions to get variable member deprecated_by
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct cpe_name *cpe_item_get_deprecated_by(const struct cpe_item *item);

/** cpe_item functions to get variable member date
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API const char *cpe_item_get_deprecation_date(const struct cpe_item *item);

/** cpe_item functions to get metadata of cpe_item
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct cpe_item_metadata *cpe_item_get_metadata(const struct cpe_item *item);

/** cpe_item functions to get CPE references
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct cpe_reference_iterator *cpe_item_get_references(const struct cpe_item *item);

/** cpe_item functions to get CPE checks
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct cpe_check_iterator *cpe_item_get_checks(const struct cpe_item *item);

/** cpe_item functions to get CPE titles
 * @memberof cpe_item
 * @param item CPE item
 */
OSCAP_API struct oscap_text_iterator *cpe_item_get_titles(const struct cpe_item *item);

/** cpe_generator functions to get product name
 * @memberof cpe_generator
 * @param item document generator
 */
OSCAP_API const char *cpe_generator_get_product_name(const struct cpe_generator *item);

/** cpe_generator functions to get product version
 * @memberof cpe_generator
 * @param item document generator
 */
OSCAP_API const char *cpe_generator_get_product_version(const struct cpe_generator *item);

/** cpe_generator functions to get document schema version
 * @memberof cpe_generator
 * @param item document generator
 */
OSCAP_API const char *cpe_generator_get_schema_version(const struct cpe_generator *item);

/** cpe_generator functions to get timestamp from generator
 * @memberof cpe_generator
 * @param item document generator
 */
OSCAP_API const char *cpe_generator_get_timestamp(const struct cpe_generator *item);

/** cpe_dict_model functions to get the base version from CPE dictionary model
 *
 * The base version is the major CPE version, 1 or 2. These two versions differ
 * in namespace when exporting so we have to remember which version we loaded.
 *
 * CPE 1.0 also doesn't have cpe_generator structure inside so we can't get
 * the version info from there.
 *
 * @memberof cpe_dict_model
 * @param item dictionary model
 */
OSCAP_API int cpe_dict_model_get_base_version(const struct cpe_dict_model *item);

/** cpe_dict_model functions to get the base version from CPE dictionary model
 * @memberof cpe_dict_model
 * @param item dictionary model
 */
OSCAP_API bool cpe_dict_model_set_base_version(struct cpe_dict_model *item, int base_version);

/** cpe_dict_model functions to get generator from CPE dictionary model
 * @memberof cpe_dict_model
 * @memberof cpe_generator
 * @param item dictionary model
 */
OSCAP_API struct cpe_generator *cpe_dict_model_get_generator(const struct cpe_dict_model *item);

/** cpe_dict_model functions to get CPE items
 * @memberof cpe_dict_model
 * @param item dictionary model
 */
OSCAP_API struct cpe_item_iterator *cpe_dict_model_get_items(const struct cpe_dict_model *item);

/** cpe_dict_model functions to get vendors
 * @memberof cpe_dict_model
 * @param item dictionary model
 */
OSCAP_API struct cpe_vendor_iterator *cpe_dict_model_get_vendors(const struct cpe_dict_model *item);

/** cpe_vendor functions to get vendor value
 * @memberof cpe_vendor
 * @param item cpe_vendor
 */
OSCAP_API const char *cpe_vendor_get_value(const struct cpe_vendor *item);

/** cpe_vendor functions to get vendor titles
 * @memberof cpe_vendor
 * @param item cpe_vendor
 */
OSCAP_API struct oscap_text_iterator *cpe_vendor_get_titles(const struct cpe_vendor *item);

/** cpe_vendor functions to get vendor products
 * @memberof cpe_vendor
 * @param item cpe_vendor
 */
OSCAP_API struct cpe_product_iterator *cpe_vendor_get_products(const struct cpe_vendor *item);

/** cpe_product functions to get product value
 * @memberof cpe_product
 * @param item cpe_product
 */
OSCAP_API const char *cpe_product_get_value(const struct cpe_product *item);

/** cpe_product functions to get product part
 * @memberof cpe_product
 * @param item cpe_product
 */
OSCAP_API cpe_part_t cpe_product_get_part(const struct cpe_product *item);

/** cpe_product functions to get versions of product
 * @memberof cpe_product
 * @memberof cpe_version
 * @param item cpe_product
 */
OSCAP_API struct cpe_version_iterator *cpe_product_get_versions(const struct cpe_product *item);

/** cpe_version functions to get vupdates of versions
 * @memberof cpe_version
 * @param item cpe_version
 */
OSCAP_API const char *cpe_version_get_value(const struct cpe_version *item);

/** cpe_version functions to get value of version
 * @memberof cpe_version
 * @memberof cpe_update
 * @param item cpe_version
 */
OSCAP_API struct cpe_update_iterator *cpe_version_get_updates(const struct cpe_version *item);

/** cpe_update functions to get updates of version
 * @memberof cpe_update
 * @param item
 */
OSCAP_API const char *cpe_update_get_value(const struct cpe_update *item);

/** cpe_update functions to get editions of update
 * @memberof cpe_update
 * @memberof cpe_edition
 * @param item cpe_update of product element
 */
OSCAP_API struct cpe_edition_iterator *cpe_update_get_editions(const struct cpe_update *item);

/** cpe_edition functions to get value of edition
 * @memberof cpe_edition
 * @param item cpe_edition of update
 */
OSCAP_API const char *cpe_edition_get_value(const struct cpe_edition *item);

/** cpe_edition functions to get languages of edition
 * @memberof cpe_edition
 * @memberof cpe_language
 * @param item cpe_edition of update
 */
OSCAP_API struct cpe_language_iterator *cpe_edition_get_languages(const struct cpe_edition *item);

/** cpe_language functions to get value of language
 * @memberof cpe_language
 * @param item language
 */
OSCAP_API const char *cpe_language_get_value(const struct cpe_language *item);

/************************************************************/
/** @} End of Getters group */

/// @memberof cpe_check
OSCAP_API void cpe_check_free(struct cpe_check *check);
/// @memberof cpe_reference
OSCAP_API void cpe_reference_free(struct cpe_reference *ref);
/// @memberof cpe_vendor
OSCAP_API void cpe_vendor_free(struct cpe_vendor *vendor);
/// @memberof cpe_product
OSCAP_API void cpe_product_free(struct cpe_product *product);
/// @memberof cpe_version
OSCAP_API void cpe_version_free(struct cpe_version *version);
/// @memberof cpe_update
OSCAP_API void cpe_update_free(struct cpe_update *update);
/// @memberof cpe_edition
OSCAP_API void cpe_edition_free(struct cpe_edition *edition);
/// @memberof cpe_language
OSCAP_API void cpe_language_free(struct cpe_language *language);
/// @memberof cpe_itemmetadata
OSCAP_API void cpe_itemmetadata_free(struct cpe_item_metadata *meta);
/// @memberof cpe_dict_model
OSCAP_API void cpe_dict_model_free(struct cpe_dict_model *dict);
/// @memberof cpe_generator
OSCAP_API void cpe_generator_free(struct cpe_generator *generator);
/// @memberof cpe_item
OSCAP_API void cpe_item_free(struct cpe_item *item);

/// @memberof cpe_dict_model
OSCAP_API struct cpe_dict_model *cpe_dict_model_new(void);
/// @memberof cpe_generator
OSCAP_API struct cpe_generator *cpe_generator_new(void);
/// @memberof cpe_check
OSCAP_API struct cpe_check *cpe_check_new(void);
/// @memberof cpe_reference
OSCAP_API struct cpe_reference *cpe_reference_new(void);
/// @memberof cpe_item
OSCAP_API struct cpe_item *cpe_item_new(void);
/// @memberof cpe_vendor
OSCAP_API struct cpe_vendor *cpe_vendor_new(void);
/// @memberof cpe_product
OSCAP_API struct cpe_product *cpe_product_new(void);
/// @memberof cpe_version
OSCAP_API struct cpe_version *cpe_version_new(void);
/// @memberof cpe_update
OSCAP_API struct cpe_update *cpe_update_new(void);
/// @memberof cpe_edition
OSCAP_API struct cpe_edition *cpe_edition_new(void);
/// @memberof cpe_language
OSCAP_API struct cpe_language *cpe_language_new(void);
/// @memberof cpe_item_metadata
OSCAP_API struct cpe_item_metadata *cpe_item_metadata_new(void);

/************************************************************/
/**
 * @name Setters
 * For lists use add functions. Parameters of set functions are duplicated in memory and need to 
 * be freed by caller.
 * @{
 */

/// @memberof cpe_item
OSCAP_API bool cpe_item_set_name(struct cpe_item *item, const struct cpe_name *new_name);

/// @memberof cpe_item
OSCAP_API bool cpe_item_set_deprecated_by(struct cpe_item *item, const struct cpe_name *new_deprecated_by);

/// @memberof cpe_item
OSCAP_API bool cpe_item_set_deprecation_date(struct cpe_item *item, const char *new_deprecation_date);

/// @memberof cpe_item_metadata
OSCAP_API bool cpe_item_metadata_set_modification_date(struct cpe_item_metadata *item_metadata,
					     const char *new_modification_date);

/// @memberof cpe_item_metadata
OSCAP_API bool cpe_item_metadata_set_status(struct cpe_item_metadata *item_metadata, const char *new_status);

/// @memberof cpe_item_metadata
OSCAP_API bool cpe_item_metadata_set_nvd_id(struct cpe_item_metadata *item_metadata, const char *new_nvd_id);

/// @memberof cpe_item_metadata
OSCAP_API bool cpe_item_metadata_set_deprecated_by_nvd_id(struct cpe_item_metadata *item_metadata,
						const char *new_deprecated_by_nvd_id);

/// @memberof cpe_check
OSCAP_API bool cpe_check_set_system(struct cpe_check *check, const char *new_system);

/// @memberof cpe_check
OSCAP_API bool cpe_check_set_href(struct cpe_check *check, const char *new_href);

/// @memberof cpe_check
OSCAP_API bool cpe_check_set_identifier(struct cpe_check *check, const char *new_identifier);

/// @memberof cpe_reference
OSCAP_API bool cpe_reference_set_href(struct cpe_reference *reference, const char *new_href);

/// @memberof cpe_reference
OSCAP_API bool cpe_reference_set_content(struct cpe_reference *reference, const char *new_content);

/// @memberof cpe_generator
OSCAP_API bool cpe_generator_set_product_name(struct cpe_generator *generator, const char *new_product_name);

/// @memberof cpe_generator
OSCAP_API bool cpe_generator_set_product_version(struct cpe_generator *generator, const char *new_product_version);

/// @memberof cpe_generator
OSCAP_API bool cpe_generator_set_schema_version(struct cpe_generator *generator, const char *new_schema_version);

/// @memberof cpe_generator
OSCAP_API bool cpe_generator_set_timestamp(struct cpe_generator *generator, const char *new_timestamp);

/// @memberof cpe_vendor
OSCAP_API bool cpe_vendor_set_value(struct cpe_vendor *vendor, const char *new_value);

/// @memberof cpe_product
OSCAP_API bool cpe_product_set_value(struct cpe_product *product, const char *new_value);

/// @memberof cpe_product
OSCAP_API bool cpe_product_set_part(struct cpe_product *product, cpe_part_t new_part);

/// @memberof cpe_version
OSCAP_API bool cpe_version_set_value(struct cpe_version *version, const char *new_value);

/// @memberof cpe_update
OSCAP_API bool cpe_update_set_value(struct cpe_update *update, const char *new_value);

/// @memberof cpe_edition
OSCAP_API bool cpe_edition_set_value(struct cpe_edition *edition, const char *new_value);

/// @memberof cpe_language
OSCAP_API bool cpe_language_set_value(struct cpe_language *language, const char *new_value);

/*
 * Add functions
 */

/// @memberof cpe_item
OSCAP_API bool cpe_item_add_reference(struct cpe_item *item, struct cpe_reference *new_reference);

/// @memberof cpe_item
OSCAP_API bool cpe_item_add_check(struct cpe_item *item, struct cpe_check *new_check);

/// @memberof cpe_item
OSCAP_API bool cpe_item_add_title(struct cpe_item *item, struct oscap_text *new_title);

/// @memberof cpe_dict_model
OSCAP_API bool cpe_dict_model_add_item(struct cpe_dict_model *dict, struct cpe_item *new_item);

/// @memberof cpe_dict_model
OSCAP_API bool cpe_dict_model_add_vendor(struct cpe_dict_model *dict, struct cpe_vendor *new_vendor);

/// @memberof cpe_vendor
OSCAP_API bool cpe_vendor_add_title(struct cpe_vendor *vendor, struct oscap_text *new_title);

/// @memberof cpe_vendor
OSCAP_API bool cpe_vendor_add_product(struct cpe_vendor *vendor, struct cpe_product *new_product);

/// @memberof cpe_product
OSCAP_API bool cpe_product_add_version(struct cpe_product *product, struct cpe_version *new_version);

/// @memberof cpe_version
OSCAP_API bool cpe_version_add_update(struct cpe_version *version, struct cpe_update *new_update);

/// @memberof cpe_update
OSCAP_API bool cpe_update_add_edition(struct cpe_update *update, struct cpe_edition *new_edition);

/// @memberof cpe_edition
OSCAP_API bool cpe_edition_add_language(struct cpe_edition *edition, struct cpe_language *new_language);


/************************************************************/
/** @} End of Setters group */

/************************************************************/
/**
 * @name Iterators
 * @{
 * */

/**
 * @struct cpe_item_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_item_iterator;

/**
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @memberof cpe_item_iterator
 */
OSCAP_API struct cpe_item *cpe_item_iterator_next(struct cpe_item_iterator *it);

/**
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @memberof cpe_item_iterator
 */
OSCAP_API bool cpe_item_iterator_has_more(struct cpe_item_iterator *it);

/**
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 * @memberof cpe_item_iterator
 */
OSCAP_API void cpe_item_iterator_free(struct cpe_item_iterator *it);

/// @memberof cpe_item
OSCAP_API void cpe_item_iterator_remove(struct cpe_item_iterator *it);
/// @memberof cpe_item_iterator
OSCAP_API void cpe_item_iterator_reset(struct cpe_item_iterator *it);

/**
 * @struct cpe_reference_iterator
 * Iterator over CPE dictionary references.
 * @see oscap_iterator
 */
struct cpe_reference_iterator;

/**
 * Iterator over CPE item reference items.
 * @see oscap_iterator
 * @memberof cpe_reference_iterator
 */
OSCAP_API struct cpe_reference *cpe_reference_iterator_next(struct cpe_reference_iterator *it);

/**
 * Iterator over CPE item reference items.
 * @see oscap_iterator
 * @memberof cpe_reference_iterator
 */
OSCAP_API bool cpe_reference_iterator_has_more(struct cpe_reference_iterator *it);

/**
 * Iterator over CPE item reference items.
 * @see oscap_iterator
 * @memberof cpe_reference_iterator
 */
OSCAP_API void cpe_reference_iterator_free(struct cpe_reference_iterator *it);

/// @memberof cpe_reference_iterator
OSCAP_API void cpe_reference_iterator_remove(struct cpe_reference_iterator *it);
/// @memberof cpe_reference_iterator
OSCAP_API void cpe_reference_iterator_reset(struct cpe_reference_iterator *it);

/**
 * @struct cpe_check_iterator
 * Iterator over CPE dictionary checks.
 * @see oscap_iterator
 */
struct cpe_check_iterator;

/**
 * Iterator over CPE item check items.
 * @see oscap_iterator
 * @memberof cpe_check_iterator
 */
OSCAP_API struct cpe_check *cpe_check_iterator_next(struct cpe_check_iterator *it);

/**
 * Iterator over CPE item check items.
 * @see oscap_iterator
 * @memberof cpe_check_iterator
 */
OSCAP_API bool cpe_check_iterator_has_more(struct cpe_check_iterator *it);

/**
 * Iterator over CPE item check items.
 * @see oscap_iterator
 * @memberof cpe_check_iterator
 */
OSCAP_API void cpe_check_iterator_free(struct cpe_check_iterator *it);

/// @memberof cpe_check_iterator
OSCAP_API void cpe_check_iterator_remove(struct cpe_check_iterator *it);
/// @memberof cpe_check_iterator
OSCAP_API void cpe_check_iterator_reset(struct cpe_check_iterator *it);

/**
 * @struct cpe_vendor_iterator
 * Iterator over CPE dictionary item vendors.
 * @see oscap_iterator
 */
struct cpe_vendor_iterator;

/**
 * Iterator over CPE vendor items.
 * @see oscap_iterator
 * @memberof cpe_vendor_iterator
 */
OSCAP_API struct cpe_vendor *cpe_vendor_iterator_next(struct cpe_vendor_iterator *it);

/**
 * Iterator over CPE vendor items.
 * @see oscap_iterator
 * @memberof cpe_vendor_iterator
 */
OSCAP_API bool cpe_vendor_iterator_has_more(struct cpe_vendor_iterator *it);

/**
 * Iterator over CPE vendor items.
 * @see oscap_iterator
 * @memberof cpe_vendor_iterator
 */
OSCAP_API void cpe_vendor_iterator_free(struct cpe_vendor_iterator *it);

/// @memberof cpe_vendor_iterator
OSCAP_API void cpe_vendor_iterator_remove(struct cpe_vendor_iterator *it);
/// @memberof cpe_vendor_iterator
OSCAP_API void cpe_vendor_iterator_reset(struct cpe_vendor_iterator *it);

/**
 * @struct cpe_product_iterator
 * Iterator over CPE dictionary item products.
 * @see oscap_iterator
 */
struct cpe_product_iterator;

/**
 * Iterator over CPE product items.
 * @see oscap_iterator
 * @memberof cpe_product_iterator
 */
OSCAP_API struct cpe_product *cpe_product_iterator_next(struct cpe_product_iterator *it);

/**
 * Iterator over CPE product items.
 * @see oscap_iterator
 * @memberof cpe_product_iterator
 */
OSCAP_API bool cpe_product_iterator_has_more(struct cpe_product_iterator *it);

/**
 * Iterator over CPE product items.
 * @see oscap_iterator
 * @memberof cpe_product_iterator
 */
OSCAP_API void cpe_product_iterator_free(struct cpe_product_iterator *it);

/// @memberof cpe_product_iterator
OSCAP_API void cpe_product_iterator_remove(struct cpe_product_iterator *it);
/// @memberof cpe_product_iterator
OSCAP_API void cpe_product_iterator_reset(struct cpe_product_iterator *it);

/**
 * @struct cpe_version_iterator
 * Iterator over CPE dictionary item versions.
 * @see oscap_iterator
 */
struct cpe_version_iterator;

/**
 * Iterator over CPE version items.
 * @see oscap_iterator
 * @memberof cpe_version_iterator
 */
OSCAP_API struct cpe_version *cpe_version_iterator_next(struct cpe_version_iterator *it);

/**
 * Iterator over CPE version items.
 * @see oscap_iterator
 * @memberof cpe_version_iterator
 */
OSCAP_API bool cpe_version_iterator_has_more(struct cpe_version_iterator *it);

/**
 * Iterator over CPE version items.
 * @see oscap_iterator
 * @memberof cpe_version_iterator
 */
OSCAP_API void cpe_version_iterator_free(struct cpe_version_iterator *it);

/// @memberof cpe_version_iterator
OSCAP_API void cpe_version_iterator_remove(struct cpe_version_iterator *it);
/// @memberof cpe_version_iterator
OSCAP_API void cpe_version_iterator_reset(struct cpe_version_iterator *it);

/**
 * @struct cpe_update_iterator
 * Iterator over CPE dictionary item updates.
 * @see oscap_iterator
 */
struct cpe_update_iterator;

/**
 * Iterator over CPE update items.
 * @see oscap_iterator
 * @memberof cpe_update_iterator
 */
OSCAP_API struct cpe_update *cpe_update_iterator_next(struct cpe_update_iterator *it);

/**
 * Iterator over CPE update items.
 * @see oscap_iterator
 * @memberof cpe_update_iterator
 */
OSCAP_API bool cpe_update_iterator_has_more(struct cpe_update_iterator *it);

/**
 * Iterator over CPE update items.
 * @see oscap_iterator
 * @memberof cpe_update_iterator
 */
OSCAP_API void cpe_update_iterator_free(struct cpe_update_iterator *it);

/// @memberof cpe_update_iterator
OSCAP_API void cpe_update_iterator_remove(struct cpe_update_iterator *it);
/// @memberof cpe_update_iterator
OSCAP_API void cpe_update_iterator_reset(struct cpe_update_iterator *it);

/**
 * @struct cpe_edition_iterator
 * Iterator over CPE dictionary item editions.
 * @see oscap_iterator
 */
struct cpe_edition_iterator;

/**
 * Iterator over CPE edition items.
 * @see oscap_iterator
 * @memberof cpe_edition_iterator
 */
OSCAP_API struct cpe_edition *cpe_edition_iterator_next(struct cpe_edition_iterator *it);

/**
 * Iterator over CPE edition items.
 * @see oscap_iterator
 * @memberof cpe_edition_iterator
 */
OSCAP_API bool cpe_edition_iterator_has_more(struct cpe_edition_iterator *it);

/**
 * Iterator over CPE edition items.
 * @see oscap_iterator
 * @memberof cpe_edition_iterator
 */
OSCAP_API void cpe_edition_iterator_free(struct cpe_edition_iterator *it);

/// @memberof cpe_edition_iterator
OSCAP_API void cpe_edition_iterator_remove(struct cpe_edition_iterator *it);
/// @memberof cpe_edition_iterator
OSCAP_API void cpe_edition_iterator_reset(struct cpe_edition_iterator *it);

/**
 * @struct cpe_language_iterator
 * Iterator over CPE dictionary item languages.
 * @see oscap_iterator
 */
struct cpe_language_iterator;

/**
 * Iterator over CPE language items.
 * @see oscap_iterator
 * @memberof cpe_language_iterator
 */
OSCAP_API struct cpe_language *cpe_language_iterator_next(struct cpe_language_iterator *it);

/**
 * Iterator over CPE language items.
 * @see oscap_iterator
 * @memberof cpe_language_iterator
 */
OSCAP_API bool cpe_language_iterator_has_more(struct cpe_language_iterator *it);

/**
 * Iterator over CPE language items.
 * @see oscap_iterator
 * @memberof cpe_language_iterator
 */
OSCAP_API void cpe_language_iterator_free(struct cpe_language_iterator *it);

/// @memberof cpe_language_iterator
OSCAP_API void cpe_language_iterator_remove(struct cpe_language_iterator *it);
/// @memberof cpe_language_iterator
OSCAP_API void cpe_language_iterator_reset(struct cpe_language_iterator *it);

/************************************************************/
/** @} End of Iterators group */

/************************************************************/
/**
 * @name Evaluators
 * @{
 * */

/**
 * Get the newest supported version of CPE dictionary XML
 * @return version of XML file format
 * @memberof cpe_dict_model
 */
OSCAP_API const char * cpe_dict_model_supported(void);

/** 
 * Verify wether given CPE is known according to specified dictionary
 * @memberof cpe_name
 * @memberof cpe_dict_model
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE
 */
OSCAP_API bool cpe_name_match_dict(struct cpe_name *cpe, struct cpe_dict_model *dict);

/**
 * Verify whether given CPE is applicable to current platform by evaluating checks associated with it
 *
 * @memberof cpe_name
 * @memberof cpe_dict_model
 * @param cpe CPE to verify
 * @param dict used CPE dictionary
 * @return true if dictionary contains given CPE and the CPE is applicable
 */
OSCAP_API bool cpe_name_applicable_dict(struct cpe_name *cpe, struct cpe_dict_model *dict, cpe_check_fn cb, void* usr);

/// @memberof cpe_item
OSCAP_API bool cpe_item_is_applicable(struct cpe_item* item, cpe_check_fn cb, void* usr);

/************************************************************/
/** @} End of Evaluators group */

/**
 * Write the dict_model to a file.
 * @param dict CPE Dict model
 * @memberof cpe_dict_model
 */
OSCAP_API void cpe_dict_model_export(const struct cpe_dict_model *dict, const char *file);

/**
 * Load new CPE dictionary from an oscap_source
 * @memberof cpe_dict_model
 * @param source The oscap_source to parse content from
 * @returns new dictionary or NULL
 */
OSCAP_API struct cpe_dict_model *cpe_dict_model_import_source(struct oscap_source *source);


/** @} */

/** @} */

#endif				/* _CPEDICT_H_ */
