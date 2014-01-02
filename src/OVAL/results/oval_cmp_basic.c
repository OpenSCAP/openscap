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

#include "oval_types.h"
#include "common/_error.h"
#include "oval_cmp_basic_impl.h"

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

