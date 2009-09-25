#ifndef __STUB_PROBE
#include <stddef.h>
#include <string.h>
#include "generic/common.h"
#include "_seap-scheme.h"

/* KEEP THIS ARRAY SORTED! */
const SEAP_schemefn_t __schtbl[] = {
        { "cons",
          sch_cons_connect, sch_cons_openfd,
          sch_cons_openfd2, sch_cons_recv,
          sch_cons_send, sch_cons_close,
          sch_cons_sendsexp, sch_cons_select },
        { "dummy",
          sch_dummy_connect, sch_dummy_openfd,
          sch_dummy_openfd2, sch_dummy_recv,
          sch_dummy_send, sch_dummy_close,
          sch_dummy_sendsexp, sch_dummy_select },
        { "generic",
          sch_generic_connect, sch_generic_openfd,
          sch_generic_openfd2, sch_generic_recv,
          sch_generic_send, sch_generic_close,
          sch_generic_sendsexp, sch_generic_select },
        { "pipe",
          sch_pipe_connect, sch_pipe_openfd,
          sch_pipe_openfd2, sch_pipe_recv,
          sch_pipe_send, sch_pipe_close,
          sch_pipe_sendsexp, sch_pipe_select }
};

#define SCHTBLSIZE ((sizeof __schtbl)/sizeof (SEAP_schemefn_t))

SEAP_scheme_t SEAP_scheme_search (const SEAP_schemefn_t fntable[SCHTBLSIZE], const char *sch, size_t schlen)
{
        SEAP_scheme_t w, s;
        int cmp;
        
        w = SCHTBLSIZE;
        s = 0;

        while (w > 0) {
                cmp = strncmp (sch, fntable[s + w/2].schstr, schlen);
                
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return (s + w/2);
                }
        }
        
        return (SCH_NONE);
}
#endif
