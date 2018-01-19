/**
 * @file oval_cmp_evr_string_impl.h
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2009--2013 Red Hat Inc., Durham, North Carolina.
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
 *     Šimon Lukašík
 */

#ifndef OSCAP_OVAL_EVR_STRING_IMPL_H_
#define OSCAP_OVAL_EVR_STRING_IMPL_H_

#include "../common/util.h"

#include "oval_definitions.h"
#include "oval_types.h"


/**
 * Compare two EVR (Epoch:Version-Release) strings. The format of input types shall
 * conform to EntityStateEVRStringType. Comparisons involving this datatype follow
 * the algorithm of librpm's rpmvercmp() function.
 * @param state evr_string as defined by state element
 * @param sys evr_string as captured from system (from syschar object)
 * @param operation type of comparison operation
 * @returns result of comparison
 */
oval_result_t oval_evr_string_cmp(const char *state, const char *sys, oval_operation_t operation);

oval_result_t oval_versiontype_cmp(const char *state, const char *syschar, oval_operation_t operation);


#endif
