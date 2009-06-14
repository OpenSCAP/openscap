#include <seap.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <config.h>
#include "xmalloc.h"
#include "common.h"

#ifndef _A
#define _A(x) assert(x)
#endif

SEXP_t *probe_main (SEXP_t *object, int *err);

int main (void)
{
        int ret = EXIT_SUCCESS;

        SEAP_CTX_t *ctx;
        SEAP_msg_t *seap_request, *seap_reply;
        int sd;

        SEXP_t *probe_in, *probe_out;
        int probe_ret;
        
        /* Initialize SEAP */
        ctx = SEAP_CTX_new ();
        sd  = SEAP_openfd2 (ctx, STDIN_FILENO, STDOUT_FILENO, 0);

        if (sd < 0) {
                _D("Can't create SEAP descriptor: errno=%u, %s.\n",
                   errno, strerror (errno));
                exit (errno);
        }
        
        /* Main loop */
        for (;;) {
                if (SEAP_recvmsg (ctx, sd, &seap_request) == -1) {
                        ret = errno;
                        
                        _D("An error ocured while receiving SEAP message. errno=%u, %s.\n",
                           errno, strerror (errno));
                        
                        break;
                }
                
                /* state and object */
                
                /* parse received object */
                
                /* what is it? */
                
                /* lookup in cache */
                
                /* call the probe */

                probe_in  = SEAP_msg_get (seap_request);
                probe_ret = -1;
                probe_out = probe_main (probe_in, &probe_ret);
                _A(probe_ret != -1);
                
                if (probe_out == NULL || probe_ret != 0) {
                        if (SEAP_replyerr (ctx, sd, seap_request, probe_ret) == -1) {
                                _D("An error ocured while sending error status. errno=%u, %s.\n",
                                   errno, strerror (errno));
                                
                                SEAP_msg_free (seap_request);
                                break;
                        }
                } else {
                        seap_reply = SEAP_msg_new ();
                        SEAP_msg_set (seap_reply, probe_out);
                        
                        if (SEAP_reply (ctx, sd, seap_reply, seap_request) == -1) {
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
        
        SEAP_close (ctx, sd);
        SEAP_CTX_free (ctx);
        
        return (ret);
}
