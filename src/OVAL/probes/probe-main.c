#include <probe.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"

#ifndef _A
#define _A(x) assert(x)
#endif

globals_t global = GLOBALS_INITIALIZER;

SEXP_t *probe_main (SEXP_t *object, int *err);

int main (void)
{
        int ret = EXIT_SUCCESS;
        SEAP_msg_t *seap_request, *seap_reply;

        SEXP_t *probe_in, *probe_out;
        int probe_ret;
        
        SEXP_t *set, *oid;
        
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
                                set = SEXP_OVALobj_getelm (probe_in, "set", 1);
                                
                                if (set != NULL) {
                                        /* complex object */
                                        probe_ret = 0;
                                        probe_out = SEXP_OVALset_eval (set, 0);
                                } else {
                                        /* simple object */
                                        probe_ret = -1;
                                        probe_out = probe_main (probe_in, &probe_ret);
                                        _A(probe_ret != -1);
                                }
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
        
        pcache_free (global.pcache);
        SEAP_close (global.ctx, global.sd);
        SEAP_CTX_free (global.ctx);
        
        return (ret);
}
