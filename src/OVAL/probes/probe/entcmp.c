/**
 * @file   probe-entcmp.c
 * @author "Tomas Heinrich" <theinric@redhat.com>
 *
 * @addtogroup PROBEAPI
 * @{
 */
/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
 *      Tomas Heinrich <theinric@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sexp.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#if defined USE_REGEX_PCRE
#include <pcre.h>
#elif defined USE_REGEX_POSIX
#include <regex.h>
#endif
#include <arpa/inet.h>

#include "common/debug_priv.h"
#include "entcmp.h"
#include "../_probe-api.h"

oval_result_t probe_ent_cmp_binary(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
	}

        oscap_free(s1);
        oscap_free(s2);

	return result;
}

oval_result_t probe_ent_cmp_bool(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	int v1, v2;

	v1 = SEXP_number_geti_32(val1);
	v2 = SEXP_number_geti_32(val2);

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_t probe_ent_cmp_evr(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_t probe_ent_cmp_filesetrev(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_t probe_ent_cmp_float(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	double v1, v2;

	v1 = SEXP_number_getf(val1);
	v2 = SEXP_number_getf(val2);

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN:
		if (v1 < v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN:
		if (v1 > v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		if (v1 <= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		if (v1 >= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_t probe_ent_cmp_int(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	int64_t v1, v2;

	v1 = SEXP_number_geti_64(val1);
	v2 = SEXP_number_geti_64(val2);

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN:
		if (v1 < v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN:
		if (v1 > v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		if (v1 <= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		if (v1 >= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_BITWISE_AND:
		if ((v1 & v2) == v1)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_BITWISE_OR:
		if ((v1 | v2) == v1)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_t probe_ent_cmp_ios(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

static SEXP_t *version_parser(char *version)
{
	long long int token;
	size_t len, nbr_len;
	char *s, *v_dup = NULL;
	const char *ac = "0123456789";
	SEXP_t *version_tokens = NULL, *r0;

	len = strlen(version);
	if (len == 0)
		goto fail;

	nbr_len = strspn(version, ac);
	if (nbr_len == 0)
		/* version string starts with a delimiter */
		goto fail;

	s = v_dup = strdup(version);
	version_tokens = SEXP_list_new(NULL);

	while (len > 0) {
		nbr_len = strspn(s, ac);
		if (nbr_len == 0)
			/* consecutive delimiters */
			goto fail;
		s[nbr_len] = '\0';
		token = atoll(s);
		SEXP_list_add(version_tokens, r0 = SEXP_number_newi_64(token));
		s += nbr_len + 1;
		len -= nbr_len + 1;
                SEXP_free(r0);
	}
	if (len == 0)
		/* trailing delimiter */
		goto fail;

	if (v_dup != NULL)
		free(v_dup);

	return version_tokens;
 fail:
	if (v_dup != NULL)
		free(v_dup);
	if (version_tokens != NULL)
		SEXP_free(version_tokens);

	return NULL;
}

oval_result_t probe_ent_cmp_version(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	SEXP_t *v1_tkns = NULL, *v2_tkns = NULL, *stmp, *r0;
	char *vtmp;
	size_t len, i;
	int lendif;
	long long int v1, v2;

	switch (op) {
	case OVAL_OPERATION_EQUALS:
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		result = OVAL_RESULT_TRUE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
	case OVAL_OPERATION_GREATER_THAN:
	case OVAL_OPERATION_LESS_THAN:
		result = OVAL_RESULT_FALSE;
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
		goto fail;
	}

	vtmp = SEXP_string_cstr(val1);
	v1_tkns = version_parser(vtmp);
	free(vtmp);
	if (v1_tkns == NULL)
		goto fail;

	vtmp = SEXP_string_cstr(val2);
	v2_tkns = version_parser(vtmp);
	free(vtmp);
	if (v2_tkns == NULL)
		goto fail;

	/* align token counts */
	lendif = SEXP_list_length(v1_tkns) - SEXP_list_length(v2_tkns);
	if (lendif < 0) {
		lendif = -lendif;
		stmp = v1_tkns;
	} else if (lendif > 0) {
		stmp = v2_tkns;
	}
	for (; lendif > 0; --lendif) {
		SEXP_list_add(stmp, r0 = SEXP_number_newi_64(0));
                SEXP_free(r0);
	}

	len = SEXP_list_length(v1_tkns);
	for (i = 1; i <= len; ++i) {
		v1 = SEXP_number_geti_64(r0 = SEXP_list_nth(v1_tkns, i));
                SEXP_free(r0);
		v2 = SEXP_number_geti_64(r0 = SEXP_list_nth(v2_tkns, i));
                SEXP_free(r0);

		if (op == OVAL_OPERATION_EQUALS) {
			if (v1 != v2) {
				result = OVAL_RESULT_FALSE;
				break;
			}
		} else if (op == OVAL_OPERATION_NOT_EQUAL) {
			if (v1 != v2) {
				result = OVAL_RESULT_TRUE;
				break;
			}
		} else if ((op == OVAL_OPERATION_GREATER_THAN) || (op == OVAL_OPERATION_GREATER_THAN_OR_EQUAL)) {
			if (v1 < v2) {
				result = OVAL_RESULT_TRUE;
				break;
			} else if (v1 > v2) {
				result = OVAL_RESULT_FALSE;
				break;
			}
		} else if ((op == OVAL_OPERATION_LESS_THAN) || (op == OVAL_OPERATION_LESS_THAN_OR_EQUAL)) {
			if (v1 > v2) {
				result = OVAL_RESULT_TRUE;
				break;
			} else if (v1 < v2) {
				result = OVAL_RESULT_FALSE;
				break;
			}
		}
	}

 fail:
	SEXP_free(v1_tkns);
	SEXP_free(v2_tkns);
	return result;
}

oval_result_t probe_ent_cmp_string(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!strcmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (strcmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_CASE_INSENSITIVE_EQUALS:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_PATTERN_MATCH:
		{
#if defined USE_REGEX_PCRE
			int erroffset = -1, rc;
			const char *error;
			pcre *re = NULL;

			re = pcre_compile(s1, PCRE_UTF8, &error, &erroffset, NULL);
			if (re == NULL) {
                                oscap_free(s1);
                                oscap_free(s2);
				return OVAL_RESULT_ERROR;
			}

			rc = pcre_exec(re, NULL, s2, strlen(s2), 0, 0, NULL, 0);
			pcre_free(re);

			if (rc == 0) {
				result = OVAL_RESULT_TRUE;
			} else if (rc == -1) {
				result = OVAL_RESULT_FALSE;
			} else {
				result = OVAL_RESULT_ERROR;
			}
#elif defined USE_REGEX_POSIX
			regex_t re;

			if (regcomp(&re, s1, REG_EXTENDED) != 0) {
                                oscap_free(s1);
                                oscap_free(s2);

				return OVAL_RESULT_ERROR;
			}

			if (regexec(&re, s2, 0, NULL, 0) == REG_NOMATCH) {
				result = OVAL_RESULT_FALSE;
			} else {
				result = OVAL_RESULT_TRUE;
			}

			regfree(&re);
#endif
		}
		break;
	default:
		dI("Unexpected compare operation: %d\n", op);
	}

        oscap_free(s1);
        oscap_free(s2);

	return result;
}

static oval_result_t probe_ent_cmp_ipv4addr(SEXP_t *val1, SEXP_t *val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s, *pfx;
	int nm1, nm2;
	struct in_addr addr1, addr2;

        memset(&addr1, 0, sizeof (struct in_addr));
        memset(&addr2, 0, sizeof (struct in_addr));

	s = SEXP_string_cstr(val1);
	pfx = strchr(s, '/');
	if (pfx) {
		int cnt;
		char nm[4];

		*pfx++ = '\0';
		cnt = sscanf(pfx, "%hhu.%hhu.%hhu.%hhu", &nm[0], &nm[1], &nm[2], &nm[3]);
		if (cnt > 1) { /* netmask */
			nm1 = (nm[0] << 24) + (nm[1] << 16) + (nm[2] << 8) + nm[3];
		} else { /* prefix */
			nm1 = (~0) << (32 - nm[0]);
		}
	} else {
		nm1 = ~0;
	}
	if (inet_pton(AF_INET, s, &addr1) <= 0) {
		dW("inet_pton() failed.\n");
		goto cleanup;
	}

	oscap_free(s);
	s = SEXP_string_cstr(val2);
	pfx = strchr(s, '/');
	if (pfx) {
		int cnt;
		char nm[4];

		*pfx++ = '\0';
		cnt = sscanf(pfx, "%hhu.%hhu.%hhu.%hhu", &nm[0], &nm[1], &nm[2], &nm[3]);
		if (cnt > 1) { /* netmask */
			nm2 = (nm[0] << 24) + (nm[1] << 16) + (nm[2] << 8) + nm[3];
		} else { /* prefix */
			nm2 = (~0) << (32 - nm[0]);
		}
	} else {
		nm2 = ~0;
	}
	if (inet_pton(AF_INET, s, &addr2) <= 0) {
		dW("inet_pton() failed.\n");
		goto cleanup;
	}

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!memcmp(&addr1, &addr2, sizeof(struct in_addr)) && nm1 == nm2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) || nm1 != nm2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUBSET_OF:
		if (nm1 <= nm2) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_GREATER_THAN:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) > 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) >= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUPERSET_OF:
		if (nm1 >= nm2) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_LESS_THAN:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) < 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		addr1.s_addr &= nm1;
		addr2.s_addr &= nm2;
		if (memcmp(&addr1, &addr2, sizeof(struct in_addr)) <= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dE("Unexpected compare operation: %d.\n", op);
	}

 cleanup:
	oscap_free(s);

	return (result);
}

static void mask_v6_addrs(struct in6_addr *addr1, int p1len, struct in6_addr *addr2, int p2len)
{
	int i;

	for (i = 0; i < 128; ++i) {
		addr1->s6_addr[i / 8] &= (((i < p1len) ? 1 : 0) << (i % 8));
		addr2->s6_addr[i / 8] &= (((i < p2len) ? 1 : 0) << (i % 8));
	}
}

static oval_result_t probe_ent_cmp_ipv6addr(SEXP_t *val1, SEXP_t *val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s, *pfx;
	int p1len, p2len;
	struct in6_addr addr1, addr2;

	s = SEXP_string_cstr(val1);
	pfx = strchr(s, '/');
	if (pfx) {
		*pfx++ = '\0';
		p1len = strtol(pfx, NULL, 10);
	} else {
		p1len = 128;
	}
	if (inet_pton(AF_INET6, s, &addr1) <= 0) {
		dW("inet_pton() failed.\n");
		goto cleanup;
	}

	oscap_free(s);
	s = SEXP_string_cstr(val2);
	pfx = strchr(s, '/');
	if (pfx) {
		*pfx++ = '\0';
		p2len = strtol(pfx, NULL, 10);
	} else {
		p2len = 128;
	}
	if (inet_pton(AF_INET6, s, &addr2) <= 0) {
		dW("inet_pton() failed.\n");
		goto cleanup;
	}

	switch (op) {
	case OVAL_OPERATION_EQUALS:
		if (!memcmp(&addr1, &addr2, sizeof(struct in6_addr)) && p1len == p2len)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_NOT_EQUAL:
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) || p1len != p2len)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUBSET_OF:
		if (p1len <= p2len) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_GREATER_THAN:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) > 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) >= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_SUPERSET_OF:
		if (p1len >= p2len) {
			result = OVAL_RESULT_FALSE;
			break;
		}
		/* FALLTHROUGH */
	case OVAL_OPERATION_LESS_THAN:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) < 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		mask_v6_addrs(&addr1, p1len, &addr2, p2len);
		if (memcmp(&addr1, &addr2, sizeof(struct in6_addr)) <= 0)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		dE("Unexpected compare operation: %d.\n", op);
	}

 cleanup:
	oscap_free(s);

	return result;
}

static oval_result_t probe_ent_cmp(SEXP_t * ent, SEXP_t * val2)
{
	oval_operation_t op;
	oval_datatype_t dtype;
	SEXP_t *stmp, *val1, *vals, *res_lst, *r0;
	int val_cnt, is_var;
	oval_check_t ochk;
	oval_result_t ores, result;

	ores = OVAL_RESULT_ERROR;
	result = OVAL_RESULT_ERROR;
        vals = NULL;
	val_cnt = probe_ent_getvals(ent, &vals);

	if (probe_ent_attrexists(ent, "var_ref")) {
		is_var = 1;
	} else {
		if (val_cnt != 1) {
                        SEXP_free(vals);
			return OVAL_RESULT_ERROR;
                }

		is_var = 0;
	}

	dtype = probe_ent_getdatatype(ent);
	stmp = probe_ent_getattrval(ent, "operation");
	if (stmp == NULL)
		op = OVAL_OPERATION_EQUALS;
	else
		op = SEXP_number_geti_32(stmp);
        SEXP_free(stmp);
	res_lst = SEXP_list_new(NULL);

	SEXP_list_foreach(val1, vals) {
		if (SEXP_typeof(val1) != SEXP_typeof(val2)) {
			dI("Types of values to compare don't match: val1: %d, val2: %d\n",
			   SEXP_typeof(val1), SEXP_typeof(val2));

                        SEXP_free(vals);
                        SEXP_free(val1);
                        SEXP_free(res_lst);

			return OVAL_RESULT_ERROR;
		}

		switch (dtype) {
		case OVAL_DATATYPE_BINARY:
			ores = probe_ent_cmp_binary(val1, val2, op);
			break;
		case OVAL_DATATYPE_BOOLEAN:
			ores = probe_ent_cmp_bool(val1, val2, op);
			break;
		case OVAL_DATATYPE_EVR_STRING:
			ores = probe_ent_cmp_evr(val1, val2, op);
			break;
		case OVAL_DATATYPE_FILESET_REVISION:
			ores = probe_ent_cmp_filesetrev(val1, val2, op);
			break;
		case OVAL_DATATYPE_FLOAT:
			ores = probe_ent_cmp_float(val1, val2, op);
			break;
		case OVAL_DATATYPE_IOS_VERSION:
			ores = probe_ent_cmp_ios(val1, val2, op);
			break;
		case OVAL_DATATYPE_VERSION:
			ores = probe_ent_cmp_version(val1, val2, op);
			break;
		case OVAL_DATATYPE_INTEGER:
			ores = probe_ent_cmp_int(val1, val2, op);
			break;
		case OVAL_DATATYPE_STRING:
			ores = probe_ent_cmp_string(val1, val2, op);
			break;
		case OVAL_DATATYPE_IPV4ADDR:
			ores = probe_ent_cmp_ipv4addr(val1, val2, op);
			break;
		case OVAL_DATATYPE_IPV6ADDR:
			ores = probe_ent_cmp_ipv6addr(val1, val2, op);
			break;
		default:
			ores = OVAL_RESULT_ERROR;
			dI("Unexpected data type: %d\n", dtype);
		}

		SEXP_list_add(res_lst, r0 = SEXP_number_newi_32(ores));
                SEXP_free(r0);
	}

	if (is_var) {
		stmp = probe_ent_getattrval(ent, "var_check");
		if (stmp == NULL) {
			ochk = OVAL_CHECK_ALL;
		} else {
			ochk = SEXP_number_geti_32(stmp);
			SEXP_free(stmp);
		}

		result = probe_ent_result_bychk(res_lst, ochk);
	} else {
		result = ores;
	}

	SEXP_free(res_lst);
        SEXP_free(vals);

	return result;
}

static oval_result_t _probe_entste_cmp_record(SEXP_t *ent_ste, SEXP_t *ent_itm)
{
	oval_result_t res;
	oval_operation_t op;
	oval_check_t ochk;
	SEXP_t *stmp, *ste_res, *ste_record_fields, *ste_rf, *itm_record_fields;
	int val_cnt;

	stmp = probe_ent_getattrval(ent_ste, "operation");
	if (stmp == NULL) {
		op = OVAL_OPERATION_EQUALS;
	} else {
		op = SEXP_number_getu(stmp);
		SEXP_free(stmp);
		if (op != OVAL_OPERATION_EQUALS)
			return OVAL_RESULT_ERROR;
	}

	val_cnt = probe_ent_getvals(ent_ste, &ste_record_fields);
	if (val_cnt <= 0) {
		SEXP_free(ste_record_fields);
		return OVAL_RESULT_ERROR;
	}
	val_cnt = probe_ent_getvals(ent_itm, &itm_record_fields);
	if (val_cnt <= 0) {
		SEXP_free(ste_record_fields);
		SEXP_free(itm_record_fields);
		return OVAL_RESULT_ERROR;
	}

	ste_res = SEXP_list_new(NULL);

	SEXP_list_foreach(ste_rf, ste_record_fields) {
		SEXP_t *itm_rf, *itm_res;
		const char *sname;
		bool matched;

		sname = probe_ent_getname(ste_rf);
		itm_res = SEXP_list_new(NULL);
		matched = false;

		SEXP_list_foreach(itm_rf, itm_record_fields) {
			const char *iname;

			iname = probe_ent_getname(itm_rf);
			if (strcmp(sname, iname)) {
				oscap_free(iname);
				continue;
			}
			oscap_free(iname);
			matched = true;

			res = probe_entste_cmp(ste_rf, itm_rf);
			/* todo: _oval_result_to_sexp() */
			stmp = SEXP_number_newu(res);
			SEXP_list_add(itm_res, stmp);
			SEXP_free(stmp);
		}

		oscap_free(sname);

		if (!matched) {
			stmp = SEXP_number_newu(OVAL_RESULT_ERROR);
			SEXP_list_add(itm_res, stmp);
			SEXP_free(stmp);
		}

		stmp = probe_ent_getattrval(ste_rf, "entity_check");
		if (stmp == NULL) {
			ochk = OVAL_CHECK_ALL;
		} else {
			ochk = SEXP_number_getu(stmp);
			SEXP_free(stmp);
		}

		res = probe_ent_result_bychk(itm_res, ochk);
		SEXP_free(itm_res);
		stmp = SEXP_number_newu(res);
		SEXP_list_add(ste_res, stmp);
		SEXP_free(stmp);
	}

	SEXP_free(ste_record_fields);
	SEXP_free(itm_record_fields);

	stmp = probe_ent_getattrval(ent_ste, "entity_check");
	if (stmp == NULL) {
		ochk = OVAL_CHECK_ALL;
	} else {
		ochk = SEXP_number_getu(stmp);
		SEXP_free(stmp);
	}

	res = probe_ent_result_bychk(ste_res, ochk);
	SEXP_free(ste_res);

	return res;
}

oval_result_t probe_entste_cmp(SEXP_t * ent_ste, SEXP_t * ent_itm)
{
	oval_datatype_t ste_dt;
	oval_syschar_status_t item_status;
	oval_result_t ores;
	SEXP_t *val2;
	int valcnt;

	item_status = probe_ent_getstatus(ent_itm);
	switch (item_status) {
	case SYSCHAR_STATUS_DOES_NOT_EXIST:
		return OVAL_RESULT_FALSE;
	case SYSCHAR_STATUS_ERROR:
	case SYSCHAR_STATUS_NOT_COLLECTED:
		return OVAL_RESULT_ERROR;
	default:
		break;
	}

	ste_dt = probe_ent_getdatatype(ent_ste);
	if (ste_dt != probe_ent_getdatatype(ent_itm))
		return OVAL_RESULT_ERROR;

	if (ste_dt == OVAL_DATATYPE_RECORD)
		/* records require special handling */
		return _probe_entste_cmp_record(ent_ste, ent_itm);

	valcnt = probe_ent_getvals(ent_ste, NULL);
	if (valcnt == 0)
		return OVAL_RESULT_ERROR;

	val2 = probe_ent_getval(ent_itm);
	ores = probe_ent_cmp(ent_ste, val2);
        SEXP_free(val2);

	if (ores == OVAL_RESULT_NOT_EVALUATED)
		return OVAL_RESULT_ERROR;
	return ores;
}

oval_result_t probe_entobj_cmp(SEXP_t * ent_obj, SEXP_t * val)
{
	oval_result_t ores;
	SEXP_t *r0 = NULL;
	int valcnt;

	valcnt = probe_ent_getvals(ent_obj, &r0);
	SEXP_free(r0);
	if (valcnt == 0) {
		dI("valcnt == 0.\n");
		return OVAL_RESULT_FALSE;
	}

	ores = probe_ent_cmp(ent_obj, val);
#if defined(OSCAP_VERBOSE_DEBUG)
	dI("ores: %d, '%s'.\n", ores, oval_result_get_text(ores));
#endif
	if (ores == OVAL_RESULT_NOT_EVALUATED)
		return OVAL_RESULT_FALSE;
	return ores;
}

struct _oresults {
	int true_cnt, false_cnt, unknown_cnt, error_cnt, noteval_cnt, notappl_cnt;
};

static int results_parser(SEXP_t * res_lst, struct _oresults *ores)
{
	oval_result_t r;
	SEXP_t *res;

	memset(ores, 0, sizeof(struct _oresults));

	SEXP_list_foreach(res, res_lst) {
		r = SEXP_number_geti_32(res);
		switch (r) {
		case OVAL_RESULT_TRUE:
			++(ores->true_cnt);
			break;
		case OVAL_RESULT_FALSE:
			++(ores->false_cnt);
			break;
		case OVAL_RESULT_UNKNOWN:
			++(ores->unknown_cnt);
			break;
		case OVAL_RESULT_ERROR:
			++(ores->error_cnt);
			break;
		case OVAL_RESULT_NOT_EVALUATED:
			++(ores->noteval_cnt);
			break;
		case OVAL_RESULT_NOT_APPLICABLE:
			++(ores->notappl_cnt);
			break;
		default:
			return -1;
		}
	}

	return 0;
}

// todo: already implemented elsewhere; consolidate
oval_result_t probe_ent_result_bychk(SEXP_t * res_lst, oval_check_t check)
{
	oval_result_t result = OVAL_RESULT_UNKNOWN;
	struct _oresults ores;

	if (SEXP_list_length(res_lst) == 0)
		return OVAL_RESULT_UNKNOWN;

	if (results_parser(res_lst, &ores) != 0) {
		return OVAL_RESULT_ERROR;
	}

	if (ores.notappl_cnt > 0 &&
	    ores.noteval_cnt == 0 &&
	    ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch (check) {
	case OVAL_CHECK_ALL:
		if (ores.true_cnt > 0 &&
		    ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.false_cnt == 0 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_AT_LEAST_ONE:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0 &&
			   ores.true_cnt == 0 &&
			   ores.unknown_cnt == 0 && ores.error_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_NONE_EXIST:
		dW("The 'none exist' CheckEnumeration value has been deprecated. "
		   "Converted to check='none satisfy'.\n");
		/* FALLTHROUGH */
	case OVAL_CHECK_NONE_SATISFY:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores.false_cnt > 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 && ores.noteval_cnt == 0 && ores.true_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		}
		break;
	case OVAL_CHECK_ONLY_ONE:
		if (ores.true_cnt == 1 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt > 1) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt < 2 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt < 2 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt < 2 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores.true_cnt != 1 && ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		}
		break;
	default:
		break;
	}

	return result;
}

// todo: already implemented elsewhere; consolidate
oval_result_t probe_ent_result_byopr(SEXP_t * res_lst, oval_operator_t operator)
{
	oval_result_t result = OVAL_RESULT_UNKNOWN;
	struct _oresults ores;

	if (SEXP_list_length(res_lst) == 0)
		return OVAL_RESULT_UNKNOWN;

	if (results_parser(res_lst, &ores) != 0) {
		return OVAL_RESULT_ERROR;
	}

	if (ores.notappl_cnt > 0 &&
	    ores.noteval_cnt == 0 &&
	    ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch (operator) {
	case OVAL_OPERATOR_AND:
		if (ores.true_cnt > 0 &&
		    ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.false_cnt == 0 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_ONE:
		if (ores.true_cnt == 1 &&
		    ores.false_cnt >= 0 &&
		    ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt >= 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt >= 0 &&
			   ores.unknown_cnt >= 0 && ores.noteval_cnt >= 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 && ores.noteval_cnt == 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt > 0 &&
			   ores.unknown_cnt >= 0 && ores.noteval_cnt >= 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt >= 1 && ores.noteval_cnt >= 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 && ores.noteval_cnt > 0 && ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_OR:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt == 0 &&
			   ores.false_cnt > 0 &&
			   ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 && ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt == 0 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_OPERATOR_XOR:
		if ((ores.true_cnt % 2) == 1 && ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if ((ores.true_cnt % 2) == 0 &&
			   ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.error_cnt == 0 && ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.error_cnt == 0 && ores.unknown_cnt == 0 && ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	default:
		break;
	}

	return result;
}
