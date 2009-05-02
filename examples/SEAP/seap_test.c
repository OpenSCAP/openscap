#include <stdio.h>
#include <seap.h>
#include <errno.h>

#define ERR(...) fprintf (stderr, __VA_ARGS__)
#define INF(...) fprintf (stdout, __VA_ARGS__)

int main (int argc, char *argv[])
{
        SEAP_CTX_t    *ctx;
        SEXP_t        *sexp;
        SEXP_pstate_t *sexp_pstate;
        int sd;

        /* Create context */
        ctx = SEAP_CTX_new ();
        
        /* Open in/out streams; in = stdin, out = stdout */
        sd = SEAP_openfd2 (ctx, fileno (stdin), fileno (stdout), 0);
        
        if (sd < 0) {
                ERR("Can't open sd: %u, %s.\n", errno, strerror (errno));
                goto exit;
        }

        /* Receive S-exps until EOF, error, etc. */
        while (SEAP_recvsexp (ctx, sd, &sexp) != -1) {
                INF("S-exp received: sexp=%p, type=%u (%s)\n",
                    sexp, SEXP_TYPE(sexp), SEXP_strtype (sexp));
                
                INF("----------------\n");
                SEXP_printfa (sexp);
                INF("\n----------------\n");        
        }
        
        /* Print the reason why SEAP_recv returned -1 */
        INF("SEAP_recv: %u, %s.\n", errno, strerror (errno));
exit:
        /* Free context */
        SEAP_CTX_free (ctx);

        return (0);
}
