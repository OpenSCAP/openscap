
/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
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

#ifndef __STUB_PROBE
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <config.h>
#include <alloc.h>
#include "_probe-api.h"
#include "probe-entcmp.h"
#include "encache.h"

#ifndef _A
#define _A(x) assert(x)
#endif

globals_t global = GLOBALS_INITIALIZER;

void *probe_worker(void *arg);

#define MAX_EVAL_DEPTH 8

static SEXP_t *probe_ste_fetch(SEXP_t * id_list)
{
	SEXP_t *res, *ste, *id;
	uint32_t i_len, r_len;

	_LOGCALL_;

	i_len = SEXP_list_length(id_list);

	if (i_len == 0)
		return SEXP_list_new(NULL);

	res = SEAP_cmd_exec(global.ctx, global.sd, 0, PROBECMD_STE_FETCH, id_list, SEAP_CMDTYPE_SYNC, NULL, NULL);

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

		if (pcache_sexp_add(global.pcache, id, ste) != 0) {

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

static SEXP_t *probe_obj_eval(SEXP_t * id)
{
	SEXP_t *res;

	_LOGCALL_;

	res = SEAP_cmd_exec(global.ctx, global.sd, 0, PROBECMD_OBJ_EVAL, id, SEAP_CMDTYPE_SYNC, NULL, NULL);

	SEXP_free(res);

	return pcache_sexp_get(global.pcache, id);
}

static SEXP_t *probe_set_combine(SEXP_t *cobj1, SEXP_t *cobj2, oval_setobject_operation_t op)
{
	char append;
	SEXP_t *res_items, *item1, *item2, *id1, *id2;
	SEXP_t *res_cobj, *item_lst1, *item_lst2;
	oval_syschar_collection_flag_t flag1, flag2, res_flag;

	_LOGCALL_;

	if (cobj1 == NULL)
		return SEXP_ref(cobj2);
	if (cobj2 == NULL)
		return SEXP_ref(cobj1);

	item_lst1 = _probe_cobj_get_items(cobj1);
	item_lst2 = _probe_cobj_get_items(cobj2);
	flag1 = _probe_cobj_get_flag(cobj1);
	flag2 = _probe_cobj_get_flag(cobj2);

	res_flag = _probe_cobj_combine_flags(flag1, flag2, op);
	res_items = SEXP_list_new(NULL);

	switch (op) {
	case OVAL_SET_OPERATION_INTERSECTION:
		SEXP_list_foreach(item1, item_lst1) {
			id1 = probe_obj_getentval(item1, "id", 1);
			append = 0;

			SEXP_list_foreach(item2, item_lst2) {
				id2 = probe_obj_getentval(item2, "id", 1);

				if (!SEXP_string_cmp(id1, id2)) {
					append = 1;

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
			append = 1;

			SEXP_list_foreach(item2, item_lst2) {
				id2 = probe_obj_getentval(item2, "id", 1);

				if (!SEXP_string_cmp(id1, id2)) {
					append = 0;

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
		_D("Unexpected set operation: %d\n", op);
		SEXP_vfree(item_lst1, item_lst2, res_items, NULL);

		return NULL;
	}

	res_cobj = _probe_cobj_new(res_flag, res_items);
	SEXP_vfree(item_lst1, item_lst2, res_items, NULL);

	// todo: variables

	return res_cobj;
}

static SEXP_t *probe_set_apply_filters(SEXP_t *cobj, SEXP_t *filters)
{
	int filtered, i;
	SEXP_t *result_items, *items, *item, *filter, *felm, *ielm;
	SEXP_t *ste_res, *elm_res, *stmp;
	char *elm_name;
	oval_syschar_status_t item_status;
	oval_result_t ores;
	oval_check_t ochk;
	oval_operator_t oopr;
	oval_syschar_collection_flag_t flag;

	_LOGCALL_;

	result_items = SEXP_list_new(NULL);
	flag = _probe_cobj_get_flag(cobj);
	items = _probe_cobj_get_items(cobj);

	SEXP_list_foreach(item, items) {
		item_status = probe_ent_getstatus(item);

		switch (item_status) {
		case OVAL_STATUS_DOESNOTEXIST:
			continue;
		case OVAL_STATUS_ERROR:
		case OVAL_STATUS_NOTCOLLECTED:
			_D("Supplied item has an invalid status: %d\n", item_status);
			SEXP_vfree(items, result_items, NULL);

			return NULL;
		default:
			break;
		}

		filtered = 0;

		SEXP_list_foreach(filter, filters) {
			ste_res = SEXP_list_new(NULL);

			SEXP_sublist_foreach(felm, filter, 2, -1) {
				elm_res = SEXP_list_new(NULL);

				elm_name = probe_ent_getname(felm);

				for (i = 1;; ++i) {
					ielm = probe_obj_getent(item, elm_name, i);

					if (ielm == NULL)
						break;

					ores = probe_entste_cmp(felm, ielm);
					SEXP_list_add(elm_res, stmp = SEXP_number_newi_32(ores));

					SEXP_free(ielm);
					SEXP_free(stmp);
				}

				stmp = probe_ent_getattrval(felm, "entity_check");

				if (stmp == NULL)
					ochk = OVAL_CHECK_ALL;
				else
					ochk = SEXP_number_geti_32(stmp);

				SEXP_free(stmp);

				ores = probe_ent_result_bychk(elm_res, ochk);
				SEXP_list_add(ste_res, stmp = SEXP_number_newi_32(ores));
				SEXP_free(stmp);
			}

			stmp = probe_ent_getattrval(filter, "operator");

			if (stmp == NULL)
				oopr = OVAL_OPERATOR_AND;
			else
				oopr = SEXP_number_geti_32(stmp);

			SEXP_free(stmp);
			ores = probe_ent_result_byopr(ste_res, oopr);
			SEXP_free(ste_res);

			if (ores == OVAL_RESULT_TRUE) {
				filtered = 1;
				SEXP_free(filter);
				break;
			}
		}

		if (!filtered) {
			SEXP_list_add(result_items, item);
		}
	}

	cobj = _probe_cobj_new(flag, result_items);
	SEXP_vfree(items, result_items, NULL);

	return cobj;
}

static SEXP_t *probe_set_eval(SEXP_t * set, size_t depth)
{
	SEXP_t *filters_u, *filters_a;

	SEXP_t *s_subset[2];
	size_t s_subset_i;
	SEXP_t *o_subset[2];
	size_t o_subset_i;

	SEXP_t *member;
	char member_name[24];

	SEXP_t *op_val;
	int op_num;

	SEXP_t *result;

	_LOGCALL_;

	if (depth > MAX_EVAL_DEPTH) {
		_D("Too many levels: max=%zu\n", MAX_EVAL_DEPTH);
#ifndef NDEBUG
		abort();
#endif
		return (NULL);
	}

	filters_u = SEXP_list_new(NULL);	/* unavailable filters */
	filters_a = SEXP_list_new(NULL);	/* available filters (cached) */

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
			_D("FAIL: Invalid set element: ptr=%p, type=%s\n", member, SEXP_strtype(member));
			goto eval_fail;
		}

		switch (member_name[0]) {
			CASE('s', "et") {
				if (s_subset_i < 2) {
					s_subset[s_subset_i] = probe_set_eval(member, depth + 1);

					if (s_subset[s_subset_i] == NULL) {
						_D("FAIL: recursive set evaluation failer: m=%p, d=%u\n",
						   member, depth + 1);
						goto eval_fail;
					}

					++s_subset_i;
				} else {
					_D("FAIL: more than 2 \"set\"\n");
					goto eval_fail;
				}
			}
			break;

			CASE('o', "bj_ref") {
				SEXP_t *id, *res;

				id = probe_ent_getval(member);

				if (id == NULL) {
					_D("FAIL: set=%p: missing obj_ref value\n", set);
					goto eval_fail;
				}

				res = pcache_sexp_get(global.pcache, id);

				if (res == NULL) {
					/* cache miss */
					res = probe_obj_eval(id);

					if (res == NULL) {
#if !defined(NDEBUG)
						char *tmp = SEXP_string_cstr(id);
						_D("FAIL: obj=%s: evaluation failed.\n", tmp);
						oscap_free(tmp);
#endif
						SEXP_free(id);
						goto eval_fail;
					}
				}

				SEXP_free(id);

				if (o_subset_i < 2) {
					o_subset[o_subset_i] = res;
					++o_subset_i;
				} else {
					_D("FAIL: more than 2 obj_refs\n");

					SEXP_free(res);
					goto eval_fail;
				}

			}
			break;

			CASE('f', "ilter") {
				SEXP_t *id, *res;

				id = probe_ent_getval(member);

				if (id == NULL) {
					_D("FAIL: set=%p: missing filter value\n", set);
					goto eval_fail;
				}

				res = pcache_sexp_get(global.pcache, id);

				if (res == NULL)
					SEXP_list_add(filters_u, id);
				else
					SEXP_list_add(filters_a, res);

				SEXP_free(id);
				SEXP_free(res);

			}
			break;
		default:
			_D("Unexpected set element: %s\n", member_name);
			goto eval_fail;
		}
#undef CASE

	}

	member = NULL;

	/* request filters */
	result = probe_ste_fetch(filters_u);

	if (result == NULL) {
#if !defined(NDEBUG)
		_D("FAIL: can't get unavailable filters:\n");
		SEXP_list_foreach(result, filters_u) {
			SEXP_fprintfa(stdout, result);
			printf("\n");
		}
#endif
		goto eval_fail;
	}

	{
		SEXP_t *filters_j;

		filters_j = SEXP_list_join(filters_a, result);

		SEXP_free(filters_a);
		SEXP_free(result);

		filters_a = filters_j;
	}

	_A((s_subset_i > 0 || o_subset_i > 0));
	_A((s_subset_i > 0 && o_subset_i == 0) || (s_subset_i == 0 && o_subset_i > 0));

	if (o_subset_i > 0) {
		for (s_subset_i = 0; s_subset_i < o_subset_i; ++s_subset_i) {
			s_subset[s_subset_i] = probe_set_apply_filters(o_subset[s_subset_i], filters_a);

#ifndef NDEBUG
			if (s_subset[s_subset_i] == NULL) {
				_D("FAIL: apply_filters returned NULL: set=%p, filters=%p\n",
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

	return (NULL);
}

int main(void)
{
	int ret = EXIT_SUCCESS;
	SEAP_msg_t *seap_request, *seap_reply;

	SEXP_t *probe_in, *probe_out = NULL;
	int probe_ret;

	SEXP_t *oid;

	pthread_attr_t thread_attr;
	pthread_t thread;

	/* Initialize SEAP */
	global.ctx = SEAP_CTX_new();
	global.sd = SEAP_openfd2(global.ctx, STDIN_FILENO, STDOUT_FILENO, 0);

	if (global.sd < 0) {
		_D("Can't create SEAP descriptor: errno=%u, %s.\n", errno, strerror(errno));
		exit(errno);
	}

	/* Create cache */
	global.pcache = pcache_new();
	if (global.pcache == NULL) {
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

	global.probe_arg = probe_init();

        /* Create the element name cache */
        global.encache = encache_new ();

	/* Main loop */
	for (;;) {
		if (SEAP_recvmsg(global.ctx, global.sd, &seap_request) == -1) {
			ret = errno;

			_D("An error ocured while receiving SEAP message. errno=%u, %s.\n", errno, strerror(errno));

			break;
		}

/*
#ifndef NDEBUG
                SEAP_msg_print (stderr, seap_request);
#endif  
*/
		probe_in = SEAP_msg_get(seap_request);
		if (probe_in == NULL) {
			_D("Unexpected error: probe_in = NULL\n");
			abort();
		}

		SEXP_VALIDATE(probe_in);

		oid = probe_obj_getattrval(probe_in, "id");

		if (oid == NULL) {
			_D("Invalid object: %s\n", "attribute \"id\" not set\n");
			probe_ret = PROBE_ENOATTR;
		} else {
			SEXP_VALIDATE(oid);

			probe_out = pcache_sexp_get(global.pcache, oid);
			if (probe_out == NULL) {
				/* cache miss */

				if (pthread_create(&thread, &thread_attr, &probe_worker, (void *)seap_request) != 0) {
					_D("Can't start new probe worker: %u, %s.\n", errno, strerror(errno));

					/* send error */
					continue;
				}

				seap_request = NULL;

				_D("New worker: id=%u, in=%p\n", thread, probe_in);

				continue;
			} else {
				/* cache hit */
				probe_ret = 0;
			}
		}

		if (probe_out == NULL || probe_ret != 0) {
			if (SEAP_replyerr(global.ctx, global.sd, seap_request, probe_ret) == -1) {
				_D("An error ocured while sending error status. errno=%u, %s.\n",
				   errno, strerror(errno));

				SEAP_msg_free(seap_request);
				break;
			}
		} else {
			SEXP_VALIDATE(probe_out);

			seap_reply = SEAP_msg_new();
			SEAP_msg_set(seap_reply, probe_out);

			if (SEAP_reply(global.ctx, global.sd, seap_reply, seap_request) == -1) {
				ret = errno;

				_D("An error ocured while sending SEAP message. errno=%u, %s.\n",
				   errno, strerror(errno));

				SEAP_msg_free(seap_reply);
				SEAP_msg_free(seap_request);

				break;
			}

			SEAP_msg_free(seap_reply);
		}

		SEAP_msg_free(seap_request);
	}

	probe_fini(global.probe_arg);
	pcache_free(global.pcache);
	SEAP_close(global.ctx, global.sd);
	SEAP_CTX_free(global.ctx);

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

	ctx = malloc(sizeof(struct probe_varref_ctx));
	ctx->pi2 = SEXP_ref(probe_in);
	ctx->ent_cnt = ent_cnt;
	ctx->ent_lst = malloc(ent_cnt * sizeof(struct probe_varref_ctx_ent));

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

		SEXP_sublist_foreach(varref, varrefs, 4, -1) {
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
		*next_val_idx = 0;
		r1 = SEXP_list_replace(ent_name_sref, 3, r0);
		SEXP_free(r1);

		if (ent == ent_end) {
			SEXP_free(r0);
			return 0;
		}

		++ent;
		val_cnt = ent->val_cnt;
		next_val_idx = &ent->next_val_idx;
		ent_name_sref = ent->ent_name_sref;
	}
	r1 = SEXP_list_replace(ent_name_sref, 3, r2 = SEXP_number_newu(*next_val_idx));
	SEXP_vfree(r0, r1, r2, NULL);

	return 1;
}

void *probe_worker(void *arg)
{
	int probe_ret;
	SEXP_t *probe_in, *set, *probe_out;
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
		probe_ret = (probe_out == NULL ? PROBE_ESETEVAL : 0);
	} else {
		/* simple object */
		varrefs = probe_obj_getent(probe_in, "varrefs", 1);

		if (varrefs == NULL) {
			SEXP_t *r0;

			_D("probe_main1\n");
			probe_ret = -1;
			probe_out = r0 = probe_main(probe_in, &probe_ret, global.probe_arg);
			if (r0 != NULL) {
				probe_out = _probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, r0);
				SEXP_free(r0);
			}
			_A(probe_ret != -1);
		} else {
			/*
			 * there are variable references in the object.
			 * create ctx, iterate through all variable combinations
			 */
			SEXP_t *cobj = NULL;
			SEXP_t *r0, *r1;
			struct probe_varref_ctx *ctx;

			_D("probe_main2\n");

			probe_varref_create_ctx(probe_in, varrefs, &ctx);
			SEXP_free(varrefs);

			do {
				probe_ret = -1;
				probe_out = probe_main(ctx->pi2, &probe_ret, global.probe_arg);
				_A(probe_ret != -1);

				if (probe_out == NULL || probe_ret != 0) {
					SEXP_free(cobj);
					cobj = NULL;
					break;
				}

				r0 = _probe_cobj_new(SYSCHAR_FLAG_UNKNOWN, probe_out);
				r1 = cobj;
				cobj = probe_set_combine (r0, r1, OVAL_SET_OPERATION_UNION);

				SEXP_vfree (probe_out, r0, r1, NULL);
			} while (probe_varref_iterate_ctx(ctx));

			probe_out = cobj;
			probe_varref_destroy_ctx(ctx);
		}
	}

	_D("probe_out = %p, probe_ret = %d\n", (void *)probe_out, probe_ret);

	if (probe_out == NULL || probe_ret != 0) {
		if (SEAP_replyerr(global.ctx, global.sd, seap_request, probe_ret) == -1) {
			int ret = errno;

			_D("An error ocured while sending error status. errno=%u, %s.\n", errno, strerror(errno));

			SEAP_msg_free(seap_request);

			/* FIXME */
			exit(ret);
		}
	} else {
		SEXP_t *oid;

		SEXP_VALIDATE(probe_out);

		oid = probe_obj_getattrval(probe_in, "id");
		_A(oid != NULL);

		if (pcache_sexp_add(global.pcache, oid, probe_out) != 0) {
			/* TODO */
			abort();
		}

		SEXP_free(oid);

		seap_reply = SEAP_msg_new();
		SEAP_msg_set(seap_reply, probe_out);

		if (SEAP_reply(global.ctx, global.sd, seap_reply, seap_request) == -1) {
			int ret = errno;

			_D("An error ocured while sending SEAP message. errno=%u, %s.\n", errno, strerror(errno));

			SEAP_msg_free(seap_reply);
			SEAP_msg_free(seap_request);

			exit(ret);
		}

		SEAP_msg_free(seap_reply);
	}

	SEAP_msg_free(seap_request);
	return (NULL);
}
#endif
