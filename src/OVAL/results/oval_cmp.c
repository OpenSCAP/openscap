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
 *     David Niemoller <David.Niemoller@g2-inc.com>
 *     Peter Vrabec <pvrabec@redhat.com>
 *     Tomas Heinrich <theinric@redhat.com>
 *     Daniel Kopecek <dkopecek@redhat.com>
 *     Šimon Lukašík
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>

#include "oval_types.h"
#include "oval_system_characteristics.h"
#include "common/_error.h"
#include "common/debug_priv.h"

#include "oval_cmp_basic_impl.h"
#include "oval_cmp_evr_string_impl.h"
#include "oval_cmp_ip_address_impl.h"
#include "oval_cmp_impl.h"

__attribute__((nonnull(1,2))) static bool cstr_to_intmax(const char *cstr, intmax_t *result)
{
	char *endptr = NULL;

	errno = 0;
	*result = strtoimax(cstr, &endptr, 10);
	// Check for underflow/overflow, strtoimax sets ERANGE in such case
	if (errno == ERANGE) {
		return false;
	}
	// Check whether there were some digits in the string
	if (endptr == cstr) {
		errno = EINVAL;
		return false;
	}
	// Check whether the function used the whole string
	if (*endptr != '\0') {
		errno = EINVAL;
		return false;
	}
	return true;
}

__attribute__((nonnull(1,2))) static bool cstr_to_double(const char *cstr, double *result)
{
	char *endptr = NULL;

	errno = 0;
	*result = strtod(cstr, &endptr);
	// Check for underflow/overflow, strtoimax sets ERANGE in such case
	if (errno == ERANGE) {
		return false;
	}
	// Check whether there were some digits in the string
	if (endptr == cstr) {
		errno = EINVAL;
		return false;
	}
	// Check whether the function used the whole string
	if (*endptr != '\0') {
		errno = EINVAL;
		return false;
	}
	return true;
}

oval_result_t oval_str_cmp_str(char *state_data, oval_datatype_t state_data_type, const char *sys_data, oval_operation_t operation)
{
	// finally, we have gotten to the point of comparing system data with a state

	if (state_data_type == OVAL_DATATYPE_STRING) {
		return oval_string_cmp(state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_INTEGER) {
		intmax_t state_val, syschar_val;

		if (!cstr_to_intmax(state_data, &state_val)) {
			oscap_seterr(OSCAP_EFAMILY_OVAL,
				"Conversion of the string \"%s\" to an integer (%u bits) failed: %s",
				state_data, sizeof(intmax_t)*8, strerror(errno));
			return OVAL_RESULT_ERROR;
		}

		if (!cstr_to_intmax(sys_data, &syschar_val)) {
			oscap_seterr(OSCAP_EFAMILY_OVAL,
				"Conversion of the string \"%s\" to an integer (%u bits) failed: %s",
				sys_data, sizeof(intmax_t)*8, strerror(errno));
			return OVAL_RESULT_ERROR;
		}
		return oval_int_cmp(state_val, syschar_val, operation);
	} else if (state_data_type == OVAL_DATATYPE_FLOAT) {
		double state_val, sys_val;

		if (!cstr_to_double(state_data, &state_val)) {
			oscap_seterr(OSCAP_EFAMILY_OVAL,
				"Conversion of the string \"%s\" to a floating type (double) failed: %s",
				state_data, strerror(errno));
			return OVAL_RESULT_ERROR;
		}

		if (!cstr_to_double(sys_data, &sys_val)) {
			oscap_seterr(OSCAP_EFAMILY_OVAL,
				"Conversion of the string \"%s\" to a floating type (double) failed: %s",
				sys_data, strerror(errno));
			return OVAL_RESULT_ERROR;
		}
		return oval_float_cmp(state_val, sys_val, operation);
	} else if (state_data_type == OVAL_DATATYPE_BOOLEAN) {
		int state_int;
		int sys_int;
		state_int = (((strcmp(state_data, "true")) == 0) || ((strcmp(state_data, "1")) == 0)) ? 1 : 0;
		sys_int = (((strcmp(sys_data, "true")) == 0) || ((strcmp(sys_data, "1")) == 0)) ? 1 : 0;
		return oval_boolean_cmp(state_int, sys_int, operation);
	} else if (state_data_type == OVAL_DATATYPE_BINARY) {
		return oval_binary_cmp(state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_EVR_STRING) {
		return oval_evr_string_cmp(state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_VERSION) {
		return oval_versiontype_cmp(state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_IPV4ADDR) {
		return oval_ipaddr_cmp(AF_INET, state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_IPV6ADDR) {
		return oval_ipaddr_cmp(AF_INET6, state_data, sys_data, operation);
	} else if (state_data_type == OVAL_DATATYPE_FILESET_REVISION
			|| state_data_type == OVAL_DATATYPE_IOS_VERSION) {
		dW("Unsupported data type: %s.\n", oval_datatype_get_text(state_data_type));
		return OVAL_RESULT_NOT_EVALUATED;
	}

	oscap_seterr(OSCAP_EFAMILY_OVAL, "Invalid OVAL data type: %d.", state_data_type);
	return OVAL_RESULT_ERROR;
}

oval_result_t oval_ent_cmp_str(char *state_data, oval_datatype_t state_data_type, struct oval_sysent *sysent, oval_operation_t operation)
{
	const char *sys_data = oval_sysent_get_value(sysent);
	return oval_str_cmp_str(state_data, state_data_type, sys_data, operation);
}
