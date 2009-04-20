#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
/* #define SEAP_TYPES_FILE "mytypes.h" */
#include "seap.h"

#define ERR(...) fprintf(stderr, __VA_ARGS__)
#define INF(...) fprintf(stdout, __VA_ARGS__)

int main (int argc, char *argv[])
{
        int i;
        SEAP_CTX_t    *ctx;
        SEXP_pstate_t *pstate = NULL; /* pstate MUST be initialized to NULL */
        SEXP_t        *sexp;
        
        if (argc < 2) {
                printf ("Usage: %s <sexp> [<sexp> ...]\n", basename (argv[0]));
                return (1);
        }
        
        /* Create new SEAP context */
        ctx  = SEAP_CTX_new ();

        if (ctx == NULL) {
                ERR("Can't create SEAP context: errno=%u, %s.\n",
                    errno, strerror (errno));
                return (1);
        }

        for (i = 1; i < argc; ++i) {
                INF("Parsing S-exp #%u.\n", i);
                /* Try to parse the buffer */
                sexp = SEAP_SEXP_parse (ctx, argv[i], strlen (argv[i]), &pstate); 
                
                if (sexp == NULL) {
                        if (i+1 < argc) {
                                INF("Continuing with S-exp buffer #%u.\n", i+1);
                                continue;
                        } else {
                                ERR("Parsing failed:\n");
                                ERR("\tCouldn't parse S-exp #%u and no more S-exps for parsing are available.\n", i);
                                return (1);
                        }
                }
                
                INF("Hooray!\n");
                /* Print some cool messages */
        }
        
        SEAP_CTX_free (ctx);
        
        return (0);
}
