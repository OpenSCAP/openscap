#include <config.h>
#include <sexp-types.h>
#include <sexp-manip.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#if defined USE_REGEX_PCRE
#include <pcre.h>
#elif defined USE_REGEX_POSIX
#include <regex.h>
#endif

#include <probe.h>

oval_result_enum SEXP_OVALent_cmp_binary(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	switch (op) {
	case OPERATION_EQUALS:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_NOT_EQUAL:
		if (strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		_D("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_enum SEXP_OVALent_cmp_bool(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	int v1, v2;

	v1 = SEXP_number_geti_32 (val1);
	v2 = SEXP_number_geti_32 (val2);
        
	switch (op) {
	case OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		_D("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_enum SEXP_OVALent_cmp_evr(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_enum SEXP_OVALent_cmp_filesetrev(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

oval_result_enum SEXP_OVALent_cmp_float(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	double v1, v2;

	v1 = SEXP_number_getf(val1);
	v2 = SEXP_number_getf(val2);

	switch (op) {
	case OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_GREATER_THAN:
		if (v1 < v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_LESS_THAN:
		if (v1 > v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_GREATER_THAN_OR_EQUAL:
		if (v1 <= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_LESS_THAN_OR_EQUAL:
		if (v1 >= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		_D("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_enum SEXP_OVALent_cmp_int(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	int v1, v2;

	v1 = SEXP_number_geti_32 (val1);
	v2 = SEXP_number_geti_32 (val2);

	switch (op) {
	case OPERATION_EQUALS:
		if (v1 == v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_NOT_EQUAL:
		if (v1 != v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_GREATER_THAN:
		if (v1 < v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_LESS_THAN:
		if (v1 > v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_GREATER_THAN_OR_EQUAL:
		if (v1 <= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_LESS_THAN_OR_EQUAL:
		if (v1 >= v2)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_BITWISE_AND:
		if ((v1 && v2) == v1)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_BITWISE_OR:
		if ((v1 || v2) == v1)
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	default:
		_D("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_enum SEXP_OVALent_cmp_ios(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;

	// todo:

	return result;
}

static SEXP_t *version_parser(char *version) {
	long long int token;
	size_t len, nbr_len;
	char *s, *v_dup = NULL;
	const char *ac = "0123456789";
	SEXP_t *version_tokens = NULL;

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
		SEXP_list_add(version_tokens, SEXP_number_newi_64 (token));
		s += nbr_len + 1;
		len -= nbr_len + 1;
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

oval_result_enum SEXP_OVALent_cmp_version(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	SEXP_t *v1_tkns = NULL, *v2_tkns = NULL, *stmp;
	char *vtmp;
	size_t len, i;
	int lendif;
	long long int v1, v2;

	switch (op) {
	case OPERATION_EQUALS:
	case OPERATION_GREATER_THAN_OR_EQUAL:
	case OPERATION_LESS_THAN_OR_EQUAL:
		result = OVAL_RESULT_TRUE;
		break;
	case OPERATION_NOT_EQUAL:
	case OPERATION_GREATER_THAN:
	case OPERATION_LESS_THAN:
		result = OVAL_RESULT_FALSE;
		break;
	default:
		_D("Unexpected compare operation: %d\n", op);
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
		lendif = - lendif;
		stmp = v1_tkns;
	} else if (lendif > 0) {
		stmp = v2_tkns;
	}
	for (; lendif > 0; --lendif) {
		SEXP_list_add(stmp, SEXP_number_newi_64 (0));
	}

	len = SEXP_list_length(v1_tkns);
	for (i = 1; i <= len; ++i) {
		v1 = SEXP_number_geti_64 (SEXP_list_nth(v1_tkns, i));
		v2 = SEXP_number_geti_64 (SEXP_list_nth(v2_tkns, i));

		if (op == OPERATION_EQUALS) {
			if (v1 != v2) {
				result = OVAL_RESULT_FALSE;
				break;
			}
		} else if (op == OPERATION_NOT_EQUAL) {
			if (v1 != v2) {
				result = OVAL_RESULT_TRUE;
				break;
			}
		} else if ((op == OPERATION_GREATER_THAN) ||
			   (op == OPERATION_GREATER_THAN_OR_EQUAL)) {
			if (v1 < v2) {
				result = OVAL_RESULT_TRUE;
				break;
			} else if (v1 > v2) {
				result = OVAL_RESULT_FALSE;
				break;
			}
		} else if ((op == OPERATION_LESS_THAN) ||
			   (op == OPERATION_LESS_THAN_OR_EQUAL)) {
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

oval_result_enum SEXP_OVALent_cmp_string(SEXP_t *val1, SEXP_t *val2, oval_operation_enum op)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	char *s1, *s2;

	s1 = SEXP_string_cstr(val1);
	s2 = SEXP_string_cstr(val2);

	switch (op) {
	case OPERATION_EQUALS:
		if (!strcmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_NOT_EQUAL:
		if (strcmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_CASE_INSENSITIVE_EQUALS:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_CASE_INSENSITIVE_NOT_EQUAL:
		if (!strcasecmp(s1, s2))
			result = OVAL_RESULT_TRUE;
		else
			result = OVAL_RESULT_FALSE;
		break;
	case OPERATION_PATTERN_MATCH:
		{
#if defined USE_REGEX_PCRE
			int erroffset = -1, rc;
			const char *error;
			pcre *re = NULL;

			re = pcre_compile(s1, PCRE_UTF8, &error, &erroffset, NULL);
			if (re == NULL) {
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
		_D("Unexpected compare operation: %d\n", op);
	}

	return result;
}

oval_result_enum SEXP_OVALent_cmp(SEXP_t *ent, SEXP_t *val2)
{
	oval_result_enum result = OVAL_RESULT_ERROR;
	oval_operation_enum op;
	oval_datatype_enum dtype;
	SEXP_t *op_sexp, *val1;

	val1 = probe_ent_getval (ent, 1);
	if (SEXP_typeof(val1) != SEXP_typeof(val2)) {
		_D("Types of values to compare don't match: val1: %d, val2: %d\n",
		   SEXP_typeof(val1), SEXP_typeof(val2));
		return OVAL_RESULT_ERROR;
	}

	op_sexp = probe_ent_getattrval (ent, "operation");
	if (op_sexp == NULL)
		op = OPERATION_EQUALS;
	else
		op = SEXP_number_geti_32 (op_sexp);

	dtype = probe_ent_getdatatype(ent, 1);

	switch (dtype) {
	case OVAL_DATATYPE_BINARY:
		result = SEXP_OVALent_cmp_binary(val1, val2, op);
		break;
	case OVAL_DATATYPE_BOOLEAN:
		result = SEXP_OVALent_cmp_bool(val1, val2, op);
		break;
	case OVAL_DATATYPE_EVR_STRING:
		result = SEXP_OVALent_cmp_evr(val1, val2, op);
		break;
	case OVAL_DATATYPE_FILESET_REVISTION:
		result = SEXP_OVALent_cmp_filesetrev(val1, val2, op);
		break;
	case OVAL_DATATYPE_FLOAT:
		result = SEXP_OVALent_cmp_float(val1, val2, op);
		break;
	case OVAL_DATATYPE_IOS_VERSION:
		result = SEXP_OVALent_cmp_ios(val1, val2, op);
		break;
	case OVAL_DATATYPE_VERSION:
		result = SEXP_OVALent_cmp_version(val1, val2, op);
		break;
	case OVAL_DATATYPE_INTEGER:
		result = SEXP_OVALent_cmp_int(val1, val2, op);
		break;
	case OVAL_DATATYPE_STRING:
		result = SEXP_OVALent_cmp_string(val1, val2, op);
		break;
	default:
		_D("Unexpected data type: %d\n", dtype);
	}

	return result;
}

oval_result_enum SEXP_OVALentste_cmp(SEXP_t *ent_ste, SEXP_t *ent_itm)
{
	oval_syschar_status_enum item_status;
	SEXP_t *val2;

	item_status = probe_ent_getstatus(ent_itm);
	switch(item_status) {
	case OVAL_STATUS_DOESNOTEXIST:
		return OVAL_RESULT_FALSE;
	case OVAL_STATUS_ERROR:
	case OVAL_STATUS_NOTCOLLECTED:
		return OVAL_RESULT_ERROR;
	default:
		break;
	}

	if (probe_ent_getdatatype(ent_ste, 1) !=
	    probe_ent_getdatatype(ent_itm, 1)) {
		return OVAL_RESULT_ERROR;
	}

	val2 = probe_ent_getval(ent_itm, 1);

	return SEXP_OVALent_cmp(ent_ste, val2);
}

oval_result_enum SEXP_OVALentobj_cmp(SEXP_t *ent_obj, SEXP_t *val)
{
	return SEXP_OVALent_cmp(ent_obj, val);
}

struct _oresults {
	int     true_cnt,
		false_cnt,
		unknown_cnt,
		error_cnt,
		noteval_cnt,
		notappl_cnt;
};

static int results_parser(SEXP_t *res_lst, struct _oresults *ores)
{
	oval_result_enum r;
	SEXP_t *res;

	memset(ores, 0, sizeof (struct _oresults));

	SEXP_list_foreach (res, res_lst) {
		r = SEXP_number_geti_32 (res);
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
		}
	}

	return 0;
}

oval_result_enum SEXP_OVALent_result_bychk(SEXP_t *res_lst, oval_check_enum check)
{
	oval_result_enum result = OVAL_RESULT_UNKNOWN;
	struct _oresults ores;

	if (SEXP_list_length(res_lst) == 0)
		return OVAL_RESULT_UNKNOWN;

	if (results_parser(res_lst, &ores) != 0) {
		return OVAL_RESULT_ERROR;
	}

	if (ores.notappl_cnt > 0 &&
	    ores.noteval_cnt == 0 &&
	    ores.false_cnt == 0 &&
	    ores.error_cnt == 0 &&
	    ores.unknown_cnt == 0 &&
	    ores.true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch (check) {
	case OVAL_CHECK_ALL:
		if (ores.true_cnt > 0 &&
		    ores.false_cnt == 0 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_AT_LEAST_ONE:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0 &&
			   ores.true_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OVAL_CHECK_NONE_SATISFY:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores.false_cnt > 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt == 0 &&
			   ores.true_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		}
		break;
	case OVAL_CHECK_ONLY_ONE:
		if (ores.true_cnt == 1 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt > 1) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt < 2 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt < 2 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt < 2 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		} else if (ores.true_cnt != 1 &&
			   ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		}
		break;
	default:
		break;
	}

	return result;
}

oval_result_enum SEXP_OVALent_result_byopr(SEXP_t *res_lst, oval_operator_enum operator)
{
	oval_result_enum result = OVAL_RESULT_UNKNOWN;
	struct _oresults ores;

	if (SEXP_list_length(res_lst) == 0)
		return OVAL_RESULT_UNKNOWN;

	if (results_parser(res_lst, &ores) != 0) {
		return OVAL_RESULT_ERROR;
	}

	if (ores.notappl_cnt > 0 &&
	    ores.noteval_cnt == 0 &&
	    ores.false_cnt == 0 &&
	    ores.error_cnt == 0 &&
	    ores.unknown_cnt == 0 &&
	    ores.true_cnt == 0)
		return OVAL_RESULT_NOT_APPLICABLE;

	switch(operator) {
	case OPERATOR_AND:
		if (ores.true_cnt > 0 &&
		    ores.false_cnt == 0 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.false_cnt > 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.false_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OPERATOR_ONE:
		if (ores.true_cnt == 1 &&
		    ores.false_cnt >= 0 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0 &&
		    ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt >= 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt >= 0 &&
			   ores.unknown_cnt >= 0 &&
			   ores.noteval_cnt >= 0 &&
			   ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt == 0 &&
			   ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt > 0 &&
			   ores.unknown_cnt >= 0 &&
			   ores.noteval_cnt >= 0 &&
			   ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt >= 1 &&
			   ores.noteval_cnt >= 0 &&
			   ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt < 2 &&
			   ores.false_cnt >= 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0 &&
			   ores.notappl_cnt >= 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OPERATOR_OR:
		if (ores.true_cnt > 0) {
			result = OVAL_RESULT_TRUE;
		} else if (ores.true_cnt == 0 &&
			   ores.false_cnt > 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.true_cnt == 0 &&
			   ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	case OPERATOR_XOR:
		if ((ores.true_cnt % 2) == 1 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0) {
			result = OVAL_RESULT_TRUE;
		} else if ((ores.true_cnt % 2) == 0 &&
		    ores.error_cnt == 0 &&
		    ores.unknown_cnt == 0 &&
		    ores.noteval_cnt == 0) {
			result = OVAL_RESULT_FALSE;
		} else if (ores.error_cnt > 0) {
			result = OVAL_RESULT_ERROR;
		} else if (ores.error_cnt == 0 &&
			   ores.unknown_cnt > 0) {
			result = OVAL_RESULT_UNKNOWN;
		} else if (ores.error_cnt == 0 &&
			   ores.unknown_cnt == 0 &&
			   ores.noteval_cnt > 0) {
			result = OVAL_RESULT_NOT_EVALUATED;
		}
		break;
	default:
		break;
	}

	return result;
}
