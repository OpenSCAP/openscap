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
 *     Tomas Heinrich <theinric@redhat.com>
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <string.h>
#include <pcre.h>

#include "oval_types.h"
#include "common/_error.h"
#include "common/debug_priv.h"
#include "oval_cmp_basic_impl.h"

oval_result_t oval_boolean_cmp(const bool state, const bool syschar, oval_operation_t operation)
{
	switch (operation) {
	case OVAL_OPERATION_EQUALS:
		return state == syschar ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_NOT_EQUAL:
		return state != syschar ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in boolean evaluation: %d.", operation);
	}
	return OVAL_RESULT_ERROR;
}

oval_result_t oval_int_cmp(const intmax_t state, const intmax_t syschar, oval_operation_t operation)
{
	switch (operation) {
	case OVAL_OPERATION_EQUALS:
		return state == syschar ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_NOT_EQUAL:
		return state != syschar ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_GREATER_THAN:
		return syschar > state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_GREATER_THAN_OR_EQUAL:
		return syschar >= state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_LESS_THAN:
		return syschar < state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_LESS_THAN_OR_EQUAL:
		return syschar <= state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_BITWISE_AND:
		return (syschar & state) == state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_BITWISE_OR:
		return (syschar | state) == state ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in integer evaluation: %d.", operation);
	}
	return OVAL_RESULT_ERROR;
}

static inline int cmp_float(double a, double b)
{
	double relative_err;
	int r;

	if (a == b)
		return 0;
	r = (a > b) ? 1 : -1;
	if (fabs(a) > fabs(b)) {
		relative_err = fabs((a - b) / a);
	} else {
		relative_err = fabs((a - b) / b);
	}
	if (relative_err <= 0.000000001)
		return 0;

	return r;
}

oval_result_t oval_float_cmp(const double state_val, const double sys_val, oval_operation_t operation)
{
	if (operation == OVAL_OPERATION_EQUALS) {
		return ((cmp_float(sys_val, state_val) == 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_NOT_EQUAL) {
		return ((cmp_float(sys_val, state_val) != 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN) {
		return ((cmp_float(sys_val, state_val) == 1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_GREATER_THAN_OR_EQUAL) {
		return ((cmp_float(sys_val, state_val) >= 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_LESS_THAN) {
		return ((cmp_float(sys_val, state_val) == -1) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else if (operation == OVAL_OPERATION_LESS_THAN_OR_EQUAL) {
		return ((cmp_float(sys_val, state_val) <= 0) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE);
	} else {
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in float evaluation: %s.", oval_operation_get_text(operation));
		return OVAL_RESULT_ERROR;
	}
}

static int istrcmp(const char *st1, const char *st2)
{
	if (st1 == NULL)
		st1 = "";
	if (st2 == NULL)
		st2 = "";
	return strcasecmp(st1, st2);
}

static oval_result_t strregcomp(const char *pattern, const char *test_str)
{
	int ret;
	oval_result_t result = OVAL_RESULT_ERROR;
	pcre *re;
	const char *err;
	int errofs;

	re = pcre_compile(pattern, PCRE_UTF8, &err, &errofs, NULL);
	if (re == NULL) {
		dE("Unable to compile regex pattern, "
			       "pcre_compile() returned error (offset: %d): '%s'.\n", errofs, err);
		return OVAL_RESULT_ERROR;
	}

	ret = pcre_exec(re, NULL, test_str, strlen(test_str), 0, 0, NULL, 0);
	if (ret > -1 ) {
		result = OVAL_RESULT_TRUE;
	} else if (ret == -1) {
		result = OVAL_RESULT_FALSE;
	} else {
		dE("Unable to match regex pattern, "
			       "pcre_exec() returned error: %d.\n", ret);
		result = OVAL_RESULT_ERROR;
	}

	pcre_free(re);
	return result;
}

oval_result_t oval_string_cmp(const char *state, const char *syschar, oval_operation_t operation)
{
	syschar = syschar ? syschar : "";
	switch (operation) {
	case OVAL_OPERATION_EQUALS:
		return oscap_strcmp(state, syschar) ? OVAL_RESULT_FALSE : OVAL_RESULT_TRUE;
	case OVAL_OPERATION_CASE_INSENSITIVE_EQUALS:
		return istrcmp(state, syschar) ? OVAL_RESULT_FALSE : OVAL_RESULT_TRUE;
	case OVAL_OPERATION_NOT_EQUAL:
		return oscap_strcmp(state, syschar) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
		return istrcmp(state, syschar) ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_PATTERN_MATCH:
		return strregcomp(state, syschar);
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in string evaluation: %d.", operation);
	}
	return OVAL_RESULT_ERROR;
}

oval_result_t oval_binary_cmp(const char *state, const char *syschar, oval_operation_t operation)
{
	// I'm going to use case insensitive compare here - don't know if it's necessary
	switch (operation) {
	case OVAL_OPERATION_EQUALS:
		return istrcmp(state, syschar) == 0 ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	case OVAL_OPERATION_NOT_EQUAL:
		return istrcmp(state, syschar) != 0 ? OVAL_RESULT_TRUE : OVAL_RESULT_FALSE;
	default:
		oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid type of operation in binary evaluation: %d.", operation);
	}
	return OVAL_RESULT_ERROR;
}
