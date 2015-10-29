/**
 * @file   entcmp.h
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

#ifndef PROBE_ENTCMP_H
#define PROBE_ENTCMP_H

#include <seap.h>
#include <stdarg.h>
#include "oval_definitions.h"
#include "oval_results.h"

/**
 * Compute the overall result.
 * Compute the overall result from a results vector and a check enumeration parameter.
 * @param res_lst the results vector
 * @param check the check enumeration value
 */
oval_result_t probe_ent_result_bychk(SEXP_t * res_lst, oval_check_t check);

/**
 * Compute the overall result.
 * Compute the overall result from a results vector and a operator enumeration parameter.
 * @param res_lst the results vector
 * @param check the operator enumeration value
 */
oval_result_t probe_ent_result_byopr(SEXP_t * res_lst, oval_operator_t operator);

/**
 * Compare object entity's content with a value.
 * The result depends on the operation attribute and
 * a possible var_check attribute.
 * @param ent_obj object entity
 * @param val raw value
 */
oval_result_t probe_entobj_cmp(SEXP_t * ent_obj, SEXP_t * val);

/**
 * Compare state entity's content with a item entity's value.
 * The result depends on the operation attribute,
 * a possible var_check attribute and item entity's status.
 * @param ent_ste state entity
 * @param ent_itm item entity
 */
oval_result_t probe_entste_cmp(SEXP_t * ent_ste, SEXP_t * ent_itm);

/**
 * Compare two binary values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_binary(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two boolean values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_bool(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two epoch-version-release values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_evr(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two Debian epoch-version-release values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_debian_evr(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two  values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_filesetrev(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two floating point values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_float(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two integer values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_int(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two ios values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_ios(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two version values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_version(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

/**
 * Compare two string values.
 * The operation to use is specified by the operation enumeration value.
 * @param val1 the first value
 * @param val2 the second value
 * @param op the desired operation
 */
oval_result_t probe_ent_cmp_string(SEXP_t * val1, SEXP_t * val2, oval_operation_t op);

#endif
/// @}
