/**
 * @file   probe-main.c
 * @brief  common probe entry point
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 * @author "Tomas Heinrich" <theinric@redhat.com>
 */

/*
 * Copyright 2009-2010 Red Hat Inc., Durham, North Carolina.
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
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <alloc.h>
#include <assume.h>
#include <bfind.h>

#include "_probe-api.h"
#include "probe-entcmp.h"
#include "encache.h"

#ifndef _A
#define _A(x) assert(x)
#endif

#define PROBE_SIGEXIT_CLEAN   1
#define PROBE_SIGEXIT_UNCLEAN 2
#define PROBE_SIGEXIT_ABORT   3

static int probe_opthandler_varref(int, va_list);
static int probe_opthandler_rcache(int, va_list);

probe_option_t OSCAP_GSYM(options)[] = {
	{ PROBE_VARREF_HANDLING, &probe_opthandler_varref },
	{ PROBE_RESULT_CACHING,  &probe_opthandler_rcache }
};

#define OPTIONS_COUNT (sizeof OSCAP_GSYM(options)/sizeof(probe_option_t))

bool probe_handle_varref_cstr(const char *ent_name);
bool probe_handle_varref_sexp(const SEXP_t *ent_name);

volatile int OSCAP_GSYM(sigexit)   = 0;    /**< signal exit flag */
SEAP_CTX_t  *OSCAP_GSYM(ctx)       = NULL; /**< SEAP context */
int          OSCAP_GSYM(sd)        = -1;   /**< SEAP descriptor */
pcache_t    *OSCAP_GSYM(pcache)    = NULL; /**< probe item cache */
void        *OSCAP_GSYM(probe_arg) = NULL; /**< pointer for probe_main, provided by probe_init */
encache_t   *OSCAP_GSYM(encache)   = NULL; /**< element name cache */
struct id_desc_t OSCAP_GSYM(id_desc);

bool   OSCAP_GSYM(varref_handling) = true;
char **OSCAP_GSYM(no_varref_ents) = NULL;
size_t OSCAP_GSYM(no_varref_ents_cnt) = 0;

void *probe_worker(void *arg);

#define MAX_EVAL_DEPTH 8 /**< maximum recursion depth for set evaluation */

/**
 * Fetch states from the library. This function performs the state fetch operation
 * that is used by the set evaluation function to fetch states that aren't available
 * in the probe cache. The operation is implemented using a remote synchronous SEAP
 * command. The fetched states are added to the probe cache by this function.
 *
 * @param id_list list of requested state ids
 * @retval list list of fetched states, the possition in the list corresponds to the
 *              possition in the id_list
 * @retval NULL on failure or when some of the ids do not refer to existing states in
 *              the associated OVAL document
 */
static SEXP_t *probe_ste_fetch(SEXP_t * id_list)
{
	SEXP_t *res, *ste, *id;
	uint32_t i_len, r_len;

	_LOGCALL_;

	i_len = SEXP_list_length(id_list);

	if (i_len == 0)
		return SEXP_list_new(NULL);

	res = SEAP_cmd_exec(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), 0, PROBECMD_STE_FETCH, id_list, SEAP_CMDTYPE_SYNC, NULL, NULL);

	r_len = SEXP_list_length(res);

	if (i_len != r_len) {
		SEXP_free(res);
		return (NULL);
	}

	for (; i_len > 0; --i_len) {
		ste = SEXP_list_nth(res, i_len);
		id = SEXP_list_nth(id_list, i_len);

		_A(id != NULL);
		_A(ste != NULL);

		if (pcache_sexp_add(OSCAP_GSYM(pcache), id, ste) != 0) {

			SEXP_free(res);
			SEXP_free(ste);
			SEXP_free(id);

			return (NULL);
		}

		SEXP_free(ste);
		SEXP_free(id);
	}

	return (res);
}

/**
 * Evaluate an OVAL object identified by its id. Using a remote
 * synchronous SEAP command, this function executes evaluation of an
 * OVAL object which results weren't found in the probe cache. This
 * indirectly spawns a new thread in the probe process which evaluates
 * the object and stores the result in the probe cache. That result is
 * not send to the library because it doesn't know how to handle
 * it. Instead, the result is fetched by this function from the cache
 * and returned to the caller.
 * @param id the id of the OVAL object to be evaluated
 * @return the result of the evaluation of the object or NULL on failure
 */
static SEXP_t *probe_obj_eval(SEXP_t * id)
{
	SEXP_t *res, *rid;

	_LOGCALL_;

	res = SEAP_cmd_exec(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), 0, PROBECMD_OBJ_EVAL, id, SEAP_CMDTYPE_SYNC, NULL, NULL);

	rid = SEXP_list_first(res);
	assume_r(SEXP_string_cmp(id, rid) == 0, NULL);
	SEXP_vfree(res, rid, NULL);

	return pcache_sexp_get(OSCAP_GSYM(pcache), id);
}

SEXP_t *probe_prepare_filters(SEXP_t *obj)
{
	SEXP_t *filters;
	int i;

	filters = SEXP_list_new(NULL);

	for (i = 1; ; ++i) {
		SEXP_t *of, *f, *ste, *ste_id, *act;

		of = probe_obj_getent(obj, "filter", i);
		if (of == NULL)
			break;

		act = probe_ent_getattrval(of, "action");
		ste_id = probe_ent_getval(of);
		ste = pcache_sexp_get(OSCAP_GSYM(pcache), ste_id);
		if (ste == NULL) {
			SEXP_t *r0, *r1;

			r0 = SEXP_list_new(ste_id, NULL);
			r1 = probe_ste_fetch(r0);
			ste = SEXP_list_first(r1);
			SEXP_vfree(r0, r1, NULL);
		}
		SEXP_vfree(of, ste_id, NULL);

		f = SEXP_list_new(act, ste, NULL);
		SEXP_list_add(filters, f);
		SEXP_vfree(act, ste, f, NULL);
	}

	return filters;
}

bool probe_item_filtered(SEXP_t *item, SEXP_t *filters)
{
	bool filtered = false;
	SEXP_t *filter, *ste;

	SEXP_list_foreach(filter, filters) {
		SEXP_t *felm, *ste_res, *r0;
		oval_result_t ores;
		oval_operator_t oopr;
		oval_filter_action_t ofact;

		r0 = SEXP_list_first(filter);
		ofact = SEXP_number_getu(r0);
		SEXP_free(r0);
		ste = SEXP_list_nth(filter, 2);
		ste_res = SEXP_list_new(NULL);

		SEXP_sublist_foreach(felm, ste, 2, SEXP_LIST_END) {
			SEXP_t *ielm, *elm_res;
			char *elm_name;
			oval_check_t ochk;
			int i;

			elm_res = SEXP_list_new(NULL);
			elm_name = probe_ent_getname(felm);

			for (i = 1;; ++i) {
				ielm = probe_obj_getent(item, elm_name, i);

				if (ielm == NULL)
					break;

				ores = probe_entste_cmp(felm, ielm);
				SEXP_list_add(elm_res, r0 = SEXP_number_newi_32(ores));

				SEXP_free(ielm);
				SEXP_free(r0);
			}

			oscap_free(elm_name);
			r0 = probe_ent_getattrval(felm, "entity_check");

			if (r0 == NULL)
				ochk = OVAL_CHECK_ALL;
			else
				ochk = SEXP_number_geti_32(r0);

			SEXP_free(r0);

			ores = probe_ent_result_bychk(elm_res, ochk);
			SEXP_list_add(ste_res, r0 = SEXP_number_newi_32(ores));
			SEXP_free(r0);
			SEXP_free(elm_res);
		}

		r0 = probe_ent_getattrval(ste, "operator");
		if (r0 == NULL)
			oopr = OVAL_OPERATOR_AND;
		else
			oopr = SEXP_number_geti_32(r0);
		ores = probe_ent_result_byopr(ste_res, oopr);
		SEXP_vfree(ste, ste_res, r0, NULL);

		if ((ores == OVAL_RESULT_TRUE && ofact == OVAL_FILTER_ACTION_EXCLUDE)
		    || (ores = OVAL_RESULT_FALSE && ofact == OVAL_FILTER_ACTION_INCLUDE)) {
			filtered = true;
			SEXP_free(filter);
			break;
		}
	}

	return filtered;
}

/**
 * Combine two collections of items using an operation.
 * @param cobj1 item collection
 * @param cobj2 item collection
 * @param op operation
 * @return the result of the operation
 */
static SEXP_t *probe_set_combine(SEXP_t *cobj1, SEXP_t *cobj2, oval_setobject_operation_t op)
{
	bool append;
	SEXP_t *res_items, *item1, *item2, *id1, *id2;
	SEXP_t *res_cobj, *item_lst1, *item_lst2;
	oval_syschar_collection_flag_t flag1, flag2, res_flag;

	_LOGCALL_;

	if (cobj1 == NULL)
		return SEXP_ref(cobj2);
	if (cobj2 == NULL)
		return SEXP_ref(cobj1);

	item_lst1 = probe_cobj_get_items(cobj1);
	item_lst2 = probe_cobj_get_items(cobj2);
	flag1 = probe_cobj_get_flag(cobj1);
	flag2 = probe_cobj_get_flag(cobj2);

	res_flag = probe_cobj_combine_flags(flag1, flag2, op);
	res_items = SEXP_list_new(NULL);

	switch (op) {
	case OVAL_SET_OPERATION_INTERSECTION:
		SEXP_list_foreach(item1, item_lst1) {
			id1 = probe_obj_getentval(item1, "id", 1);
			append = false;

			SEXP_list_foreach(item2, item_lst2) {
				id2 = probe_obj_getentval(item2, "id", 1);

				if (!SEXP_string_cmp(id1, id2)) {
					append = true;

					SEXP_free(id2);
					SEXP_free(item2);

					break;
				}

				SEXP_free(id2);
			}

			if (append) {
				SEXP_list_add(res_items, item1);
			}

			SEXP_free(id1);
		}
		break;
	case OVAL_SET_OPERATION_UNION:
		{
			SEXP_t *lj;

			lj = SEXP_list_join(res_items, item_lst2);
			SEXP_free(res_items);
			res_items = lj;
			/* fall through */
		}
	case OVAL_SET_OPERATION_COMPLEMENT:
		SEXP_list_foreach(item1, item_lst1) {
			id1 = probe_obj_getentval(item1, "id", 1);
			append = true;

			SEXP_list_foreach(item2, item_lst2) {
				id2 = probe_obj_getentval(item2, "id", 1);

				if (!SEXP_string_cmp(id1, id2)) {
					append = false;

					SEXP_free(id2);
					SEXP_free(item2);

					break;
				}

				SEXP_free(id2);
			}

			if (append) {
				SEXP_list_add(res_items, item1);
			}

			SEXP_free(id1);
		}
		break;
	default:
		{
			char msg[40];
			SEXP_t *r0, *r1;

			snprintf(msg, sizeof (msg), "Unexpected set operation: %d.\n", op);
			_D(msg);
			r0 = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, msg);
			r1 = SEXP_list_new(r0, NULL);
			res_cobj = probe_cobj_new(SYSCHAR_FLAG_ERROR, r1, NULL);
			SEXP_vfree(item_lst1, item_lst2, res_items, r0, r1, NULL);
			return res_cobj;
		}
	}

	/*
	 * If the collected information is complete but all the items are
	 * removed, the flag is set to SYSCHAR_FLAG_DOES_NOT_EXIST
	 */
	if (res_flag == SYSCHAR_FLAG_COMPLETE
	    && SEXP_list_length(res_items) == 0)
		res_flag = SYSCHAR_FLAG_DOES_NOT_EXIST;

	res_cobj = probe_cobj_new(res_flag, NULL, res_items);
	SEXP_vfree(item_lst1, item_lst2, res_items, NULL);

	// todo: variables

	return res_cobj;
}

/**
 * Apply a set of filters to a collected object.
 * @param cobj item collection
 * @param filter set (list) of filters
 * @return collection of items without items that match any of the filters in the input set
 */
static SEXP_t *probe_set_apply_filters(SEXP_t *cobj, SEXP_t *filters)
{
	SEXP_t *result_items, *items, *item;
	oval_syschar_status_t item_status;
	oval_syschar_collection_flag_t flag;

	_LOGCALL_;

	result_items = SEXP_list_new(NULL);
	flag = probe_cobj_get_flag(cobj);
	items = probe_cobj_get_items(cobj);

	SEXP_list_foreach(item, items) {
		item_status = probe_ent_getstatus(item);

		switch (item_status) {
		case OVAL_STATUS_DOESNOTEXIST:
			continue;
		case OVAL_STATUS_ERROR:
			break;
		case OVAL_STATUS_NOTCOLLECTED:
			{
				char msg[50];
				SEXP_t *r0, *r1;

				snprintf(msg, sizeof (msg), "Supplied item has an invalid status: %d\n", item_status);
				_D(msg);
				r0 = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, msg);
				r1 = SEXP_list_new(r0, NULL);
				cobj = probe_cobj_new(SYSCHAR_FLAG_ERROR, r1, NULL);
				SEXP_vfree(items, result_items, r0, r1, NULL);
				return cobj;
			}
		default:
			break;
		}

		if (!probe_item_filtered(item, filters)) {
			SEXP_list_add(result_items, item);
		}
	}

	/*
	 * If the collected information is complete but all the items are
	 * filtered out, the flag is set to SYSCHAR_FLAG_DOES_NOT_EXIST
	 */
	if (flag == SYSCHAR_FLAG_COMPLETE
	    && SEXP_list_length(result_items) == 0)
		flag = SYSCHAR_FLAG_DOES_NOT_EXIST;

	cobj = probe_cobj_new(flag, NULL, result_items);
	SEXP_vfree(items, result_items, NULL);

	return cobj;
}

/**
 * Evaluate a set. This function takes care of evaluating a set of either two other sets
 * or an object and 0..n filters. Objects are evaluated using the probe_obj_eval function
 * and filters are fetched using the probe_ste_fetch function.
 * @param set the set to be evaluated
 * @param depth maximum recursion depth
 * @return the result of the evaluation or NULL on failure
 */
static SEXP_t *probe_set_eval(SEXP_t * set, size_t depth)
{
	SEXP_t *filters_u, *filters_a, *filters_req;

	SEXP_t *s_subset[2];
	size_t s_subset_i;
	SEXP_t *o_subset[2];
	size_t o_subset_i;

	SEXP_t *member;
	char member_name[24];

	SEXP_t *op_val;
	int op_num;

	SEXP_t *result;
	char msg[70] = "probe_set_eval: Unspecified error condition.";
	SEXP_t *r0, *r1;

	_LOGCALL_;

	if (depth > MAX_EVAL_DEPTH) {
		snprintf(msg, sizeof (msg), "probe_set_eval: Too many levels: max=%zu.\n", (size_t) MAX_EVAL_DEPTH);
		_D(msg);
#ifndef NDEBUG
		abort();
#endif
		r0 = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, msg);
		r1 = SEXP_list_new(r0, NULL);
		result = probe_cobj_new(SYSCHAR_FLAG_ERROR, r1, NULL);
		SEXP_vfree(r0, r1, NULL);
		return result;
	}

	filters_u = SEXP_list_new(NULL);	/* unavailable filters */
	filters_a = SEXP_list_new(NULL);	/* available filters (cached) */
	filters_req = SEXP_list_new(NULL);	/* request list for probe_ste_fetch() */

	s_subset[0] = NULL;
	s_subset[1] = NULL;
	s_subset_i = 0;

	o_subset[0] = NULL;
	o_subset[1] = NULL;
	o_subset_i = 0;

	result = NULL;

	op_val = probe_ent_getattrval(set, "operation");

	if (op_val != NULL)
		op_num = SEXP_number_geti_32(op_val);
	else
		op_num = OVAL_SET_OPERATION_UNION;

	SEXP_free(op_val);

	_A(op_num == OVAL_SET_OPERATION_UNION ||
	   op_num == OVAL_SET_OPERATION_COMPLEMENT || op_num == OVAL_SET_OPERATION_INTERSECTION);

#define probe_set_foreach(elm_var, set_list) SEXP_sublist_foreach (elm_var, set_list, 2, 1000)
#define CASE(__c1, __rest) case (__c1): if (strcmp (__rest, member_name + 1) == 0)

	probe_set_foreach(member, set) {
		if (probe_ent_getname_r(member, member_name, sizeof member_name) == 0) {
			snprintf(msg, sizeof (msg), "probe_set_eval: Invalid set element: ptr=%p, type=%s.\n", member, SEXP_strtype(member));
			_D(msg);
			goto eval_fail;
		}

		switch (member_name[0]) {
			CASE('s', "et") {
				if (s_subset_i < 2) {
					s_subset[s_subset_i] = probe_set_eval(member, depth + 1);

					if (s_subset[s_subset_i] == NULL) {
						snprintf(msg, sizeof (msg),
							 "probe_set_eval: Recursive set evaluation failed: m=%p, d=%zu.\n",
							 member, depth + 1);
						_D(msg);
						goto eval_fail;
					}

					++s_subset_i;
				} else {
					snprintf(msg, sizeof (msg), "probe_set_eval: More than 2 \"set\".\n");
					_D(msg);
					goto eval_fail;
				}
			}
			break;

			CASE('o', "bj_ref") {
				SEXP_t *id, *res;

				id = probe_ent_getval(member);

				if (id == NULL) {
					snprintf(msg, sizeof (msg), "probe_set_eval: set=%p: Missing obj_ref value.\n", set);
					_D(msg);
					goto eval_fail;
				}

				res = pcache_sexp_get(OSCAP_GSYM(pcache), id);

				if (res == NULL) {
					/* cache miss */
					res = probe_obj_eval(id);

					if (res == NULL) {
						char *tmp = SEXP_string_cstr(id);
						snprintf(msg, sizeof (msg), "probe_set_eval: obj=%s: Evaluation failed.\n", tmp);
#if !defined(NDEBUG)
						_D(msg);
#endif
						oscap_free(tmp);
						SEXP_free(id);
						goto eval_fail;
					}
				}

				SEXP_free(id);

				if (o_subset_i < 2) {
					o_subset[o_subset_i] = res;
					++o_subset_i;
				} else {
					snprintf(msg, sizeof (msg), "probe_set_eval: More than 2 obj_refs.\n");
					_D(msg);
					SEXP_free(res);
					goto eval_fail;
				}

			}
			break;

			CASE('f', "ilter") {
				SEXP_t *id, *act, *ste;

				id = probe_ent_getval(member);
				if (id == NULL) {
					snprintf(msg, sizeof (msg), "probe_set_eval: set=%p: Missing filter value.\n", set);
					_D(msg);
					goto eval_fail;
				}

				act = probe_ent_getattrval(member, "action");
				if (act == NULL) {
					snprintf(msg, sizeof (msg), "probe_set_eval: set=%p: Missing filter action.\n", set);
					_D(msg);
					goto eval_fail;
				}

				ste = pcache_sexp_get(OSCAP_GSYM(pcache), id);

				if (ste == NULL) {
					SEXP_list_add(filters_req, id);
					r0 = SEXP_list_new(act, id, NULL);
					SEXP_list_add(filters_u, r0);
				} else {
					r0 = SEXP_list_new(act, ste, NULL);
					SEXP_list_add(filters_a, r0);
					SEXP_free(ste);
				}

				SEXP_vfree(id, act, r0, NULL);
			}
			break;
		default:
			snprintf(msg, sizeof (msg), "probe_set_eval: Unexpected set element: %s.\n", member_name);
			_D(msg);
			goto eval_fail;
		}
#undef CASE

	}

	member = NULL;

	/* request filters */
	result = probe_ste_fetch(filters_req);

	if (result == NULL) {
		snprintf(msg, sizeof (msg), "probe_set_eval: Can't get unavailable filters:\n");
#if !defined(NDEBUG)
		_D(msg);
		SEXP_list_foreach(result, filters_req) {
			SEXP_fprintfa(stdout, result);
			printf("\n");
		}
#endif
		SEXP_free(filters_req);
		goto eval_fail;
	}
	SEXP_vfree(filters_req, result, NULL);

	SEXP_list_foreach(member, filters_u) {
		SEXP_t *id, *act, *ste;

		act = SEXP_list_first(member);
		id = SEXP_list_nth(member, 2);
		ste = pcache_sexp_get(OSCAP_GSYM(pcache), id);
		r0 = SEXP_list_new(act, ste, NULL);
		SEXP_list_add(filters_a, r0);
		SEXP_vfree(act, id, ste, r0, NULL);
	}

	SEXP_free(filters_u);

	_A((s_subset_i > 0 && o_subset_i == 0) || (s_subset_i == 0 && o_subset_i > 0));

	if (o_subset_i > 0) {
		for (s_subset_i = 0; s_subset_i < o_subset_i; ++s_subset_i) {
			s_subset[s_subset_i] = probe_set_apply_filters(o_subset[s_subset_i], filters_a);

#ifndef NDEBUG
			if (s_subset[s_subset_i] == NULL) {
				snprintf(msg, sizeof (msg),
					 "probe_set_eval: apply_filters returned NULL: set=%p, filters=%p.\n",
					 o_subset[s_subset_i], filters_a);
				_D(msg);
				goto eval_fail;
			}
#endif
			SEXP_free(o_subset[s_subset_i]);
		}

		SEXP_free(filters_a);
	}

	result = probe_set_combine(s_subset[0], s_subset[1], op_num);

	_A(result != NULL);

	SEXP_free(s_subset[0]);
	SEXP_free(s_subset[1]);

	return (result);
 eval_fail:
	SEXP_free(member);

	for (; s_subset_i > 0; --s_subset_i)
		SEXP_free(s_subset[s_subset_i - 1]);

	for (; o_subset_i > 0; --o_subset_i)
		SEXP_free(o_subset[o_subset_i - 1]);

	SEXP_free(filters_u);
	SEXP_free(filters_a);
	SEXP_free(result);

	r0 = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR, msg);
	r1 = SEXP_list_new(r0, NULL);
	result = probe_cobj_new(SYSCHAR_FLAG_ERROR, r1, NULL);
	SEXP_vfree(r0, r1, NULL);
	return result;
}

/**
 * Cleanup function. This function is registered as a exit
 * hook using the atexit(3) function so as to properly free
 * all resources used by the probe.
 */
static void probe_cleanup(void)
{
	probe_fini(OSCAP_GSYM(probe_arg));
	pcache_free(OSCAP_GSYM(pcache));
        encache_free(OSCAP_GSYM(encache));
	SEAP_close(OSCAP_GSYM(ctx), OSCAP_GSYM(sd));
	SEAP_CTX_free(OSCAP_GSYM(ctx));
}

/**
 * Signal hook. This function is registered as a signal handler
 * for various signals and its only purpose is to set a global
 * flag. This flag is watched by the main loop in the probe and
 * in case it is set, the probe starts the process of a clean
 * shutdown or immediately exits in case the flag was set to
 * PROBE_SIGEXIT_UNCLEAN -- signaling a fatal error.
 */
static void probe_sigexit(int signum)
{
        switch(signum) {
        case SIGTERM:
        case SIGINT:
        case SIGPIPE:
        case SIGQUIT:
                OSCAP_GSYM(sigexit) = PROBE_SIGEXIT_CLEAN;
                break;
        case SIGSEGV:
        case SIGABRT:
                OSCAP_GSYM(sigexit) = PROBE_SIGEXIT_UNCLEAN;
                break;
	case SIGUSR1:
		OSCAP_GSYM(sigexit) = PROBE_SIGEXIT_ABORT;
		break;
        }
}

static SEXP_t *probe_reset(SEXP_t *arg0, void *arg1)
{
        /*
         * FIXME: implement main loop locking & worker waiting
         */
	pcache_free(OSCAP_GSYM(pcache));
        encache_free(OSCAP_GSYM(encache));

        OSCAP_GSYM(pcache) = pcache_new();
        OSCAP_GSYM(encache)= encache_new();

	probe_item_resetidctr(&(OSCAP_GSYM(id_desc)));

        return(NULL);
}

/**
 * Common probe entry point. This is the common main function of all
 * probe processes that initializes all the stuff needed at probe
 * runtime. This function also contains the main loop which listens
 * for new request and in case the result was already evaluated (it is
 * stored in the item cache) it replies to the request immediately.
 * In the other case a new thread is spawned which takes care of the
 * evaluation and reply.
 * @return 0 on success, error code otherwise
 */
int main(void)
{
	int ret = EXIT_SUCCESS;
	SEAP_msg_t *seap_request, *seap_reply;

	SEXP_t *probe_in, *probe_out = NULL;
	int probe_ret;

	SEXP_t *oid;

	pthread_attr_t thread_attr;
	pthread_t      thread;

        struct sigaction sigact;

        /* Setup signal handler */
        sigemptyset(&sigact.sa_mask);
        sigaddset(&sigact.sa_mask, SIGTERM);
        sigaddset(&sigact.sa_mask, SIGINT);
        sigaddset(&sigact.sa_mask, SIGPIPE);
        sigaddset(&sigact.sa_mask, SIGQUIT);
        sigaddset(&sigact.sa_mask, SIGSEGV);
        sigaddset(&sigact.sa_mask, SIGABRT);

        sigact.sa_handler = &probe_sigexit;
        sigact.sa_flags   = 0;

        if (sigaction(SIGTERM, &sigact, NULL) != 0 ||
            sigaction(SIGINT,  &sigact, NULL) != 0 ||
            sigaction(SIGPIPE, &sigact, NULL) != 0 ||
            sigaction(SIGQUIT, &sigact, NULL) != 0 ||
            sigaction(SIGSEGV, &sigact, NULL) != 0 ||
            sigaction(SIGABRT, &sigact, NULL) != 0 ||
	    sigaction(SIGUSR1, &sigact, NULL) != 0)
        {
                _D("Can't setup signal handlers: errno=%u, %s.\n", errno, strerror(errno));
                exit(errno);
        }

	/* Initialize SEAP */
	OSCAP_GSYM(ctx) = SEAP_CTX_new();
	OSCAP_GSYM(sd) = SEAP_openfd2(OSCAP_GSYM(ctx), STDIN_FILENO, STDOUT_FILENO, 0);

	if (OSCAP_GSYM(sd) < 0) {
		_D("Can't create SEAP descriptor: errno=%u, %s.\n", errno, strerror(errno));
		exit(errno);
	}

	/* Create cache */
	OSCAP_GSYM(pcache) = pcache_new();
	if (OSCAP_GSYM(pcache) == NULL) {
		_D("Can't create cache: %u, %s.\n", errno, strerror(errno));
		exit(errno);
	}

	if (pthread_attr_init(&thread_attr) != 0) {
		_D("Can't initialize thread attributes: %u, %s.\n", errno, strerror(errno));
		exit(errno);
	}

        if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
                _D("Can't set detach state: %u, %s.\n", errno, strerror (errno));
                exit (errno);
        }

	OSCAP_GSYM(probe_arg) = probe_init();

        /* Create the element name cache */
        OSCAP_GSYM(encache) = encache_new ();

	/* Reset the item id generator */
	probe_item_resetidctr(&(OSCAP_GSYM(id_desc)));

        /* Register cleanup function */
        atexit(&probe_cleanup);

        /* Register SEAP commands */
        if (SEAP_cmd_register(OSCAP_GSYM(ctx), PROBECMD_RESET, 0, &probe_reset) != 0) {
                _D("Can't register SEAP command: %s: %u, %s.\n", "reset", errno, strerror (errno));
                exit(errno);
        }

	/* Main loop */
	while(OSCAP_GSYM(sigexit) == 0) {
		if (SEAP_recvmsg(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), &seap_request) == -1) {
			ret = errno;
			_D("An error ocured while receiving SEAP message. errno=%u, %s.\n", errno, strerror(errno));
                        switch(OSCAP_GSYM(sigexit)) {
                        case PROBE_SIGEXIT_CLEAN:
                                ret = 0;
                                break;
			case PROBE_SIGEXIT_ABORT:
				ret = ECONNABORTED;
				exit(ECONNABORTED); /* hack */
				break;
                        case PROBE_SIGEXIT_UNCLEAN:
                                /* Try to save cached data & restart? */
                                _exit(ret);
                        }

			break;
		}

		probe_in = SEAP_msg_get(seap_request);
		if (probe_in == NULL) {
			_D("Unexpected error: probe_in = NULL\n");
			abort();
		}

		SEXP_VALIDATE(probe_in);
		oid = probe_obj_getattrval(probe_in, "id");
                SEXP_free(probe_in);

		if (oid == NULL) {
			_D("Invalid object: %s\n", "attribute \"id\" not set\n");
			probe_ret = PROBE_ENOATTR;
		} else {
			SEXP_VALIDATE(oid);

			probe_out = pcache_sexp_get(OSCAP_GSYM(pcache), oid);
                        SEXP_free(oid);

			if (probe_out == NULL) {
				/* cache miss */

				if (pthread_create(&thread, &thread_attr, &probe_worker, (void *)seap_request) != 0) {
					_D("Can't start new probe worker: %u, %s.\n", errno, strerror(errno));

					/* send error */
					continue;
				}

				seap_request = NULL;
				continue;
			} else {
				/* cache hit */
				probe_ret = 0;
			}
		}

		if (probe_out == NULL || probe_ret != 0) {
			if (SEAP_replyerr(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), seap_request, probe_ret) == -1) {
				_D("An error ocured while sending error status. errno=%u, %s.\n",
				   errno, strerror(errno));

				SEAP_msg_free(seap_request);

                                if (probe_out != NULL)
                                        SEXP_free(probe_out);

				break;
			}
		} else {
			SEXP_VALIDATE(probe_out);

			seap_reply = SEAP_msg_new();
			SEAP_msg_set(seap_reply, probe_out);
                        SEXP_free(probe_out);

			if (SEAP_reply(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), seap_reply, seap_request) == -1) {
				ret = errno;

				_D("An error ocured while sending SEAP message. errno=%u, %s.\n",
				   errno, strerror(errno));

                                switch(OSCAP_GSYM(sigexit)) {
                                case PROBE_SIGEXIT_CLEAN:
                                        SEAP_msg_free(seap_reply);
                                        SEAP_msg_free(seap_request);
                                        ret = 0;
                                        break;
				case PROBE_SIGEXIT_ABORT:
                                        SEAP_msg_free(seap_reply);
                                        SEAP_msg_free(seap_request);
					ret = ECONNABORTED;
					break;
                                case PROBE_SIGEXIT_UNCLEAN:
                                        /* Try to save cache & restart? */
                                        _exit(ret);
                                default:
                                        SEAP_msg_free(seap_reply);
                                        SEAP_msg_free(seap_request);
                                }

				break;
			}

			SEAP_msg_free(seap_reply);
		}

		SEAP_msg_free(seap_request);
	}

	switch(OSCAP_GSYM(sigexit)) {
	case PROBE_SIGEXIT_CLEAN:
		ret = 0;
		break;
	case PROBE_SIGEXIT_ABORT:
		ret = ECONNABORTED;
		break;
	case PROBE_SIGEXIT_UNCLEAN:
		/* Try to save cached data & restart? */
		_exit(ret);
	}

	return (ret);
}

struct probe_varref_ctx {
	SEXP_t *pi2;
	unsigned int ent_cnt;
	struct probe_varref_ctx_ent *ent_lst;
};

struct probe_varref_ctx_ent {
	SEXP_t *ent_name_sref;
	unsigned int val_cnt;
	unsigned int next_val_idx;
};

static int probe_varref_create_ctx(const SEXP_t * probe_in, SEXP_t * varrefs, struct probe_varref_ctx **octx)
{
	unsigned int i, ent_cnt, val_cnt, varref_cnt;
	SEXP_t *ent_name, *ent, *varref, *val_lst;
	SEXP_t *r0, *r1, *r2, *r3;
	SEXP_t *vid, *vidx_name, *vidx_val;
	struct probe_varref_ctx *ctx;

	varref_cnt = SEXP_number_getu_32(r0 = SEXP_list_nth(varrefs, 2));
	ent_cnt = SEXP_number_getu_32(r1 = SEXP_list_nth(varrefs, 3));
	SEXP_vfree(r0, r1, NULL);

	ctx = oscap_talloc (struct probe_varref_ctx);
	ctx->pi2 = SEXP_softref((SEXP_t *)probe_in);
	ctx->ent_cnt = ent_cnt;
	ctx->ent_lst = oscap_alloc(ent_cnt * sizeof (ctx->ent_lst[0]));

	vidx_name = SEXP_string_new(":val_idx", 8);
	vidx_val = SEXP_number_newu(0);

	/* entities that use var_refs are stored at the begining of an object */
	for (i = 0; i < ent_cnt; ++i) {
		/*
		 * add variable values to entities and insert
		 * them into the new probe_in object
		 */
		r0 = SEXP_list_nth(ctx->pi2, i + 2);
		vid = probe_ent_getattrval(r0, "var_ref");
		r1 = SEXP_list_first(r0);
		r2 = SEXP_list_first(r1);

		r3 = SEXP_list_new(r2, vidx_name, vidx_val, NULL);
		SEXP_free(r0);
		r0 = SEXP_list_rest(r1);
		ent_name = SEXP_list_join(r3, r0);
		SEXP_vfree(r0, r1, r2, r3, NULL);

		SEXP_sublist_foreach(varref, varrefs, 4, SEXP_LIST_END) {
			r0 = SEXP_list_first(varref);
			if (!SEXP_string_cmp(vid, r0)) {
				break;
			}
			SEXP_free(r0);
		}
		if (varref == NULL) {
			_D("Unexpected error: variable id \"%s\" not found in varrefs.", SEXP_string_cstr(vid));
			abort();
		}
		SEXP_free(r0);
		SEXP_free(vid);

		r0 = SEXP_list_nth(varref, 2);
		val_cnt = SEXP_number_getu_32(r0);
		val_lst = SEXP_list_nth(varref, 3);
		SEXP_vfree(varref, r0, NULL);

		ent = SEXP_list_new(ent_name, val_lst, NULL);
		SEXP_vfree(ent_name, val_lst, NULL);

		r0 = SEXP_list_replace(ctx->pi2, i + 2, ent);
		SEXP_vfree(r0, ent, NULL);

		r0 = SEXP_listref_nth(ctx->pi2, i + 2);
		ctx->ent_lst[i].ent_name_sref = SEXP_listref_first(r0);
		SEXP_free(r0);
		ctx->ent_lst[i].val_cnt = val_cnt;
		ctx->ent_lst[i].next_val_idx = 0;
	}

	SEXP_vfree(vidx_name, vidx_val, NULL);

	*octx = ctx;

	return 0;
}

static void probe_varref_destroy_ctx(struct probe_varref_ctx *ctx)
{
	struct probe_varref_ctx_ent *ent, *ent_end;

	SEXP_free(ctx->pi2);

	ent = ctx->ent_lst;
	ent_end = ent + ctx->ent_cnt;

	while (ent != ent_end) {
		SEXP_free(ent->ent_name_sref);
		++ent;
	}

	free(ctx->ent_lst);
	free(ctx);
}

static int probe_varref_iterate_ctx(struct probe_varref_ctx *ctx)
{
	unsigned int val_cnt, *next_val_idx;
	SEXP_t *ent_name_sref;
	SEXP_t *r0, *r1, *r2;
	struct probe_varref_ctx_ent *ent, *ent_end;

	ent = ctx->ent_lst;
	ent_end = ent + ctx->ent_cnt;
	val_cnt = ent->val_cnt;
	next_val_idx = &ent->next_val_idx;
	ent_name_sref = ent->ent_name_sref;

	r0 = SEXP_number_newu(0);

	while (++(*next_val_idx) >= val_cnt) {
		if (++ent == ent_end) {
			SEXP_free(r0);
			return 0;
		}

		*next_val_idx = 0;
		r1 = SEXP_list_replace(ent_name_sref, 3, r0);
		SEXP_free(r1);

		val_cnt = ent->val_cnt;
		next_val_idx = &ent->next_val_idx;
		ent_name_sref = ent->ent_name_sref;
	}
	r1 = SEXP_list_replace(ent_name_sref, 3, r2 = SEXP_number_newu(*next_val_idx));
	SEXP_vfree(r0, r1, r2, NULL);

	return 1;
}

/**
 * Worker thread function. This functions handles the evalution of objects and sets.
 * @param arg SEAP message with the request which contains the object to be evaluated
 */
void *probe_worker(void *arg)
{
	int probe_ret = 0;
	SEXP_t *probe_in, *set, *probe_out = NULL, *oid;
	SEXP_t *varrefs;

	SEAP_msg_t *seap_reply, *seap_request;

	_LOGCALL_;

	seap_request = (SEAP_msg_t *) arg;
	probe_in = SEAP_msg_get(seap_request);

	set = probe_obj_getent(probe_in, "set", 1);

	if (set != NULL) {
		/* complex object */
		probe_out = probe_set_eval(set, 0);
		SEXP_free(set);
		// todo: in case of an internal error set probe_ret accordingly
	} else {
		/* simple object */
		varrefs = probe_obj_getent(probe_in, "varrefs", 1);

		if (varrefs == NULL || !OSCAP_GSYM(varref_handling)) {
			_D("probe_main1\n");
			probe_out = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL);
			probe_ret = probe_main(probe_in, probe_out, OSCAP_GSYM(probe_arg));
			probe_cobj_compute_flag(probe_out);
		} else {
			/*
			 * there are variable references in the object.
			 * create ctx, iterate through all variable combinations
			 */
			struct probe_varref_ctx *ctx;

			_D("probe_main2\n");

			probe_varref_create_ctx(probe_in, varrefs, &ctx);
			SEXP_free(varrefs);

			do {
				SEXP_t *cobj, *r0;

				cobj = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL);
				probe_ret = probe_main(ctx->pi2, cobj, OSCAP_GSYM(probe_arg));
				probe_cobj_compute_flag(cobj);
				r0 = probe_out;
				probe_out = probe_set_combine(r0, cobj, OVAL_SET_OPERATION_UNION);
				SEXP_vfree(cobj, r0, NULL);
			} while (probe_ret == 0
				 && probe_varref_iterate_ctx(ctx));

			probe_varref_destroy_ctx(ctx);
		}
	}

	_D("probe_out = %p, probe_ret = %d\n", (void *)probe_out, probe_ret);

	SEXP_VALIDATE(probe_out);

	oid = probe_obj_getattrval(probe_in, "id");
	_A(oid != NULL);
	SEXP_free(probe_in);
	if (pcache_sexp_add(OSCAP_GSYM(pcache), oid, probe_out) != 0) {
		/* TODO */
		abort();
	}

	SEXP_free(oid);

	if (probe_ret != 0) {
		if (SEAP_replyerr(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), seap_request, probe_ret) == -1) {
			int ret = errno;

			_D("An error ocured while sending error status. errno=%u, %s.\n", errno, strerror(errno));

			SEAP_msg_free(seap_request);
			SEXP_free(probe_out);

			/* FIXME */
			exit(ret);
		}
	} else {
		seap_reply = SEAP_msg_new();
		SEAP_msg_set(seap_reply, probe_out);

		if (SEAP_reply(OSCAP_GSYM(ctx), OSCAP_GSYM(sd), seap_reply, seap_request) == -1) {
			int ret = errno;

			_D("An error ocured while sending SEAP message. errno=%u, %s.\n", errno, strerror(errno));

			SEAP_msg_free(seap_reply);
			SEAP_msg_free(seap_request);
                        SEXP_free(probe_out);

			exit(ret);
		}

		SEAP_msg_free(seap_reply);
                SEXP_free(probe_out);
	}

	SEAP_msg_free(seap_request);
	return (NULL);
}

static int probe_optkcmp(int *a, probe_option_t *b)
{
	if (*a > b->option)
		return  1;
	if (*a < b->option)
		return -1;
	else
		return  0;
}

int probe_setoption(int option, ...)
{
	va_list ap;
	probe_option_t *optrec;
	int ret;

	optrec = oscap_bfind (OSCAP_GSYM(options), OPTIONS_COUNT, sizeof(probe_option_t), &option, (int(*)(void *, void *))&probe_optkcmp);

	if (optrec == NULL) {
		errno = EINVAL;
		return (-1);
	}

	if (optrec->handler == NULL)
		return 0;

	va_start(ap, option);
	ret = optrec->handler(option, ap);
	va_end(ap);

	return (ret);
}

static int probe_optekcmp(const char *a, char **b)
{
	return strcmp(a, *b);
}

static int probe_optekcmp_sexp(const SEXP_t *a, char **b)
{
	return SEXP_strcmp(a, *b);
}

static int probe_optecmp(char **a, char **b)
{
	return strcmp(*a, *b);
}

static int probe_opthandler_varref(int option, va_list args)
{
	bool  o_switch;
	char *o_name;
	char *o_temp;

	o_switch = va_arg(args, int);
	o_name   = va_arg(args, char *);

	if (o_name == NULL) {
		/* switch varref handling on/off globally */
		OSCAP_GSYM(varref_handling) = o_switch;
		return (0);
	}

	o_temp = oscap_bfind (OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
			      sizeof(char *), o_name, (int(*)(void *, void *)) &probe_optecmp);

	if (o_temp != NULL)
		return (0);

	OSCAP_GSYM(no_varref_ents) = oscap_realloc(OSCAP_GSYM(no_varref_ents),
						   sizeof (char *) * ++OSCAP_GSYM(no_varref_ents_cnt));
	OSCAP_GSYM(no_varref_ents)[OSCAP_GSYM(no_varref_ents_cnt) - 1] = strdup(o_name);

	qsort(OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt), sizeof (char *), (int(*)(const void *, const void *))&probe_optecmp);

	return (0);
}

static int probe_opthandler_rcache(int option, va_list args)
{
	return (0);
}

bool probe_handle_varref_cstr (const char *ent_name)
{
	if (ent_name == NULL)
		return OSCAP_GSYM(varref_handling);

	if (OSCAP_GSYM(no_varref_ents_cnt) == 0)
		return true;

	return (oscap_bfind (OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
			     sizeof(char *), (void *)ent_name, (int(*)(void *, void *))&probe_optekcmp) == NULL ? true : false);
}

bool probe_handle_varref_sexp (const SEXP_t *ent_name)
{
	if (ent_name == NULL)
		return OSCAP_GSYM(varref_handling);

	if (OSCAP_GSYM(no_varref_ents_cnt) == 0)
		return true;

	return (oscap_bfind (OSCAP_GSYM(no_varref_ents), OSCAP_GSYM(no_varref_ents_cnt),
			     sizeof(char *), (void *)ent_name, (int(*)(void *, void *))&probe_optekcmp_sexp) == NULL ? true : false);
}
