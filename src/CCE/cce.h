/**
 * @addtogroup CCE
 * @{
 *
 * @file cce.h
 * \brief Interface to Common Configuration Enumeration (CCE).
 *
 * See more details at http://cce.mitre.org/
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
 *      Riley C. Porter <Riley.Porter@g2-inc.com>
 */

#include <stdbool.h>
#include "../common/oscap.h"

#ifndef _CCE_H
#define _CCE_H

/** @struct cce
 * Structure holding CCE entries.
 */
struct cce;

/** @struct cce_entry
 * Structure holding single CCE entry data.
 */
struct cce_entry;

/** @struct cce_reference
 * Structure holding a CCE reference.
 */
struct cce_reference;


/** @struct cce_reference_iterator
 * Iterator over CCE references.
 * @see oscap_iterator
 */
struct cce_reference_iterator;
/// @relates cce_reference_iterator
struct cce_reference* cce_reference_iterator_next(struct cce_reference_iterator* it);
/// @relates cce_reference_iterator
bool cce_reference_iterator_has_more(struct cce_reference_iterator* it);
/// @relates cce_reference_iterator
void cce_reference_iterator_free(struct cce_reference_iterator* it);

/** @struct cce_entry_iterator
 * Iterator over CCE entries.
 * @see oscap_iterator
 */
struct cce_entry_iterator;
/// @relates cce_entry_iterator
struct cce_entry* cce_entry_iterator_next(struct cce_entry_iterator* it);
/// @relates cce_entry_iterator
bool cce_entry_iterator_has_more(struct cce_entry_iterator* it);
/// @relates cce_entry_iterator
void cce_entry_iterator_free(struct cce_entry_iterator* it);


/**
 * Create a new CCE structure from XML file.
 * @relates cce
 * @param fname XML file name to porcess
 * @retval NULL on failure
 */
struct cce* cce_new(const char* fname);

/**
 * CCE structure destructor.
 * Deinitializes CCE structure and releases used resources.
 * @relates cce
 * @param cce pointer to target structure
 */
void cce_free(struct cce *cce);

/**
 * Get an iterator to the contents of the CCE.
 * @relates cce
 */
struct cce_entry_iterator* cce_get_entries(const struct cce* cce);

/**
 * Get an CCE entry by ID.
 * @relates cce
 * @retval NULL if given entry does not exist
 */
struct cce_entry* cce_get_entry(const struct cce* cce, const char* id);

/**
 * Vlaidate CCE XML file.
 *
 * @param filename file to be validated
 * @return result of validation (true / false)
 */
bool cce_validate(const char *filename);


/**
 * Get CCE entry ID.
 * @relates cce_entry
 */
const char* cce_entry_get_id(const struct cce_entry* cce);

/**
 * Get CCE entry desription.
 * @relates cce_entry
 */
const char* cce_entry_get_description(const struct cce_entry* cce);

/**
 * Get an iterator to CCE entry's parameters.
 * @relates cce_entry
 */
struct oscap_string_iterator* cce_entry_get_params(const struct cce_entry* cce);

/**
 * Get an iterator to CCE entry's technical mechanisms.
 * @relates cce_entry
 */
struct oscap_string_iterator* cce_entry_get_tech_mechs(const struct cce_entry* cce);

/**
 * Get an iterator to CCE entry's references.
 * @relates cce_entry
 */
struct cce_reference_iterator* cce_entry_get_references(const struct cce_entry* cce);


/**
 * Get source of CCE reference.
 * @relates cce_reference
 */
const char* cce_reference_get_source(const struct cce_reference* ref);

/**
 * Get contents of CCE reference.
 * @relates cce_reference
 */
const char* cce_reference_get_value(const struct cce_reference* ref);

#endif
