#include <seap.h>
#include <assert.h>
#include <errno.h>
#include "oval_probe.h"
#include "probes/probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

/* KEEP THIS LIST SORTED! (by subtype) */
const oval_probe_t __probetbl[] = {
        /* 9001 */ { LINUX_DPKG_INFO, "dpkginfo", "probe.dpkginfo" },
        /* 9003 */ { LINUX_RPM_INFO,  "rpminfo",  "probe.rpminfo"  },
        /* 9004 */ { LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo", "probe.slackwarepkginfo" }
};

#define PROBETBLSIZE (sizeof __probetbl / sizeof (oval_probe_t))

const oval_probe_t *search_probe (oval_subtype_enum typenum)
{
        uint32_t w, s;
        
        w = PROBETBLSIZE;
        s = 0;
                
        while (w > 0) {
                if (typenum > __probetbl[s + w/2].typenum) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (typenum < __probetbl[s + w/2].typenum) {
                        w = w/2;
                } else {
                        return &(__probetbl[s + w/2]);
                }
        }
        
#undef cmp

        return (NULL);
}

struct oval_iterator_syschar *sexp_to_oval_state (SEXP_t *sexp)
{
        /* TODO */
        return (NULL);
}

struct oval_iterator_syschar *probe_simple_object (struct oval_object *object,
                                                   struct oval_iterator_variable_binding *binding)
{
        SEXP_t       *sexp;
        const oval_probe_t *probe;
        struct oval_iterator_syschar *sysch;
        
        _A(object != NULL);

        probe = search_probe (oval_object_subtype(object));
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        /* create s-exp */
        sexp  = oval_object_to_sexp (probe->typestr, object);
        
        

        /* translate the result to oval state */
        sysch = sexp_to_oval_state (sexp);
        
        /* cleanup */

        return (NULL);
}
