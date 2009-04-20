#include <stdio.h>
#include <errno.h>
/* RPM headers */
#include <rpmdb.h>
#include <rpmts.h>
#include <rpmlib.h>
#include <rpmfileutil.h>
#include <rpmmacro.h>
/* SEAP */
#include <seap.h>
#include "probe.h"

static int get_rpminfo (struct rpminfo_req *req, struct rpminfo_rep *rep)
{
        return (-1);
}

int main (int argc, char *argv[])
{
        SEAP_CTX_t *sctx;
        int sd;
        SEAP_msg_t *request, *reply;
        struct rpminfo_req request_st;
        struct rpminfo_rep reply_st;
        
        rpmts RPMts;

        /* Initialize SEAP */
        sctx = SEAP_CTX_new ();
        sd   = SEAP_openfd2 (SCH_GENERIC, STDIN_FILENO, STDOUT_FILENO, 0);
        
        /* Initialize RPM db */
        if (rpmReadConfigFiles ((const char *) NULL,
                                (const char *) NULL) != 0)
        {
                _D("rpmReadConfigFiles failed: %u, %s.\n", errno, strerror (errno));
                exit (errno);
        }
        
        RPMts = rpmtsCreate ();

        /* Main loop */
        for (;;) {
                /* receive S-exp */
                if (SEAP_recvmsg (ctx, sd, &request) == -1) {
                        /* handle the error */
                }

                /* translate the received msg into struct */

                /* get info from RPM db */
                switch (get_rpminfo (&request_st, &reply_st)) {
                case RPMINFO_NOPKG: /* package not found */
                        
                        break;
                case RPMINFO_OK:
                        
                        break;
                default: /* error */
                        
                }
                
                /* translate generated structure info sexp/seap_msg */

                /* set reply attribute */

                /* send reply */
                if (SEAP_sendmsg (ctx, sd, reply) == -1) {
                        /* handle the error */
                }
        }

        /* Close SEAP descriptor */
        SEAP_close (ctx, sd);
        
        /* TODO: close the RPM db */

        return (0);
}
