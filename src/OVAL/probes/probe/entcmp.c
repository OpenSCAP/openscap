/**
 * @file   entcmp.c
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
#ifdef OS_WINDOWS
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include "common/debug_priv.h"
#include "entcmp.h"
#include "../_probe-api.h"
#include "../../results/oval_cmp_basic_impl.h"
#include "../../results/oval_cmp_evr_string_impl.h"
#include "../../results/oval_cmp_ip_address_impl.h"

oval_result_t probe_ent_cmp_binary(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	result = oval_binary_cmp(s1, s2, op);

        free(s1);
        free(s2);

	return result;
}

oval_result_t probe_ent_cmp_bool(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	int v1, v2;

	v1 = SEXP_number_geti_32(val1);
	v2 = SEXP_number_geti_32(val2);

	return oval_boolean_cmp(v1, v2, op);
}

oval_result_t probe_ent_cmp_evr(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s1 = SEXP_string_cstr(val1);
	char *s2 = SEXP_string_cstr(val2);

	result = oval_evr_string_cmp(s1, s2, op);

	free(s1);
	free(s2);
	return result;
}

oval_result_t probe_ent_cmp_debian_evr(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	//TODO: implement Debian's epoch-version-release comparing algorithm
	// it is different algorithm than RPM algorithm
	dW("Using RPM algorithm to compare epoch, version and release.");
	return probe_ent_cmp_evr(val1, val2, op);
}

oval_result_t probe_ent_cmp_filesetrev(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_t probe_ent_cmp_float(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	double v1, v2;

	v1 = SEXP_number_getf(val1);
	v2 = SEXP_number_getf(val2);

	return oval_float_cmp(v1, v2, op);
}

oval_result_t probe_ent_cmp_int(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	int64_t v1, v2;

	v1 = SEXP_number_geti_64(val1);
	v2 = SEXP_number_geti_64(val2);

	result = oval_int_cmp(v1, v2, op);

	return result;
}

oval_result_t probe_ent_cmp_ios(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_t probe_ent_cmp_version(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	char *state_version = SEXP_string_cstr(val1);
	char *sys_version = SEXP_string_cstr(val2);

	oval_result_t result = oval_versiontype_cmp(state_version, sys_version, op);

	free(state_version);
	free(sys_version);
	return result;
}

oval_result_t probe_ent_cmp_string(SEXP_t * val1, SEXP_t * val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	result = oval_string_cmp(s1, s2, op);

        free(s1);
        free(s2);

	return result;
}

static oval_result_t probe_ent_cmp_ipaddr(int af, SEXP_t *val1, SEXP_t *val2, oval_operation_t op)
{
	oval_result_t result = OVAL_RESULT_ERROR;
	char *addr1 = SEXP_string_cstr(val1);
	char *addr2 = SEXP_string_cstr(val2);

	result = oval_ipaddr_cmp(af, addr1, addr2, op);

	free(addr1);
	free(addr2);
	return result;
}

static inline oval_result_t probe_ent_cmp_single(SEXP_t *state_ent, oval_datatype_t state_data_type, SEXP_t *sysent, oval_operation_t op)
{
	// This compares a single entity of a state with a single entity of a sysitem
	// This is very similar to oval_ent_cmp_str, we hope to get a rid of this soon

	switch (state_data_type) {
	case OVAL_DATATYPE_BINARY:
		return probe_ent_cmp_binary(state_ent, sysent, op);
	case OVAL_DATATYPE_BOOLEAN:
		return probe_ent_cmp_bool(state_ent, sysent, op);
	case OVAL_DATATYPE_EVR_STRING:
		return probe_ent_cmp_evr(state_ent, sysent, op);
	case OVAL_DATATYPE_DEBIAN_EVR_STRING:
		return probe_ent_cmp_debian_evr(state_ent, sysent, op);
	case OVAL_DATATYPE_FILESET_REVISION:
		return probe_ent_cmp_filesetrev(state_ent, sysent, op);
	case OVAL_DATATYPE_FLOAT:
		return probe_ent_cmp_float(state_ent, sysent, op);
	case OVAL_DATATYPE_IOS_VERSION:
		return probe_ent_cmp_ios(state_ent, sysent, op);
	case OVAL_DATATYPE_VERSION:
		return probe_ent_cmp_version(state_ent, sysent, op);
	case OVAL_DATATYPE_INTEGER:
		return probe_ent_cmp_int(state_ent, sysent, op);
	case OVAL_DATATYPE_STRING:
		return probe_ent_cmp_string(state_ent, sysent, op);
	case OVAL_DATATYPE_IPV4ADDR:
		return probe_ent_cmp_ipaddr(AF_INET, state_ent, sysent, op);
	case OVAL_DATATYPE_IPV6ADDR:
		return probe_ent_cmp_ipaddr(AF_INET6, state_ent, sysent, op);
	default:
		dI("Unexpected data type: %d", state_data_type);
		break;
	}

	return OVAL_RESULT_ERROR;
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
			dI("Types of values to compare don't match: val1: %d, val2: %d",
			   SEXP_typeof(val1), SEXP_typeof(val2));

                        SEXP_free(vals);
                        SEXP_free(val1);
                        SEXP_free(res_lst);

			return OVAL_RESULT_ERROR;
		}

		ores = probe_ent_cmp_single(val1, dtype, val2, op);

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
		bool matched;

		char *sname = probe_ent_getname(ste_rf);
		itm_res = SEXP_list_new(NULL);
		matched = false;

		SEXP_list_foreach(itm_rf, itm_record_fields) {
			char *iname = probe_ent_getname(itm_rf);
			if (strcmp(sname, iname)) {
				free(iname);
				continue;
			}
			free(iname);
			matched = true;

			res = probe_entste_cmp(ste_rf, itm_rf);
			/* todo: _oval_result_to_sexp() */
			stmp = SEXP_number_newu(res);
			SEXP_list_add(itm_res, stmp);
			SEXP_free(stmp);
		}

		free(sname);

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
		dI("valcnt == 0.");
		return OVAL_RESULT_FALSE;
	}

	ores = probe_ent_cmp(ent_obj, val);
#if defined(OSCAP_VERBOSE_DEBUG)
	dI("Result of entobj comparison: %s.", oval_result_get_text(ores));
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
		   "Converted to check='none satisfy'.");
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

/// @}
