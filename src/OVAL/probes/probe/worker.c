/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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
 *      Daniel Kopecek <dkopecek@redhat.com>
 */
#include <seap.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "probe-api.h"
#include "common/debug_priv.h"
#include "common/assume.h"
#include "entcmp.h"

#include "worker.h"

extern bool  OSCAP_GSYM(varref_handling);
extern void *OSCAP_GSYM(probe_arg);

void *probe_worker_runfn(void *arg)
{
	probe_pwpair_t *pair = (probe_pwpair_t *)arg;

	SEXP_t *probe_res, *obj, *oid;
	int     probe_ret;

	dI("handling SEAP message ID %u\n", pair->pth->sid);
	//
	probe_ret = -1;
	probe_res = pair->pth->msg_handler(pair->probe, pair->pth->msg, &probe_ret);
	//
	dI("handler result = %p, return code = %d\n", probe_res, probe_ret);

	/* Assuming that the red-black tree API is doing locking for us... */
	if (rbt_i32_del(pair->probe->workers, pair->pth->sid, NULL) != 0) {
		dW("thread not found in the probe thread tree, probably canceled by an external signal\n");
		/*
		 * XXX: this is a possible deadlock; we can't send anything from
		 * here because the signal handler replied to the message
		 */
		arg = NULL;

                SEAP_msg_free(pair->pth->msg);
                SEXP_free(probe_res);
                oscap_free(pair);

                return (NULL);
	} else {
		dI("probe thread deleted\n");

		obj = SEAP_msg_get(pair->pth->msg);
		oid = probe_obj_getattrval(obj, "id");

		if (probe_rcache_sexp_add(pair->probe->rcache, oid, probe_res) != 0) {
			/* TODO */
			abort();
		}

		SEXP_vfree(obj, oid, NULL);
	}

	if (probe_ret != 0) {
		/*
		 * Something bad happened. A hint of the cause is stored as a error code in
		 * probe_ret (should be). We'll send it to the library using a SEAP error packet.
		 */
		if (SEAP_replyerr(pair->probe->SEAP_ctx, pair->probe->sd, pair->pth->msg, probe_ret) == -1) {
			int ret = errno;

			dE("An error ocured while sending error status. errno=%u, %s.\n", errno, strerror(errno));
			SEXP_free(probe_res);

			/* FIXME */
			exit(ret);
		}
	} else {
		SEAP_msg_t *seap_reply;
		/*
		 * OK, the probe actually returned something, let's send it to the library.
		 */
		seap_reply = SEAP_msg_new();
		SEAP_msg_set(seap_reply, probe_res);

		if (SEAP_reply(pair->probe->SEAP_ctx, pair->probe->sd, seap_reply, pair->pth->msg) == -1) {
			int ret = errno;

			SEAP_msg_free(seap_reply);
			SEXP_free(probe_res);

			exit(ret);
		}

		SEAP_msg_free(seap_reply);
                SEXP_free(probe_res);
	}

        SEAP_msg_free(pair->pth->msg);
        oscap_free(pair->pth);
	oscap_free(pair);

	return (NULL);
}

probe_worker_t *probe_worker_new(void)
{
	probe_worker_t *pth = oscap_talloc(probe_worker_t);

	pth->sid = 0;
	pth->tid = 0;
	pth->msg_handler = NULL;
	pth->msg = NULL;

	return (pth);
}

#if 0
/**
 * Create a new thread for handling a message with ID `sid'. This thread will be registered by
 * the thread manager. The handler is defined by the function `function' and `arg' is the user
 * pointer which will be passed to this function. The `arg_free' function is called only if the
 * thread is canceled before returing from the handler function.
 */
int probe_worker_create(probe_t *probe, SEAP_msgid_t sid, SEXP_t * (*msg_handler)(SEAP_msg_t *, int *), SEAP_msg_t *msg)
{
	pthread_attr_t   pth_attr;
	probe_pmpair_t  *pair;

	if (mgr == NULL || msg_handler == NULL)
		return (-1);

	if (pthread_attr_init(&pth_attr) != 0)
		return (-1);

	if (pthread_attr_setdetachstate(&pth_attr, PTHREAD_CREATE_DETACHED) != 0) {
		pthread_attr_destroy(&pth_attr);
		return (-1);
	}

	pair = oscap_talloc(probe_pmpair_t);

	pair->mgr      = mgr;
	pair->pth      = probe_thread_new();
	pair->pth->sid = sid;
	pair->pth->msg = msg;
	pair->pth->msg_handler = msg_handler;

	if (rbt_i32_add(mgr->threads, sid, pair->pth, NULL) != 0) {
		/*
		 * Getting here means that there is already a
		 * thread handling the message with the given
		 * ID.
		 */
		pthread_attr_destroy(&pth_attr);
		oscap_free(pair->pth);
		oscap_free(pair);

		return (-1);
	}

	if (pthread_create(&pair->pth->tid, &pth_attr, &probe_thread_runfn, pair) != 0) {
		pthread_attr_destroy(&pth_attr);

		if (rbt_i32_del(pair->mgr->threads, pair->pth->sid, NULL) != 0) {
			/* ... do something ... */
		}

		oscap_free(pair);
		return (-1);
	}

	pthread_attr_destroy(&pth_attr);

	return (0);
}
#endif /* 0 */

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

static int probe_varref_create_ctx(const SEXP_t *probe_in, SEXP_t *varrefs, struct probe_varref_ctx **octx)
{
	unsigned int i, ent_cnt, val_cnt;
	SEXP_t *ent_name, *ent, *varref, *val_lst;
	SEXP_t *r0, *r1, *r2, *r3;
	SEXP_t *vid, *vidx_name, *vidx_val;
	struct probe_varref_ctx *ctx;

	/* varref_cnt = SEXP_number_getu_32(r0 = SEXP_list_nth(varrefs, 2)); */
	ent_cnt = SEXP_number_getu_32(r1 = SEXP_list_nth(varrefs, 3));
	SEXP_free(r1);

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
			dE("Unexpected error: variable id \"%s\" not found in varrefs.", SEXP_string_cstr(vid));
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
static SEXP_t *probe_ste_fetch(probe_t *probe, SEXP_t *id_list)
{
	SEXP_t *res, *ste, *id;
	uint32_t i_len, r_len;

	_LOGCALL_;

	i_len = SEXP_list_length(id_list);

	if (i_len == 0)
		return SEXP_list_new(NULL);

	res = SEAP_cmd_exec(probe->SEAP_ctx, probe->sd, 0, PROBECMD_STE_FETCH, id_list, SEAP_CMDTYPE_SYNC, NULL, NULL);

	r_len = SEXP_list_length(res);

	if (i_len != r_len) {
		SEXP_free(res);
		return (NULL);
	}

	for (; i_len > 0; --i_len) {
		ste = SEXP_list_nth(res, i_len);
		id  = SEXP_list_nth(id_list, i_len);

		_A(id != NULL);
		_A(ste != NULL);

		if (probe_rcache_sexp_add(probe->rcache, id, ste) != 0) {

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
static SEXP_t *probe_obj_eval(probe_t *probe, SEXP_t *id)
{
	SEXP_t *res, *rid;

	_LOGCALL_;

	res = SEAP_cmd_exec(probe->SEAP_ctx, probe->sd, 0, PROBECMD_OBJ_EVAL, id, SEAP_CMDTYPE_SYNC, NULL, NULL);

	rid = SEXP_list_first(res);
	assume_r(SEXP_string_cmp(id, rid) == 0, NULL);
	SEXP_vfree(res, rid, NULL);

	return probe_rcache_sexp_get(probe->rcache, id);
}

static SEXP_t *probe_prepare_filters(probe_t *probe, SEXP_t *obj)
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
		ste = probe_rcache_sexp_get(probe->rcache, ste_id);

		if (ste == NULL) {
			SEXP_t *r0, *r1;

			r0  = SEXP_list_new(ste_id, NULL);
			r1  = probe_ste_fetch(probe, r0);
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
			SEXP_t *r0, *r1;

                        r0 = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                              "Unexpected set operation: %d.\n", op);
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
				SEXP_t *r0, *r1;

                                r0 = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                      "Supplied item has an invalid status: %d.\n", item_status);
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
static SEXP_t *probe_set_eval(probe_t *probe, SEXP_t *set, size_t depth)
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

	SEXP_t *r0, *r1, *result, *msg = NULL;

	_LOGCALL_;

	if (depth > MAX_EVAL_DEPTH) {
		char *fmt = "probe_set_eval: Too many levels: max=%zu.\n";
#ifndef NDEBUG
                _D(fmt, (size_t) MAX_EVAL_DEPTH);
		abort();
#endif
		r0 = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, fmt, (size_t) MAX_EVAL_DEPTH);
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
                        msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR, "probe_set_eval: Invalid set element: ptr=%p, type=%s.\n", member, SEXP_strtype(member));
			goto eval_fail;
		}

		switch (member_name[0]) {
			CASE('s', "et") {
				if (s_subset_i < 2) {
					s_subset[s_subset_i] = probe_set_eval(probe, member, depth + 1);

					if (s_subset[s_subset_i] == NULL) {
                                                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                                       "probe_set_eval: Recursive set evaluation failed: m=%p, d=%zu.\n",
                                                                       member, depth + 1);
						goto eval_fail;
					}

					++s_subset_i;
				} else {
                                        msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                               "probe_set_eval: More than 2 \"set\".\n");
					goto eval_fail;
				}
			}
			break;

			CASE('o', "bj_ref") {
				SEXP_t *id, *res;

				id = probe_ent_getval(member);

				if (id == NULL) {
                                        msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                              "probe_set_eval: set=%p: Missing obj_ref value.\n",
                                                              set);
					goto eval_fail;
				}

				res = probe_rcache_sexp_get(probe->rcache, id);

				if (res == NULL) {
					/* cache miss */
					res = probe_obj_eval(probe, id);

					if (res == NULL) {
						char *tmp = SEXP_string_cstr(id);

                                                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                                       "probe_set_eval: obj=%s: Evaluation failed.\n",
                                                                       tmp);
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
                                        msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                               "probe_set_eval: More than 2 obj_refs.\n");
					SEXP_free(res);
					goto eval_fail;
				}
			}
			break;

			CASE('f', "ilter") {
				SEXP_t *id, *act, *ste;

				id = probe_ent_getval(member);
				if (id == NULL) {
					msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                               "probe_set_eval: set=%p: Missing filter value.\n", set);
					goto eval_fail;
				}

				act = probe_ent_getattrval(member, "action");
				if (act == NULL) {
					msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                               "probe_set_eval: set=%p: Missing filter action.\n", set);
					goto eval_fail;
				}

				ste = probe_rcache_sexp_get(probe->rcache, id);

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
			msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                               "probe_set_eval: Unexpected set element: %s.\n", member_name);
			goto eval_fail;
		}
#undef CASE

	}

	member = NULL;

	/* request filters */
	result = probe_ste_fetch(probe, filters_req);

	if (result == NULL) {
                msg = probe_msg_creat(OVAL_MESSAGE_LEVEL_ERROR,
                                      "probe_set_eval: Can't get unavailable filters.\n");
		SEXP_free(filters_req);
		goto eval_fail;
	}
	SEXP_vfree(filters_req, result, NULL);

	SEXP_list_foreach(member, filters_u) {
		SEXP_t *id, *act, *ste;

		act = SEXP_list_first(member);
		id = SEXP_list_nth(member, 2);
		ste = probe_rcache_sexp_get(probe->rcache, id);
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
                                msg = probe_msg_creatf(OVAL_MESSAGE_LEVEL_ERROR,
                                                       "probe_set_eval: apply_filters returned NULL: set=%p, filters=%p.\n",
                                                       o_subset[s_subset_i], filters_a);
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

        r1 = SEXP_list_new(msg, NULL);
	result = probe_cobj_new(SYSCHAR_FLAG_ERROR, r1, NULL);
	SEXP_vfree(msg, r1, NULL);
	return result;
}

/**
 * Worker thread function. This functions handles the evalution of objects and sets.
 * @param msg_in SEAP message with the request which contains the object to be evaluated
 * @param ret pointer to the return code storage
 */
SEXP_t *probe_worker(probe_t *probe, SEAP_msg_t *msg_in, int *ret)
{
	SEXP_t *probe_in, *probe_out, *set;

	if (msg_in == NULL) {
		*ret = PROBE_EINVAL;
		return (NULL);
	}

	probe_in  = SEAP_msg_get(msg_in);
	probe_out = NULL;

	if (probe_in == NULL) {
		*ret = PROBE_ENOOBJ;
		return (NULL);
	}

	set = probe_obj_getent(probe_in, "set", 1);

	if (set != NULL) {
		/* set object */
		probe_out = probe_set_eval(probe, set, 0);
		SEXP_free(set);
		// todo: in case of an internal error set probe_ret accordingly
		*ret = 0;
	} else {
		SEXP_t *varrefs, *filters;

		/* simple object */
		varrefs = probe_obj_getent(probe_in, "varrefs", 1);
                filters = probe_prepare_filters(probe, probe_in);

		if (varrefs == NULL || !OSCAP_GSYM(varref_handling)) {
			probe_out = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL);
			*ret = probe_main(probe_in, probe_out, probe->probe_arg, filters);
			probe_cobj_compute_flag(probe_out);
		} else {
			/*
			 * there are variable references in the object.
			 * create ctx, iterate through all variable combinations
			 */
			struct probe_varref_ctx *ctx;

			dI("handling varrefs in object\n");

			probe_varref_create_ctx(probe_in, varrefs, &ctx);
			SEXP_free(varrefs);

			do {
				SEXP_t *cobj, *r0;

				cobj = probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, NULL, NULL);
				*ret = probe_main(ctx->pi2, cobj, probe->probe_arg, filters);
				probe_cobj_compute_flag(cobj);
				r0 = probe_out;
				probe_out = probe_set_combine(r0, cobj, OVAL_SET_OPERATION_UNION);
				SEXP_vfree(cobj, r0, NULL);
			} while (*ret == 0
				 && probe_varref_iterate_ctx(ctx));

			probe_varref_destroy_ctx(ctx);
		}

                SEXP_free(filters);
	}

	SEXP_free(probe_in);
	SEXP_VALIDATE(probe_out);

	return (probe_out);
}
