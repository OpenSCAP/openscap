#include <probe.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <config.h>
#include <common/alloc.h>

#ifndef _A
#define _A(x) assert(x)
#endif

globals_t global = GLOBALS_INITIALIZER;

void   *probe_init (void);
void    probe_fini (void *arg);
SEXP_t *probe_main (SEXP_t *object, int *err, void *arg);
void   *probe_worker (void *arg);

#define MAX_EVAL_DEPTH 8

SEXP_t *SEXP_OVALste_fetch (SEXP_t *id_list)
{
        SEXP_t *res, *ste, *id;
        uint32_t i_len, r_len;
                
        i_len = SEXP_list_length (id_list);
        res   = SEAP_cmd_exec (global.ctx, global.sd, 0, PROBECMD_STE_FETCH,
                             id_list, SEAP_CMDTYPE_SYNC, NULL, NULL);
        
        r_len = SEXP_list_length (res);
        
        if (i_len != r_len) {
                SEXP_list_free (res);
                return (NULL);
        }
        
        for (; i_len > 0; --i_len) {
                ste = SEXP_list_nth (res, i_len);
                id  = SEXP_list_nth (id_list, i_len); 

                _A(id  != NULL);
                _A(ste != NULL);

                if (pcache_sexp_add (global.pcache, id, ste) != 0) {
                        SEXP_list_free (res);
                        return (NULL);
                }
        }
        
        return (res);
}

SEXP_t *SEXP_OVALobj_eval (SEXP_t *id)
{
        SEXP_t *res;

        res = SEAP_cmd_exec (global.ctx, global.sd, 0, PROBECMD_OBJ_EVAL,
                             id, SEAP_CMDTYPE_SYNC, NULL, NULL);
        
        return pcache_sexp_get (global.pcache, id);
}

SEXP_t *SEXP_OVALset_combine(SEXP_t *item_lst1, SEXP_t *item_lst2, oval_set_operation_enum op)
{
	char append;
	SEXP_t *res_items, *item1, *item2, *id1, *id2;

	if (SEXP_list_length(item_lst2) == 0)
		return item_lst1;

	res_items = SEXP_list_new();
	switch (op) {
	case OVAL_SET_OPERATION_INTERSECTION:
		SEXP_list_foreach(item1, item_lst1) {
			id1 = SEXP_OVALobj_getelmval(item1, "id", 1, 1);
			append = 0;
			SEXP_list_foreach(item2, item_lst2) {
				id2 = SEXP_OVALobj_getelmval(item2, "id", 1, 1);
				if (!SEXP_string_cmp(id1, id2)) {
					append = 1;
					break;
				}
			}
			if (append) {
				SEXP_list_add(res_items, item1);
			}
		}
		break;
	case OVAL_SET_OPERATION_UNION:
		SEXP_list_join(res_items, item_lst2);
		/* fall through */
	case OVAL_SET_OPERATION_COMPLEMENT:
		SEXP_list_foreach(item1, item_lst1) {
			id1 = SEXP_OVALobj_getelmval(item1, "id", 1, 1);
			append = 1;
			SEXP_list_foreach(item2, item_lst2) {
				id2 = SEXP_OVALobj_getelmval(item2, "id", 1, 1);
				if (!SEXP_string_cmp(id1, id2)) {
					append = 0;
					break;
				}
			}
			if (append) {
				SEXP_list_add(res_items, item1);
			}
		}
		break;
	default:
		_D("Unexpected set operation: %d\n", op);
		return NULL;
	}

	// todo: set result flags
	// todo: variables

	return res_items;
}

SEXP_t *SEXP_OVALset_apply_filters(SEXP_t *items, SEXP_t *filters)
{
	int filtered, i;
	SEXP_t *result_items, *item, *filter, *felm, *ielm;
	SEXP_t *ste_res, *elm_res, *stmp;
	char *elm_name;
	oval_syschar_status_enum item_status;
	oval_result_enum ores;
	oval_check_enum ochk;
	oval_operator_enum oopr;

	result_items = SEXP_list_new();

	SEXP_list_foreach (item, items) {
		item_status = SEXP_OVALelm_getstatus(item);
		switch(item_status) {
		case OVAL_STATUS_DOESNOTEXIST:
			continue;
		case OVAL_STATUS_ERROR:
		case OVAL_STATUS_NOTCOLLECTED:
			_D("Supplied item has an invalid status: %d\n", item_status);
			SEXP_free(result_items);

			return NULL;
		default:
			break;
		}

		filtered = 0;

		SEXP_list_foreach (filter, filters) {
			ste_res = SEXP_list_new();

			SEXP_sublist_foreach(felm, filter, 2, -1) {
				elm_res = SEXP_list_new();
				stmp = SEXP_OVALelm_getval(felm, 0);
				elm_name = SEXP_string_cstr(stmp);

				for (i = 1; ; ++i) {
					ielm = SEXP_OVALobj_getelm(item, elm_name, i);
					if (ielm == NULL)
						break;
					ores = SEXP_OVALentste_cmp(felm, ielm);
					SEXP_list_add(elm_res, SEXP_number_newd(ores));
				}

				stmp = SEXP_OVALelm_getattrval(felm, "entity_check");
				if (stmp == NULL)
					ochk = OVAL_CHECK_ALL;
				else
					ochk = SEXP_number_getd(stmp);
				ores = SEXP_OVALent_result_bychk(elm_res, ochk);
				SEXP_list_add(ste_res, SEXP_number_newd(ores));
				// todo: var_check
			}

			stmp = SEXP_OVALelm_getattrval(filter, "operator");
			if (stmp == NULL)
				oopr = OPERATOR_AND;
			else
				oopr = SEXP_number_getd(stmp);
			ores = SEXP_OVALent_result_byopr(ste_res, oopr);
			if (ores == OVAL_RESULT_TRUE) {
				filtered = 1;
				break;
			}
		}

		if (!filtered) {
			SEXP_list_add(result_items, item);
		}
	}

	return result_items;
}

SEXP_t *SEXP_OVALset_eval (SEXP_t *set, size_t depth)
{
        SEXP_t *filters_u, *filters_a;
        
        SEXP_t *s_subset[2];
        size_t  s_subset_i;
        SEXP_t *o_subset[2];
        size_t  o_subset_i;
        
        SEXP_t *member;
        char    member_name[24];
        
        SEXP_t *op_val;
        int     op_num;
        
        SEXP_t *result;
        
        if (depth > MAX_EVAL_DEPTH) {
                _D("Too many levels: max=%zu\n", MAX_EVAL_DEPTH);
                return (NULL);
        }
                        
        filters_u = SEXP_list_new (); /* unavailable filters */
        filters_a = SEXP_list_new (); /* available filters (cached) */
        
        s_subset[0] = NULL;
        s_subset[1] = NULL;
        s_subset_i = 0;
        
        o_subset[0] = NULL;
        o_subset[1] = NULL;
        o_subset_i = 0;
        
        result = NULL;
        
        op_val = SEXP_OVALelm_getattrval (set, "operation");
        
        if (op_val != NULL)
                op_num = SEXP_number_getd (op_val);
        else 
                op_num = OVAL_SET_OPERATION_UNION;
        
        _A(op_num == OVAL_SET_OPERATION_UNION       ||
           op_num == OVAL_SET_OPERATION_COMPLEMENT  ||
           op_num == OVAL_SET_OPERATION_INTERSECTION);
        
#define SEXP_OVALset_foreach(elm_var, set_list) SEXP_sublist_foreach (elm_var, set_list, 2, 1000)
        
        SEXP_OVALset_foreach (member, set) {
                if (SEXP_OVALelm_name_cstr_r (member,
                                              member_name, sizeof member_name) == NULL)
                {
                        _D("FAIL: Invalid set element: ptr=%p, type=%s\n", member, SEXP_strtype (member));
                        goto eval_fail;
                }
                
#define CASE(__c1, __rest) case (__c1): if (strcmp (__rest, member_name + 1) == 0)
                
                switch (member_name[0]) {
                        CASE ('s',"et") {
                                if (s_subset_i < 2) {
                                        s_subset[s_subset_i] = SEXP_OVALset_eval (member, depth + 1);
                                        ++s_subset_i;
                                } else {
                                        _D("FAIL: more than 2 \"set\"\n");
                                                goto eval_fail;
                                }
                        } break;
                        
                        CASE ('o', "bj_ref") {
                                SEXP_t *id, *res;
                                
                                id = SEXP_OVALelm_getval (member, 1);
                                
                                if (id == NULL) {
                                        _D("FAIL: set=%p: missing obj_ref value\n", set);
                                        goto eval_fail;
                                }
                                
                                res = pcache_sexp_get (global.pcache, id);
                                
                                if (res == NULL) {
                                        /* cache miss */
                                        res = SEXP_OVALobj_eval (id);
                                        
                                        if (res == NULL) {
#if !defined(NDEBUG)
                                                char *tmp = SEXP_string_cstr (id);
                                                _D("FAIL: obj=%s: evaluation failed.\n", tmp);
                                                oscap_free (tmp);
#endif
                                                goto eval_fail;
                                        }
                                }       
                                 
                                if (o_subset_i < 2) {
                                        o_subset[o_subset_i] = res;
                                        ++o_subset_i;
                                } else {
                                        _D("FAIL: more than 2 obj_refs\n");
                                        goto eval_fail;
                                }
                                                                
                        } break;
                        
                        CASE ('f', "ilter") {
                                SEXP_t *id, *res;
                                        
                                id = SEXP_OVALelm_getval (member, 1);
                                        
                                if (id == NULL) {
                                        _D("FAIL: set=%p: missing filter value\n", set);
                                        goto eval_fail;
                                }
                                        
                                res = pcache_sexp_get (global.pcache, id);
                                        
                                if (res == NULL)
                                        SEXP_list_add (filters_u, id);
                                else
                                        SEXP_list_add (filters_a, res);
                                
                        } break;
                default:
                        _D("Unexpected set element: %s\n", member_name);
                        goto eval_fail;
                }
#undef CASE
                
        }
        
        /* request filters */
        result = SEXP_OVALste_fetch (filters_u);
        
        if (result == NULL) {
#if !defined(NDEBUG)
                _D("FAIL: can't get unavailable filters:\n");
                SEXP_list_foreach (result, filters_u) {
                        SEXP_printfa (result);
                        printf ("\n");
                }
#endif
                goto eval_fail;
        }
        
        filters_a = SEXP_list_join (filters_a, result);
        
        _A((s_subset_i > 0 || o_subset_i > 0));
        _A((s_subset_i > 0 && o_subset_i == 0)||
           (s_subset_i == 0 && o_subset_i > 0));
        
#if !defined(NDEBUG)
        { FILE *fp;
                
                fp = fopen ("seteval.log", "a");
                setbuf (fp, NULL);
                
                fprintf (fp, "\n--- FILTERS ---\n");
                SEXP_fprintfa (fp, filters_a);
                
                if (o_subset_i > 0) {
                        fprintf (fp, "\n--- O-ITEMS ---\n");

                        switch (o_subset_i) {
                        case 2:
                                fprintf (fp, "[1]\n");
                                SEXP_fprintfa (fp, o_subset[1]);
                        case 1:
                                fprintf (fp, "\n[0]\n");
                                SEXP_fprintfa (fp, o_subset[0]);
                        }
                } else {
                        fprintf (fp, "\n--- S-ITEMS ---\n");

                        switch (s_subset_i) {
                        case 2:
                                fprintf (fp, "[1]\n");
                                SEXP_fprintfa (fp, s_subset[1]);
                        case 1:
                                fprintf (fp, "\n[0]\n");
                                SEXP_fprintfa (fp, s_subset[0]);
                        }
                }
                                
                fprintf (fp, "\n---------------\n");
                fclose (fp);
        }
#endif
        
        if (o_subset_i > 0) {
                for (s_subset_i = 0; s_subset_i < o_subset_i; ++s_subset_i)
                        s_subset[s_subset_i] = SEXP_OVALset_apply_filters (o_subset[s_subset_i], filters_a);
        }
        
        result = SEXP_OVALset_combine (s_subset[0], s_subset[1], op_num);
                
        return (result);
eval_fail:
        /*
        for (; s_subset_i > 0; --s_subset_i)
                SEXP_list_free (s_subset[s_subset_i - 1]);
        
        for (; o_subset_i > 0; --o_subset_i)
                SEXP_list_free_nr (o_subset[o_subset_i - 1]);
        
        SEXP_list_free (filters_u);
        SEXP_list_free_nr (filters_a);
        SEXP_list_free (result);
        */
        return (NULL);
}

int main (void)
{
        int ret = EXIT_SUCCESS;
        SEAP_msg_t *seap_request, *seap_reply;

        SEXP_t *probe_in, *probe_out;
        int probe_ret;
        
        SEXP_t *oid;
        
        pthread_attr_t thread_attr;
        pthread_t      thread;

        /* Initialize SEAP */
        global.ctx = SEAP_CTX_new ();
        global.sd  = SEAP_openfd2 (global.ctx, STDIN_FILENO, STDOUT_FILENO, 0);
        
        if (global.sd < 0) {
                _D("Can't create SEAP descriptor: errno=%u, %s.\n",
                   errno, strerror (errno));
                exit (errno);
        }
        
        /* Create cache */
        global.pcache = pcache_new ();
        if (global.pcache == NULL) {
                _D("Can't create cache: %u, %s.\n",
                   errno, strerror (errno));
                exit (errno);
        }

        if (pthread_attr_init (&thread_attr) != 0) {
                _D("Can't initialize thread attributes: %u, %s.\n",
                   errno, strerror (errno));
                exit (errno);
        }
        
        global.probe_arg = probe_init ();
        
        /* Main loop */
        for (;;) {
                if (SEAP_recvmsg (global.ctx, global.sd, &seap_request) == -1) {
                        ret = errno;
                        
                        _D("An error ocured while receiving SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        break;
                }
                
                probe_in = SEAP_msg_get (seap_request);
                if (probe_in == NULL) {
                        _D("Unexpected error: probe_in = NULL\n");
                        abort ();
                }
                
                SEXP_VALIDATE(probe_in);
                
                oid = SEXP_OVALobj_getattrval (probe_in, "id");
                
                if (oid == NULL) {
                        _D("Invalid object: %s\n", "attribute \"id\" not set\n");
                        probe_ret = PROBE_ENOATTR;
                } else {
                        SEXP_VALIDATE(oid);
                        
                        probe_out = pcache_sexp_get (global.pcache, oid);
                        if (probe_out == NULL) {
                                /* cache miss */

                                if (pthread_create (&thread, &thread_attr,
                                                    &probe_worker, (void *)seap_request) != 0)
                                {
                                        _D("Can't start new probe worker: %u, %s.\n",
                                           errno, strerror (errno));
                                        
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
                        if (SEAP_replyerr (global.ctx, global.sd, seap_request, probe_ret) == -1) {
                                _D("An error ocured while sending error status. errno=%u, %s.\n",
                                   errno, strerror (errno));
                                
                                SEAP_msg_free (seap_request);
                                break;
                        }
                } else {
                        SEXP_VALIDATE(probe_out);

                        seap_reply = SEAP_msg_new ();
                        SEAP_msg_set (seap_reply, probe_out);
                        
                        if (SEAP_reply (global.ctx, global.sd, seap_reply, seap_request) == -1) {
                                ret = errno;
                                
                                _D("An error ocured while sending SEAP message. errno=%u, %s.\n",
                                   errno, strerror (errno));
                                
                                SEAP_msg_free (seap_reply);
                                SEAP_msg_free (seap_request);
                                
                                break;
                        }
                        
                        SEAP_msg_free (seap_reply);
                }
                
                SEAP_msg_free (seap_request);
        }
        
        probe_fini (global.probe_arg);
        pcache_free (global.pcache);
        SEAP_close (global.ctx, global.sd);
        SEAP_CTX_free (global.ctx);
        
        return (ret);
}

void *probe_worker (void *arg)
{
        int     probe_ret;
        SEXP_t *probe_in, *set, *probe_out;
        
        SEAP_msg_t *seap_reply, *seap_request;

        seap_request = (SEAP_msg_t *)arg;
        probe_in     = SEAP_msg_get (seap_request);
        
        set = SEXP_OVALobj_getelm (probe_in, "set", 1);
        
        if (set != NULL) {
                /* complex object */
                probe_ret = 0;
                probe_out = SEXP_OVALset_eval (set, 0);
        } else {
                /* simple object */
                probe_ret = -1;
                probe_out = probe_main (probe_in, &probe_ret, global.probe_arg);
                _A(probe_ret != -1);
        }
        
        if (probe_out == NULL || probe_ret != 0) {
                if (SEAP_replyerr (global.ctx, global.sd, seap_request, probe_ret) == -1) {
                        int ret = errno;
                        
                        _D("An error ocured while sending error status. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        SEAP_msg_free (seap_request);
                        
                        /* FIXME */
                        exit (ret);
                }
        } else {
                SEXP_t *oid;
                
                SEXP_VALIDATE(probe_out);

                oid = SEXP_OVALobj_getattrval (probe_in, "id");
                _A(oid != NULL);
                
                if (pcache_sexp_add (global.pcache, oid, probe_out) != 0) {
                        /* TODO */
                }
                
                SEXP_free (oid);

                seap_reply = SEAP_msg_new ();
                SEAP_msg_set (seap_reply, probe_out);
                
                if (SEAP_reply (global.ctx, global.sd, seap_reply, seap_request) == -1) {
                        int ret = errno;

                        _D("An error ocured while sending SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        SEAP_msg_free (seap_reply);
                        SEAP_msg_free (seap_request);
                        
                        exit (ret);
                }
                
                SEAP_msg_free (seap_reply);
        }
        
        SEAP_msg_free (seap_request);
        return (NULL);
}
