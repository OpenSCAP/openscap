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
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#ifndef _CPEDICT_H_
#define _CPEDICT_H_

#include <stdlib.h>

#include "cpeuri.h"

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


struct oscap_string_iterator;
const char* oscap_string_iterator_next(struct oscap_string_iterator* it);
bool oscap_string_iterator_has_more(struct oscap_string_iterator* it);


/** @struct cpe_dictitem_iterator
 * Iterator over CPE dictionary items.
 * @see oscap_iterator
 */
struct cpe_dictitem_iterator;
/// @relates cpe_dictitem_iterator
struct cpe_dictitem* cpe_dictitem_iterator_next(struct cpe_dictitem_iterator* it);
/// @relates cpe_dictitem_iterator
bool cpe_dictitem_iterator_has_more(struct cpe_dictitem_iterator* it);

/** @struct cpe_dict_reference_iterator
 * Iterator over CPE dictionary references.
 * @see oscap_iterator
 */
struct cpe_dict_reference_iterator;
/// @relates cpe_dict_reference_iterator
struct cpe_dict_reference* cpe_dict_reference_iterator_next(struct cpe_dict_reference_iterator* it);
/// @relates cpe_dict_reference_iterator
bool cpe_dict_reference_iterator_has_more(struct cpe_dict_reference_iterator* it);

/** @struct cpe_dict_check_iterator
 * Iterator over CPE dictionary checks.
 * @see oscap_iterator
 */
struct cpe_dict_check_iterator;
/// @relates cpe_dict_check_iterator
struct cpe_dict_check* cpe_dict_check_iterator_next(struct cpe_dict_check_iterator* it);
/// @relates cpe_dict_check_iterator
bool cpe_dict_check_iterator_has_more(struct cpe_dict_check_iterator* it);


/**
 * Load new CPE dictionary from file
 * @relates cpe_dict
 * @param fname file name of dictionary to load
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict *cpe_dict_new(const char *fname);

/**
 * Create new empty CPE dictionary
 * @relates cpe_dict
 * @return new dictionary
 * @retval NULL on failure
 */
struct cpe_dict *cpe_dict_new_empty(void);

/**
 * Add @a item to dictionary @a dict
 *
 * @relates cpe_dict
 * @note The item will be deleted as soon as you call cpe_dict_delete on the dictionary.
 * @param dict dictionary to add new item to
 * @param item pointer to item to add
 * @return true on success
 */
bool cpe_dict_add_item(struct cpe_dict * dict, struct cpe_dictitem * item);

/**
 * Frees CPE dictionary and its contents
 * @relates cpe_dict
 * @param dict dictionary to be deleted
 */
void cpe_dict_delete(struct cpe_dict * dict);

/**
 * Get an iterator to the dictionary contents.
 * @relates cpe_dict
 */
struct cpe_dictitem_iterator* cpe_dict_items(const struct cpe_dict* item);

/**
 * Get name of a generator of the dictionary.
 * @relates cpe_dict
 */
const char* cpe_dict_generator_product_name(const struct cpe_dict* item);

/**
 * Get version of a generator of the dictionary.
 * @relates cpe_dict
 */
const char* cpe_dict_generator_product_version(const struct cpe_dict* item);

/**
 * Get CPE dictionary schema version used to create this dictionary.
 * @relates cpe_dict
 */
const char* cpe_dict_generator_schema_version(const struct cpe_dict* item);

/**
 * Get timestamp of the dictionary creation.
 * @relates cpe_dict
 */
const char* cpe_dict_generator_timestamp(const struct cpe_dict* item);

/**
 * Get name of a checking system of the CPE check.
 * @relates cpe_dict_check
 */
const char* cpe_dict_check_system(const struct cpe_dict_check* item);

/**
 * Get URL of the CPE check.
 * @relates cpe_dict_check
 */
const char* cpe_dict_check_href(const struct cpe_dict_check* item);

/**
 * Get identifier of the CPE check.
 * @relates cpe_dict_check
 */
const char* cpe_dict_check_identifier(const struct cpe_dict_check* item);


/**
 * Get URL of the CPE dictionary reference.
 * @relates cpe_dict_reference
 */
const char* cpe_dict_reference_href(const struct cpe_dict_reference* item);

/**
 * Get content of the CPE dictionary reference.
 * @relates cpe_dict_reference
 */
const char* cpe_dict_reference_content(const struct cpe_dict_reference* item);


/**
 * Get CPE name of the dictionary entry.
 * @relates cpe_dictitem
 */
struct cpe_name* cpe_dictitem_name(const struct cpe_dictitem* item);

/**
 * Get title of the dictionary entry.
 * @relates cpe_dictitem
 */
const char* cpe_dictitem_title(const struct cpe_dictitem* item);

/**
 * Get an iterator to the dictionary entry's references.
 * @relates cpe_dictitem
 */
struct cpe_dict_reference_iterator* cpe_dictitem_references(const struct cpe_dictitem* item);

/**
 * Get an iterator to the dictionary entry's checks.
 * @relates cpe_dictitem
 */
struct cpe_dict_check_iterator* cpe_dictitem_checks(const struct cpe_dictitem* item);

/**
 * Get an iterator to the dictionary entry's notes.
 * @relates cpe_dictitem
 */
struct oscap_string_iterator* cpe_dictitem_notes(const struct cpe_dictitem* item);

/**
 * Get CPE name of item that depracated this one.
 * @retval NULL if the item has not been depracated
 * @relates cpe_dictitem
 */
struct cpe_name* cpe_dictitem_depracated(const struct cpe_dictitem* item);

/**
 * Get date this item was depracated.
 * @relates cpe_dictitem
 */
const char* cpe_dictitem_depracation_date(const struct cpe_dictitem* item);

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
