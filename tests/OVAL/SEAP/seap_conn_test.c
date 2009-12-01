#include <stdio.h>
#include <seap.h>
#include <errno.h>
#include <libgen.h>

#define ERR(...) fprintf (stderr, __VA_ARGS__)
#define INF(...) fprintf (stdout, __VA_ARGS__)

int main (int argc, char *argv[])
{
        SEAP_CTX_t    *ctx;
        SEXP_t        *sexp;
        SEXP_pstate_t *sexp_pstate;
        int sd;
        
        if (argc != 2) {
                fprintf (stderr, "Usage: %s <uri>\n", basename (argv[0]));
                exit (1);
        }
        
        /* Create context */
        ctx = SEAP_CTX_new ();        
        sd  = SEAP_connect (ctx, argv[1], 0);
        
        if (sd < 0) {
                ERR("Can't connect to \"%s\": %u, %s.\n", argv[1], errno, strerror (errno));
                goto exit;
        }

        /* Receive S-exps until EOF, error, etc. */
        while (SEAP_recvsexp (ctx, sd, &sexp) != -1) {
                INF("S-exp received: sexp=%p, type=%s\n",
                    sexp, SEXP_strtype (sexp));
                
                INF("----------------\n");
                SEXP_printfa (sexp);
                INF("\n----------------\n");        
        }
        
        /* Print the reason why SEAP_recv returned -1 */
        INF("SEAP_recv: %u, %s.\n", errno, strerror (errno));
        INF("Closing SEAP descriptor\n");
        
        SEAP_close (ctx, sd);
exit:
        /* Free context */
        SEAP_CTX_free (ctx);

        return (0);
}
