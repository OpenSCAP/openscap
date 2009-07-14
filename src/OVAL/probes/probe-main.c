#include <probe.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"

#ifndef _A
#define _A(x) assert(x)
#endif

globals_t global = GLOBALS_INITIALIZER;

void   *probe_init (void);
void    probe_fini (void *arg);
SEXP_t *probe_main (SEXP_t *object, int *err, void *arg);
void   *probe_worker (void *arg);

#define MAX_EVAL_DEPTH 8

SEXP_t *SEXP_OVALset_eval (SEXP_t *set, size_t depth)
{
        const char *str;

        SEXP_t *filters, *items, *item, *res_items;
                
        if (depth > MAX_EVAL_DEPTH) {
                _D("Too many levels: max=%zu\n", MAX_EVAL_DEPTH);
                return (NULL);
        }
        
        items = SEXP_list_new ();
        filters = SEXP_list_new ();
        
        /* Get items */
        SEXP_sublist_foreach (item, set, 2, -1) {
                str = SEXP_string_cstrp (item);
                
                if (str != NULL) {
                        switch (*str) {
                        case 's':
                                if (SEXP_strcmp (item, "set") == 0) {
                                        SEXP_t *subset;
                                        
                                        subset = SEXP_OVALset_eval (item, depth + 1);
                                        
                                        if (subset != NULL) {
                                                SEXP_list_join (items, subset);
                                        }
                                }
                                break;
                        case 'o':
                                if (SEXP_strcmp (item, "obj_ref") == 0) {
                                        SEXP_t *resobj, *id;

                                        id = SEXP_OVALelm_getattrval (item, "id");
                                        
                                        if (id == NULL) {
                                                _D("Missing object id!\n");
                                                break;
                                        }
                                        
                                        resobj = pcache_sexp_get (global.pcache, id);
                                        
                                        if (resobj == NULL) {
                                                /* Request object */
                                        }
                                        
                                        SEXP_list_join (items, resobj);
                                }
                                break;
                        case 'f':
                                if (SEXP_strcmp (item, "filter") == 0) {
                                        SEXP_t *resfilter, *id;
                                        
                                        id = SEXP_OVALelm_getattrval (item, "id");

                                        if (id == NULL) {
                                                _D("Missing object id!\n");
                                                break;
                                        }

                                        resfilter = pcache_sexp_get (global.pcache, id);

                                        if (resfilter == NULL) {
                                                /* Request filter */
                                        }

                                        SEXP_list_add (filters, resfilter);
                                }
                                break;
                        default:
                                _D("Unexpected set element: %.*s\n",
                                   SEXP_string_length (item), str);
                        }
                }
        }
        
        /* Apply filters to items */
        
        return (res_items);
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
