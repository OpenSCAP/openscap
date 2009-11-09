#include <stdio.h>
#include "public/oval_agent_api.h"
#include "public/oval_system_characteristics.h"
#include "oval_definitions_impl.h"

int main (void)
{
        struct oval_sysinfo *info;

        info = oval_sysinfo_probe ();
        
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

                if (a == NULL || b == NULL || c == NULL || d == NULL) {
                        oval_sysinfo_free (info);
                        return (1);
                }
                
                return (0);
        }

        return (1);
}
