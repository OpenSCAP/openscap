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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory.h>

#define OSCAP_PCRE_EXEC_RECURSION_LIMIT_DEFAULT 3500

#ifdef HAVE_PCRE2
#define PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_ERR_BUF_SIZE 127
#include <pcre2.h>
#else
#include <pcre.h>
#endif

#include "debug_priv.h"
#include "oscap_pcre.h"


struct oscap_pcre {
#ifdef HAVE_PCRE2
	pcre2_code_8           *re;
	pcre2_match_context_8  *re_ctx;
#else
	pcre                   *re;
	struct pcre_extra      *re_extra;
#endif
};


static inline int _oscap_pcre_opts_to_pcre(oscap_pcre_options_t opts)
{
	int res = 0;
#ifdef HAVE_PCRE2
	if (opts & OSCAP_PCRE_OPTS_UTF8)
		res |= PCRE2_UTF;
	if (opts & OSCAP_PCRE_OPTS_MULTILINE)
		res |= PCRE2_MULTILINE;
	if (opts & OSCAP_PCRE_OPTS_DOTALL)
		res |= PCRE2_DOTALL;
	if (opts & OSCAP_PCRE_OPTS_CASELESS)
		res |= PCRE2_CASELESS;
	if (opts & OSCAP_PCRE_OPTS_NO_UTF8_CHECK)
		res |= PCRE2_NO_UTF_CHECK;
	if (opts & OSCAP_PCRE_OPTS_PARTIAL)
		res |= PCRE2_PARTIAL_SOFT;
#else
	if (opts & OSCAP_PCRE_OPTS_UTF8)
		res |= PCRE_UTF8;
	if (opts & OSCAP_PCRE_OPTS_MULTILINE)
		res |= PCRE_MULTILINE;
	if (opts & OSCAP_PCRE_OPTS_DOTALL)
		res |= PCRE_DOTALL;
	if (opts & OSCAP_PCRE_OPTS_CASELESS)
		res |= PCRE_CASELESS;
	if (opts & OSCAP_PCRE_OPTS_NO_UTF8_CHECK)
		res |= PCRE_NO_UTF8_CHECK;
	if (opts & OSCAP_PCRE_OPTS_PARTIAL)
		res |= PCRE_PARTIAL;
#endif
	return res;
};

static inline oscap_pcre_error_t _pcre_error_to_oscap_pcre(int res)
{
	switch (res) {
#ifdef HAVE_PCRE2
	case PCRE2_ERROR_NOMATCH:
		return OSCAP_PCRE_ERR_NOMATCH;
	case PCRE2_ERROR_PARTIAL:
		return OSCAP_PCRE_ERR_PARTIAL;
	case PCRE2_ERROR_UTF8_ERR1:
	case PCRE2_ERROR_UTF8_ERR2:
	case PCRE2_ERROR_UTF8_ERR3:
	case PCRE2_ERROR_UTF8_ERR4:
	case PCRE2_ERROR_UTF8_ERR5:
	case PCRE2_ERROR_UTF8_ERR6:
	case PCRE2_ERROR_UTF8_ERR7:
	case PCRE2_ERROR_UTF8_ERR8:
	case PCRE2_ERROR_UTF8_ERR9:
	case PCRE2_ERROR_UTF8_ERR10:
	case PCRE2_ERROR_UTF8_ERR11:
	case PCRE2_ERROR_UTF8_ERR12:
	case PCRE2_ERROR_UTF8_ERR13:
	case PCRE2_ERROR_UTF8_ERR14:
	case PCRE2_ERROR_UTF8_ERR15:
	case PCRE2_ERROR_UTF8_ERR16:
	case PCRE2_ERROR_UTF8_ERR17:
	case PCRE2_ERROR_UTF8_ERR18:
	case PCRE2_ERROR_UTF8_ERR19:
	case PCRE2_ERROR_UTF8_ERR20:
	case PCRE2_ERROR_UTF8_ERR21:
		return OSCAP_PCRE_ERR_BADUTF8;
	case PCRE2_ERROR_RECURSIONLIMIT:
		return OSCAP_PCRE_ERR_RECURSIONLIMIT;
#else
	case PCRE_ERROR_NOMATCH:
		return OSCAP_PCRE_ERR_NOMATCH;
	case PCRE_ERROR_PARTIAL:
		return OSCAP_PCRE_ERR_PARTIAL;
	case PCRE_ERROR_BADPARTIAL:
		return OSCAP_PCRE_ERR_BADPARTIAL;
	case PCRE_ERROR_BADUTF8:
		return OSCAP_PCRE_ERR_BADUTF8;
	case PCRE_ERROR_RECURSIONLIMIT:
		return OSCAP_PCRE_ERR_RECURSIONLIMIT;
#endif
	default:
		dW("Unknown PCRE error code: %d", res);
		return OSCAP_PCRE_ERR_UNKNOWN;
	}
}

oscap_pcre_t *oscap_pcre_compile(const char *pattern, oscap_pcre_options_t options,
                                 char **errptr, int *erroffset)
{
	oscap_pcre_t *res = malloc(sizeof(oscap_pcre_t));
#ifdef HAVE_PCRE2
	int errno;
	PCRE2_SIZE erroffset2;
	res->re_ctx = NULL;
	dD("pcre2_compile_8: patt=%s", pattern);
	res->re = pcre2_compile_8((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, _oscap_pcre_opts_to_pcre(options), &errno, &erroffset2, NULL);
	if (res->re == NULL) {
		PCRE2_UCHAR8 errmsg[PCRE2_ERR_BUF_SIZE];
		pcre2_get_error_message_8(errno, errmsg, PCRE2_ERR_BUF_SIZE);
		dW("pcre2_compile_8: error (at offset %d): %s", erroffset2, errmsg);
		*erroffset = erroffset2;
		*errptr = strdup((const char*)errmsg);
	}
#else
	res->re_extra = NULL;
	dD("pcre_compile: patt=%s", pattern);
	res->re = pcre_compile(pattern, _oscap_pcre_opts_to_pcre(options), (const char **)errptr, erroffset, NULL);
	if (res->re == NULL)
		dW("pcre_compile: error (at offset %d): %s", *erroffset, *errptr);
#endif
	if (res->re == NULL) {
		free(res);
		res = NULL;
	}
	return res;
}

void oscap_pcre_optimize(oscap_pcre_t *opcre)
{
#ifdef HAVE_PCRE2
	// This is a NOOP for PCRE2 as all patterns are optimized
	// unless the library configured differently.
#else
	const char *errptr = NULL;
	pcre_extra *extra = pcre_study(opcre->re, 0, &errptr);
	if (extra != NULL) {
		if (opcre->re_extra != NULL)
			free(opcre->re_extra);
		opcre->re_extra = extra;
	}
#endif
}

void oscap_pcre_set_match_limit_recursion(oscap_pcre_t *opcre, unsigned long limit)
{
#ifdef HAVE_PCRE2
	if (opcre->re_ctx == NULL) {
		opcre->re_ctx = pcre2_match_context_create_8(NULL);
	}
	pcre2_set_depth_limit_8(opcre->re_ctx, limit);
#else
	if (opcre->re_extra == NULL) {
		opcre->re_extra = calloc(1, sizeof(struct pcre_extra));
	}
	opcre->re_extra->match_limit_recursion = limit;
	opcre->re_extra->flags = PCRE_EXTRA_MATCH_LIMIT_RECURSION;
#endif
}

int oscap_pcre_exec(const oscap_pcre_t *opcre, const char *subject,
                    int length, int startoffset, oscap_pcre_options_t options,
                    int *ovector, int ovecsize)
{
	int rc = 0;
#ifdef HAVE_PCRE2
	// The ovecsize is multiplied by 3 in the code for compatibility with PCRE1
	int ovecsize2 = ovecsize/3;
	pcre2_match_data_8 *mdata = pcre2_match_data_create_8(ovecsize2, NULL);
	dD("pcre2_match_8: subj=%s", subject);
	rc = pcre2_match_8(opcre->re, (PCRE2_SPTR8)subject, length, startoffset, _oscap_pcre_opts_to_pcre(options), mdata, opcre->re_ctx);
	dD("pcre2_match_8: rc=%d, ", rc);
	if (rc > PCRE2_ERROR_NOMATCH) {
		PCRE2_SIZE *ovecp = pcre2_get_ovector_pointer_8(mdata);
		uint32_t ovecp_count = pcre2_get_ovector_count_8(mdata);
		dD("pcre2_match_8: pcre2_get_ovector_count_8=%d", ovecp_count);
		for (int i = 0; i < rc; i++) {
			if (i < ovecsize2) {
				ovector[i*2] = ovecp[i*2];
				ovector[i*2+1] = ovecp[i*2+1];
			}
		}
	}
	pcre2_match_data_free_8(mdata);
#else
	dD("pcre_exec: subj=%s", subject);
	rc = pcre_exec(opcre->re, opcre->re_extra, subject, length, startoffset, _oscap_pcre_opts_to_pcre(options), ovector, ovecsize);
	dD("pcre_exec: rc=%d, ", rc);
#endif
	return rc >= 0 ? rc : _pcre_error_to_oscap_pcre(rc);
}

void oscap_pcre_free(oscap_pcre_t *opcre)
{
	if (opcre != NULL) {
#ifdef HAVE_PCRE2
		if (opcre->re_ctx != NULL)
			pcre2_match_context_free_8(opcre->re_ctx);
		pcre2_code_free_8(opcre->re);
#else
		if (opcre->re_extra != NULL)
			free(opcre->re_extra);
		pcre_free(opcre->re);
#endif
		free(opcre);
	}
}

int oscap_pcre_get_substrings(char *str, int *ofs, oscap_pcre_t *re, int want_substrs, char ***substrings) {
	int i, ret, rc;
	int ovector[60], ovector_len = sizeof (ovector) / sizeof (ovector[0]);
	char **substrs;

	// todo: max match count check

	for (i = 0; i < ovector_len; ++i) {
		ovector[i] = -1;
	}

	unsigned long limit = OSCAP_PCRE_EXEC_RECURSION_LIMIT_DEFAULT;
	char *limit_str = getenv("OSCAP_PCRE_EXEC_RECURSION_LIMIT");
	if (limit_str != NULL)
		if (sscanf(limit_str, "%lu", &limit) <= 0)
			dW("Unable to parse OSCAP_PCRE_EXEC_RECURSION_LIMIT value");
	oscap_pcre_set_match_limit_recursion(re, limit);
	size_t str_len = strlen(str);
#if defined(OS_SOLARIS)
	rc = oscap_pcre_exec(re, str, str_len, *ofs, OSCAP_PCRE_OPTS_NO_UTF8_CHECK, ovector, ovector_len);
#else
	rc = oscap_pcre_exec(re, str, str_len, *ofs, 0, ovector, ovector_len);
#endif

	if (rc < OSCAP_PCRE_ERR_NOMATCH) {
		if (str_len < 100)
			dE("Function oscap_pcre_exec() failed to match a regular expression with return code %d on string '%s'.", rc, str);
		else
			dE("Function oscap_pcre_exec() failed to match a regular expression with return code %d on string '%.100s' (truncated, showing first 100 characters).", rc, str);
		return rc;
	} else if (rc == OSCAP_PCRE_ERR_NOMATCH) {
		/* no match */
		return 0;
	}

	*ofs = (*ofs == ovector[1]) ? ovector[1] + 1 : ovector[1];

	if (!want_substrs) {
		/* just report successful match */
		return 1;
	}

	ret = 0;
	if (rc == 0) {
		/* vector too small */
		// todo: report partial results
		rc = ovector_len / 3;
	}

	substrs = malloc(rc * sizeof (char *));
	for (i = 0; i < rc; ++i) {
		int len;
		char *buf;

		if (ovector[2 * i] == -1) {
			continue;
		}
		len = ovector[2 * i + 1] - ovector[2 * i];
		buf = malloc(len + 1);
		memcpy(buf, str + ovector[2 * i], len);
		buf[len] = '\0';
		substrs[ret] = buf;
		++ret;
	}

	*substrings = substrs;

	return ret;
}

void oscap_pcre_err_free(char *err)
{
	if (err != NULL) {
#ifdef HAVE_PCRE2
		free(err);
#else
		// PCRE1 error messages are static, NOOP.
#endif
	}
}

