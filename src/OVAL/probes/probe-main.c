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
        SEXP_t *res;
        
        res = SEAP_cmd_exec (global.ctx, global.sd, 0, PROBECMD_STE_FETCH,
                             id_list, SEAP_CMDTYPE_SYNC, NULL, NULL);
        
        /* add to cache */
        
        return (res);
}

SEXP_t *SEXP_OVALobj_eval (SEXP_t *id)
{
        SEXP_t *res;
        
        res = SEAP_cmd_exec (global.ctx, global.sd, 0, PROBECMD_OBJ_EVAL,
                             id, SEAP_CMDTYPE_SYNC, NULL, NULL);
        
        return pcache_sexp_get (global.pcache, id);
}

SEXP_t *SEXP_OVALset_eval (SEXP_t *set, size_t depth)
{
        const char *str;

        SEXP_t *filters_u, *filters_a;
        
        SEXP_t *s_subset[2];
        size_t  s_subset_i;
        SEXP_t *o_subset[2];
        size_t  o_subset_i;
        
        SEXP_t *member;
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
        
        /* Get items */
        SEXP_sublist_foreach (member, set, 2, -1) {
                str = SEXP_string_cstrp (member);
                
                if (str != NULL && SEXP_string_length (member) > 0) {
                        switch (*str) {
                        case 's':
                                if (SEXP_strcmp (member, "set") == 0) {
                                        if (s_subset_i < 2) {
                                                s_subset[s_subset_i] = SEXP_OVALset_eval (member, depth + 1);
                                                ++s_subset_i;
                                        } else {
                                                _D("FAIL: more than 2 \"set\"\n");
                                                goto eval_fail;
                                        }
                                        
                                        break;
                                }
                        case 'o':
                                if (SEXP_strcmp (member, "obj_ref") == 0) {
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
                                                
                                                if (o_subset_i < 2) {
                                                        o_subset[o_subset_i] = res;
                                                        ++o_subset_i;
                                                } else {
                                                        _D("FAIL: more than 2 obj_refs\n");
                                                        goto eval_fail;
                                                }
                                        }
                                        
                                        break;
                                }
                        case 'f':
                                if (SEXP_strcmp (member, "filter") == 0) {
                                        SEXP_t *id, *res;
                                        
                                        id = SEXP_OVALelm_getval (member, 1);
                                        
                                        if (id == NULL) {
                                                _D("FAIL: set=%p: missing obj_ref value\n", set);
                                                goto eval_fail;
                                        }
                                        
                                        res = pcache_sexp_get (global.pcache, id);
                                        
                                        if (res == NULL)
                                                SEXP_list_add (filters_u, id);
                                        else
                                                SEXP_list_add (filters_a, res);
                                        
                                        break;
                                }
                        default:
                                _D("Unexpected set element: %.*s\n",
                                   SEXP_string_length (member), str);
                                goto eval_fail;
                        }
                } else {
                        _D("FAIL: Invalid set element: str=%p, len=%zu\n", str, SEXP_string_length (member));
                        goto eval_fail;
                }
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
        
        /* Apply filters to items */
        /* set operation */
        
        return (result);
eval_fail:
        
        for (; s_subset_i > 0; --s_subset_i)
                SEXP_list_free (s_subset[s_subset_i - 1]);
        
        for (; o_subset_i > 0; --o_subset_i)
                SEXP_list_free_nr (o_subset[o_subset_i - 1]);
        
        SEXP_list_free (filters_u);
        SEXP_list_free_nr (filters_a);
        SEXP_list_free (result);
        
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
                                                    &probe_worker, (void *)probe_in) != 0)
                                {
                                        _D("Can't start new probe worker: %u, %s.\n",
                                           errno, strerror (errno));
                                        
                                        /* send error */
                                        continue;
                                }
                                
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
        
        probe_in = (SEXP_t *)arg;
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
        
        return (NULL);
}
