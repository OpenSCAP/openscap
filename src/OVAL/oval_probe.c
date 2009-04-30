#include "oval_definitions_impl.h"
#include "oval_agent_api_impl.h"
#include "oval_parser_impl.h"
#include "oval_string_map_impl.h"
#include <seap.h>
#include <assert.h>
#include <errno.h>
#include "oval_probe.h"

#ifndef _A
#define _A(x) assert(x)
#endif

const oval_probe_t __probetbl[] = {
        /* 9001 */ { LINUX_DPGK_INFO, "dpkginfo", "probe.dpkginfo" },
        /* 9003 */ { LINUX_RPM_INFO,  "rpminfo",  "probe.rpminfo"  },
        /* 9004 */ { LINUX_SLACKWARE_PKG_INFO_TEST, "slackwarepkginfo", "probe.slackwarepkginfo" }
};

#define PROBETBLSIZE (sizeof __probetbl / sizeof (oval_probe_t))

const oval_probe_t *search_probe (oval_subtype_enum typenum)
{
        uint32_t w, s;
        
        w = PROBETBLSIZE;
        s = 0;
        
#define cmp (typenum - __probetbl[s + w/2].typenum)
        
        while (w > 0) {
                if (cmp > 0) {
                        s += w/2 + 1;
                        w  = w - w/2 - 1;
                } else if (cmp < 0) {
                        w = w/2;
                } else {
                        return &(__probetbl[s + w/2]);
                }
        }
        
#undef cmp

        return (NULL);
}

struct oval_iterator_syschar *probe_simple_object (struct oval_object *object,
                                                   struct oval_iterator_variable_binding *binding)
{
        SEXP_t       *sexp;
        oval_probe_t *probe;
        
        _A(object != NULL);

        probe = search_probe (object->subtype);
        if (probe == NULL) {
                errno = EOPNOTSUPP;
                return (NULL);
        }
        
        /* create s-exp */
        sexp = oval_object_to_sexp (probe->typestr, object);
        
        /* translate the result to oval state */

        return (NULL);
}
