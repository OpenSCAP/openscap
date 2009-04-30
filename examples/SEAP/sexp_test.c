#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>
/* #define SEAP_TYPES_FILE "mytypes.h" */
#include <seap.h>

#define ERR(...) fprintf(stderr, __VA_ARGS__)
#define INF(...) fprintf(stdout, __VA_ARGS__)

int main (int argc, char *argv[])
{
        int i;
        SEXP_pstate_t *pstate = NULL; /* pstate MUST be initialized to NULL */
        SEXP_t        *sexp;
        ssize_t        pret;
        SEXP_ostate_t *ost = NULL;
        SEXP_psetup_t psetup;
        
        if (argc < 2) {
                printf ("Usage: %s <sexp> [<sexp> ...]\n", basename (argv[0]));
                return (1);
        }
        
        SEXP_psetup_init (&psetup);

        for (i = 1; i < argc; ++i) {
                INF("Parsing S-exp #%u.\n", i);
                /* Try to parse the buffer */
                sexp = SEXP_parse (&psetup, argv[i], strlen (argv[i]), &pstate); 
                
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
                
                if (SEXP_listp (sexp)) {
                        sexp = SEXP_list_first (sexp);
                        INF("Hooray!\n");
                        INF("dprint----\n");
                        pret = SEXP_st_dprintc (STDOUT_FILENO, sexp, &ost);
                        INF("\nprintfa---\n");
                        SEXP_printfa (sexp);
                
                        INF("\npret=%zd, ost=%p\n", pret, ost);
                } else {
                        INF("Not a list\n");
                }
        }
                
        return (0);
}
