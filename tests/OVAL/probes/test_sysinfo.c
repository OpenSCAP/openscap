#include <stdio.h>
#include <assume.h>
#include "oval_agent_api.h"
#include "oval_system_characteristics.h"
#include "oval_definitions_impl.h"

int main (void)
{
  
  /* 
   *  Create empty models
   */
  struct oval_definition_model *def_model = oval_definition_model_new();
  assume(def_model != NULL);
  
  struct oval_syschar_model *sys_model = oval_syschar_model_new(def_model);
  assume(sys_model != NULL);

  /*
   *  Create probe context.
   */
  oval_pctx_t *pctx = oval_pctx_new (sys_model);
  assume(pctx != NULL);

  /*
   *  Call the sysinfo probe.
   */
  struct oval_sysinfo *info = oval_probe_sysinf_eval (pctx);
  assume(info != NULL);
  
  if (info != NULL) {
    char *a, *b, *c, *d;

    printf ("          os_name: %s\n"
	    "       os_version: %s\n"
	    "  os_architecture: %s\n"
	    "primary_host_name: %s\n",
            
	    a = oval_sysinfo_get_os_name (info),
	    b = oval_sysinfo_get_os_version (info),
	    c = oval_sysinfo_get_os_architecture (info),
	    d = oval_sysinfo_get_primary_host_name (info));
    
    assume(a != NULL);
    assume(b != NULL);
    assume(c != NULL);
    assume(d != NULL);
        
    struct oval_sysint_iterator *ifit = oval_sysinfo_get_interfaces (info);
    
    if (ifit != NULL) {
      printf ("Interfaces:\n");
      
      while (oval_sysint_iterator_has_more (ifit)) {
	struct oval_sysint *ife = oval_sysint_iterator_next (ifit);
        assume(ife != NULL);

	printf ("%s %s %s\n",
		oval_sysint_get_name (ife),
		oval_sysint_get_ip_address (ife),
		oval_sysint_get_mac_address (ife));
	
	/* oval_sysint_free (ife); */
      }
      
      oval_sysint_iterator_free (ifit);
    }
    
    oval_sysinfo_free (info);
    oval_pctx_free (pctx);
    
    return (0);
  }
  
  /*
   *  Free the probe context. This also terminates
   *  all running probes executed under this context.
   */
  oval_pctx_free (pctx);
  
  return (1);
}
