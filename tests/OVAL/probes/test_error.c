#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "public/oval_agent_api.h"
#include "oval_definitions_impl.h"
#include "oval_system_characteristics_impl.h"
#include "public/error.h"

int main (void)
{
        oval_pctx_t *pctx;

        pctx = oval_pctx_new (NULL);
        
	if (pctx == NULL) {
		printf("oval_pctx_new() failed.\n");
		return 5;
	}

        (void) oval_probe_object_eval(pctx, (void *)1);
        oval_pctx_setattr (pctx, OVAL_PCTX_ATTR_MODEL, NULL);
        
        if (oscap_err ()) {
                oscap_errfamily_t f;
                oscap_errcode_t   c;
                const char       *d;

                printf ("error: %d, %d, %s.\n",
                        f = oscap_err_family (),
                        c = oscap_err_code (),
                        d = oscap_err_desc ());
                
                if (f != 123 || c != 123 || d == NULL)
                        return (6);
        } else {
                return (4);
        }

        return (0);
}
