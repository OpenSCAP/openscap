
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "oval_agent_api.h"
#include "oval_system_characteristics.h"
#include "oval_definitions_impl.h"
#include "oval_probe.h"
#include "oscap_assert.h"

int main (void)
{
  struct oval_sysinfo *info;

  /*
   *  Create empty models
   */
  struct oval_definition_model *def_model = oval_definition_model_new();
	oscap_assert(def_model != NULL);

  struct oval_syschar_model *sys_model = oval_syschar_model_new(def_model);
	oscap_assert(sys_model != NULL);

  /*
   *  Create probe session
   */
  oval_probe_session_t *sess = oval_probe_session_new(sys_model);
	oscap_assert(sess != NULL);

  /*
   *  Call the sysinfo probe.
   */
	oscap_assert(oval_probe_query_sysinfo(sess, &info) == 0);
	oscap_assert(info != NULL);

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

		oscap_assert(a != NULL);
		oscap_assert(b != NULL);
		oscap_assert(c != NULL);
		oscap_assert(d != NULL);

    struct oval_sysint_iterator *ifit = oval_sysinfo_get_interfaces (info);

    if (ifit != NULL) {
      printf ("Interfaces:\n");

      while (oval_sysint_iterator_has_more (ifit)) {
	struct oval_sysint *ife = oval_sysint_iterator_next (ifit);
				oscap_assert(ife != NULL);

	printf ("%s %s %s\n",
		oval_sysint_get_name (ife),
		oval_sysint_get_ip_address (ife),
		oval_sysint_get_mac_address (ife));

	/* oval_sysint_free (ife); */
      }

      oval_sysint_iterator_free (ifit);
    }

    oval_sysinfo_free (info);
    oval_probe_session_destroy(sess);

    return (0);
  }

  /*
   *  Free the probe context. This also terminates
   *  all running probes executed under this context.
   */
  oval_probe_session_destroy(sess);

  return (1);
}
