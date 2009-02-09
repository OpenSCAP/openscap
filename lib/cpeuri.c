/*! \file cpeuri.c
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
 *      Brandon Dixon  <brandon.dixon@g2-inc.com>
 *      Lukas Kuklinek <lkuklinek@redhat.com>
 */

#define _BSD_SOURCE

#include <string.h>
#include <stdio.h>
#include <pcre.h>
#include <ctype.h>
#include <assert.h>

#include "libcpeuri.h"


const char* CPE_PART_CHAR[] = { NULL, "h", "o", "a" };
const char* CPE_SCHEMA   = "cpe:/";
const char  CPE_SEP_CHAR = ':';
const char* CPE_SEP_STR  = ":";


Cpe_t* cpe_new(const char* cpestr)
{
	int i;
	Cpe_t* cpe;

	if (cpestr && !cpe_check(cpestr))
		return NULL;

	cpe = malloc(sizeof(Cpe_t));
	if (cpe == NULL)
		return NULL;
	memset(cpe, 0, sizeof(Cpe_t)); // zero memory
	
	if (cpestr) {
		cpe->data_ = strdup(cpestr + 5); // without 'cpe:/'
		cpe->fields_ = cpe_split(cpe->data_, ":");
		for (i = 0; cpe->fields_[i]; ++i)
			cpe_urldecode(cpe->fields_[i]);
		cpe_assign_values(cpe, cpe->fields_);
	}
	return cpe;
}

const size_t CPE_SPLIT_INIT_ALLOC = 8;

char** cpe_split(char* str, const char* delim)
{
	assert(str != NULL);

	char** stringp = &str;
	int alloc = CPE_SPLIT_INIT_ALLOC;
	char** fields = malloc(alloc * sizeof(char*));
	if (!fields) return NULL;

	int i = 0;
	while (*stringp) {
		if (i + 2 > alloc) {
			void* old = fields;
			alloc *= 2;
			fields = realloc(fields, alloc * sizeof(char*));
			if (fields == NULL) {
				free(old);
				return NULL;
			}
		}
		fields[i++] = strsep(stringp, delim);
	}
	fields[i] = NULL;

	return fields;
}

bool cpe_urldecode(char* str)
{
	assert(str != NULL);
	char *inptr, *outptr;

	for (inptr = outptr = str; *inptr; ++inptr) {
		if (*inptr == '%') {
			if (isxdigit(inptr[1]) && isxdigit(inptr[2])) {
				char hex[3] = { inptr[1], inptr[2], '\0' };
				unsigned out;
				sscanf(hex, "%x", &out);
				// if (out == 0) return false; // this is strange
				*outptr++ = out;
				inptr += 2;
			}
			else {
				*outptr = '\0';
				return false;
			}
		}
		else *outptr++ = *inptr;
	}
	*outptr = '\0';
	return true;
}

bool cpe_name_match_one(const Cpe_t* cpe, const Cpe_t* against)
{
	int i;

	if (cpe == NULL || against == NULL) return false;

	if (ptrarray_length((void**)against->fields_) < ptrarray_length((void**)cpe->fields_))
		return false;

	for (i = 0; cpe->fields_[i]; ++i)
		if (strcmp(cpe->fields_[i], "") != 0 &&
			strcasecmp(cpe->fields_[i], against->fields_[i]) != 0)
				return false;
	return true;
}

bool cpe_name_match_cpes(const Cpe_t* name, size_t n, Cpe_t** namelist)
{
	int i;

	if (name == NULL || namelist == NULL) return false;

	for (i = 0; i < n; ++i)
		if (cpe_name_match_one(name, namelist[i]))
			return true;
	return false;
}

int cpe_name_match_strs(const char* candidate, size_t n, char** targets)
{
	int i;
	Cpe_t *ccpe, *tcpe;
	
	ccpe = cpe_new(candidate); // candidate cpe
	if (ccpe == NULL)
		return -2;

	for (i = 0; i < n; ++i) {
		tcpe = cpe_new(targets[i]); // target cpe

		if (cpe_name_match_one(ccpe, tcpe)) {
			// CPE matched
			cpe_delete(ccpe);
			cpe_delete(tcpe);
			return i;
		}

		cpe_delete(tcpe);
	}
	
	cpe_delete(ccpe);
	return -1;
}

bool cpe_check(const char* str)
{
	int length = strlen(str); /* Get the length for PCRE */

	pcre *re;
	const char *error;
	int erroffset;
	re = pcre_compile(
		//"^[Cc][Pp][Ee]:/(?P<part>[oahOAH])(?::(?P<vendor>[-%.~_a-zA-Z0-9]*))?(?::(?P<product>[-%.~_a-zA-Z0-9]*))?(?::(?P<version>[-%.~_a-zA-Z0-9]*))?(?::(?P<update>[-%.~_a-zA-Z0-9]*))?(?::(?P<edition>[-%.~_a-zA-Z0-9]*))?(?::(?P<language>[-%.~_a-zA-Z0-9]*))?$", /* the pattern */
		"^cpe:/[aho]?(:[a-z0-9._~%-]*){0,6}$",
		PCRE_CASELESS,    /* case insensitive */
		&error,           /* for error message */
		&erroffset,       /* for error offset */
		NULL);            /* use default character tables */

	int rc;
	int ovector[30];
	rc = pcre_exec(
		re,      /* result of pcre_compile() */
		NULL,    /* we didn't study the pattern */
		str,     /* the subject string */
		length,  /* the length of the subject string */
		0,       /* start at offset 0 in the subject */
		0,       /* default options */
		ovector, /* vector of integers for substring information */
		30);     /* number of elements (NOT size in bytes) */

	pcre_free(re);

	return rc >= 0;
}

const char* as_str(const char* str)
{
	if (str == NULL) return "";
	return str;
}

char* cpe_get_uri(const Cpe_t* cpe)
{
	int len = 16;
	int i;
	char* result;
	char* tptr;
	char* sptr;

	if (cpe == NULL || cpe->fields_ == NULL)
		return NULL;

	for (i = 0; cpe->fields_[i] && i < CPE_FIELDNUM; ++i)
		len += strlen(cpe->fields_[i]);

	result = malloc(len * sizeof(char));
	if (result == NULL)
		return NULL;

	i = sprintf(result, "cpe:/%s:%s:%s:%s:%s:%s:%s",
		as_str(CPE_PART_CHAR[cpe->part]),
		as_str(cpe->vendor),
		as_str(cpe->product),
		as_str(cpe->version),
		as_str(cpe->update),
		as_str(cpe->edition),
		as_str(cpe->language)
	);

	// trim trailing colons
	while (result[--i] == ':') result[i] = '\0';

	return result;
}

int cpe_write(const Cpe_t* cpe, FILE* f)
{
	int ret;
	char* uri;
	
	uri = cpe_get_uri(cpe);
	if (uri == NULL)
		return EOF;

	ret = fprintf(f, "%s", uri);

	free(uri);
	return ret;
}

bool cpe_assign_values(Cpe_t* cpe, char** fields)
{
	int i;

	if (cpe == NULL || fields == NULL) return false;
	
	for (i = 0; fields[i]; ++i) {
		const char* out = (fields[i] && strcmp(fields[i], "") != 0 ? fields[i] : NULL);
		switch (i) {
			case CPE_FIELD_TYPE:
				if (out) {
					if (strcasecmp(out, "h") == 0)
						cpe->part = CPE_PART_HW;
					else if (strcasecmp(out, "o") == 0)
						cpe->part = CPE_PART_OS;
					else if (strcasecmp(out, "a") == 0)
						cpe->part = CPE_PART_APP;
					else return false;
				}
				else return false;
				break;
			case CPE_FIELD_VENDOR:   cpe->vendor   = out; break;
			case CPE_FIELD_PRODUCT:  cpe->product  = out; break;
			case CPE_FIELD_VERSION:  cpe->version  = out; break;
			case CPE_FIELD_UPDATE:   cpe->update   = out; break;
			case CPE_FIELD_EDITION:  cpe->edition  = out; break;
			case CPE_FIELD_LANGUAGE: cpe->language = out; break;
			case CPE_FIELDNUM: return false;
		}
	}
	return true;
}

void cpe_delete(Cpe_t* cpe)
{
	if (cpe != NULL) {
		free(cpe->data_);
		free(cpe->fields_);
	}
	free(cpe);
}

size_t ptrarray_length(void** arr)
{
	if (arr == NULL) return 0;

	size_t s = 0;
	while (arr[s]) ++s;
	return s;
}


