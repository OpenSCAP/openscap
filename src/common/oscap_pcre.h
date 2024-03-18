/*
 * Copyright 2023 Red Hat Inc., Durham, North Carolina.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors:
 *      Evgenii Kolesnikov <ekolesni@redhat.com>
 */

#ifndef OSCAP_PCRE_
#define OSCAP_PCRE_

typedef struct oscap_pcre oscap_pcre_t;

typedef enum {
	OSCAP_PCRE_OPTS_NONE                        = 0x0001,
	OSCAP_PCRE_OPTS_UTF8                        = 0x0002,
	OSCAP_PCRE_OPTS_MULTILINE                   = 0x0004,
	OSCAP_PCRE_OPTS_DOTALL                      = 0x0008,
	OSCAP_PCRE_OPTS_CASELESS                    = 0x0010,
	OSCAP_PCRE_OPTS_NO_UTF8_CHECK               = 0x0020,
	OSCAP_PCRE_OPTS_PARTIAL                     = 0x0040,
} oscap_pcre_options_t;

typedef enum {
	OSCAP_PCRE_ERR_NOMATCH                      = -1,
	OSCAP_PCRE_ERR_PARTIAL                      = -2,
	OSCAP_PCRE_ERR_BADPARTIAL                   = -3,
	OSCAP_PCRE_ERR_BADUTF8                      = -10,
	OSCAP_PCRE_ERR_RECURSIONLIMIT               = -21,
	OSCAP_PCRE_ERR_UNKNOWN                      = -100,
} oscap_pcre_error_t;


/**
 * Compile a regular expression string into PCRE object and returns it.
 * Caller is responsible for freeing the returned object or the error message
 * if the result is NULL (USE oscap_pcre_err_free()!).
 * @param pattern expresstion string
 * @param options compile options
 * @param errptr a return value for a string representation of error
 * @param erroffset the offset in the expression where the problem was detected
 * @return a PCRE object
 * NULL on failure
 */
oscap_pcre_t* oscap_pcre_compile(const char *pattern, oscap_pcre_options_t options,
                                 char **errptr, int *erroffset);

/**
 * Execute the compiled regular expression against a string subject and returns
 * matches count (or a negative error code).
 * @param opcre the oscap_pcre_t object
 * @param subject target string
 * @param length target string length
 * @param startoffset the offset for the target string
 * @param options match options
 * @param ovector the output vector for match offset pairs, see pcre_exec for details
 * @param ovecsize the size of ovector, see pcre_exec for details
 * @return matches count
 * negative error code on failure
 */
int oscap_pcre_exec(const oscap_pcre_t *opcre, const char *subject,
                    int length, int startoffset, oscap_pcre_options_t options,
                    int *ovector, int ovecsize);

/**
 * Free the compiled regular expression object.
 * @param opcre the oscap_pcre_t object
 */
void oscap_pcre_free(oscap_pcre_t *opcre);

/**
 * Limit the compiled regular expression object's recursion depth for future
 * matches.
 * @param opcre the oscap_pcre_t object
 * @param limit maximum depth
 */
void oscap_pcre_set_match_limit_recursion(oscap_pcre_t *opcre, unsigned long limit);

/**
 * Optimize the compiled regular expression object to increase matching speed.
 * @param opcre the oscap_pcre_t object
 */
void oscap_pcre_optimize(oscap_pcre_t *opcre);

/**
 * Match a regular expression and return substrings.
 * Caller is responsible for freeing the returned array.
 * @param str subject string
 * @param ofs starting offset in str
 * @param re compiled regular expression
 * @param want_substrs if non-zero, substrings will be returned
 * @param substrings contains returned substrings
 * @return count of matched substrings, 0 if no match
 * negative value on failure
 */
int oscap_pcre_get_substrings(char *str, int *ofs, oscap_pcre_t *re, int want_substrs, char ***substrings);

/**
 * Free the error message returned by oscap_pcre_compile. DON'T USE REGULAR free()!
 * @param err the message
 */
void oscap_pcre_err_free(char *err);

#endif
