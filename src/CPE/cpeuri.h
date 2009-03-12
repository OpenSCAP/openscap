/*! \file cpeuri.h
 *  \brief Interface to Common Product Enumeration (CPE) URI
 *  
 *   See more details at http://nvd.nist.gov/cpe.cfm
 *  
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

#ifndef _CPEURI_H_
#define _CPEURI_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/// enumeration of possible CPE parts
enum CpePart_t {
	CPE_PART_NONE,  ///< no part specified -- error condition
	CPE_PART_HW,    ///< hardware
	CPE_PART_OS,    ///< operating system
	CPE_PART_APP    ///< application
};

/// string representation of CPE parts, order corresponds with values in enum above
const char*  CPE_PART_CHAR[4];
/// CPE URI schema string
const char*  CPE_SCHEMA;
/// CPE URI component separator character
const char   CPE_SEP_CHAR;
/// CPE URI component separator character as string
const char*  CPE_SEP_STR;

/// enumeration of CPE URI fields (useful for indexing arrays)
enum CpeField_t {
	CPE_FIELD_TYPE,
	CPE_FIELD_VENDOR,
	CPE_FIELD_PRODUCT,
	CPE_FIELD_VERSION,
	CPE_FIELD_UPDATE,
	CPE_FIELD_EDITION,
	CPE_FIELD_LANGUAGE,
	CPE_FIELDNUM,
};

/**
 * Structure holding Common Platform Enumeration data.
 *
 * Empty components are set to NULL.
 */
typedef struct {
	char* data_;            ///< parsed string, internal use only
	char** fields_;         ///< NULL-terminated array of pointers to individual components of CPE URI, internal
	enum CpePart_t part;    ///< part
	const char* vendor;     ///< vendor
	const char* product;    ///< product
	const char* version;    ///< version
	const char* update;     ///< update
	const char* edition;    ///< edition
	const char* language;   ///< language
} Cpe_t;

/**
 * Create new CPE structure from string @a cpe.
 *
 * @note If @a cpe is NULL, empty cpe will be created.
 * @param cpe CPE URI string to be parsed
 * @return new structure holding parsed data
 * @retval NULL on failure
 */
Cpe_t* cpe_new(const char* cpe);

/**
 * Split CPE string into individual fields separated by @a delim.
 *
 * This function returns NULL-terminated array of pointers to strings.
 * @note Return value must be freed explicitly by caller.
 * @note This function modifies its first argument.
 * @param str string to be parsed, will be changed as side-effect of this function
 * @param delim delimiter
 * @return newly allocated NULL-terminated array of ponters to strings representing individual parts
 * @retval NULL on failure
 */
char** cpe_split(char* str, const char* delim);

/**
 * In-place decodes a %-encoded string.
 * @param str string to be decoded (will be modified)
 * @return true on success
 */
bool cpe_urldecode(char* str);

/**
 * Check if candidate CPE @a cpe matches CPE @a against
 * according to CPE specification v 2.1.
 */
bool cpe_name_match_one(const Cpe_t* cpe, const Cpe_t* against);

/**
 * Check if CPE @a name matches any CPE in @a namelist.
 * @param name name to be looked-up
 * @param n number of items in namelist
 * @param namelist list of names to search in
 * @return true if @a name was found within @a namelist
 */
bool cpe_name_match_cpes(const Cpe_t* name, size_t n, Cpe_t** namelist);

/**
 * Match CPE URI @a candidate against list of @a n CPE URIs given by @a targets.
 * @param candidate candidarte CPE URI as string
 * @param n number of items in targets
 * @param targets list of CPE URIs to be candidate matched against
 * @todo maybe it should go into libcpe library itself
 * @return index of first URI in targets, that matched
 * @retval -1 on mismatch
 * @retval -2 invalid CPE URI was given as parameter
 */
int cpe_name_match_strs(const char* candidate, size_t n, char** targets);

/**
 * Ensures @a str is in proper CPE format.
 * @param str string to be validated
 */
bool cpe_check(const char* str);

/**
 * Return CPE URI as a new string.
 * @note Returned string is newly allocated and is caller's responsibility to free it.
 * @param cpe CPE to be converted
 * @return CPE URI as string
 * @retval NULL on failure
 */
char* cpe_get_uri(const Cpe_t* cpe);

/**
 * Write CPE URI @a cpe to file a descriptor @a f
 * @param cpe cpe to write
 * @param f file descriptor to write CPE URI to
 * @return number of written characters
 * @retval <0 on failure
 */
int cpe_write(const Cpe_t* cpe, FILE* f);

/**
 * Fill @a cpe structure with parsed @a fields.
 *
 * Fields can be obtained via cpe_split().
 * Pointers in target sructure will point to same strings as pointers in @a fields do.
 * No string duplication is performed.
 *
 * @see cpe_split
 * @param cpe structure to be filled
 * @param fields NULL-terminated array of strings representing individual fields
 * @return true on success
 */
bool cpe_assign_values(Cpe_t* cpe, char** fields);

/**
 * Destructor. Frees any used resources and safely destroys @a cpe.
 * @param cpe CPE to be deleted
 */
void cpe_delete(Cpe_t* cpe);

/**
 * Return number of elements in NULL-terminated array of pointers.
 * @param array of pointers
 * @return number of members
 */
size_t ptrarray_length(void** arr);


#endif /* _CPEURI_H_ */
