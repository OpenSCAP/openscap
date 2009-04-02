#ifndef OVAL_SYSCHAR_IMPL
#define OVAL_SYSCHAR_IMPL
#include "api/oval_system_characteristics.h"
#include "oval_definitions_impl.h"

struct oval_sysint *oval_sysint_new();
void oval_sysint_free(struct oval_sysint *);

void set_oval_sysint_name(struct oval_sysint *, char *);
void set_oval_sysint_ip_address(struct oval_sysint *, char *);
void set_oval_sysint_mac_address(struct oval_sysint *, char *);

struct oval_sysinfo *oval_sysinfo_new();
void oval_sysinfo_free(struct oval_sysinfo *);

void set_oval_sysinfo_os_name(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_version(struct oval_sysinfo *, char *);
void set_oval_sysinfo_os_architecture(struct oval_sysinfo *, char *);
void set_oval_sysinfo_primary_host_name(struct oval_sysinfo *, char *);
void add_oval_sysinfo_interface(struct oval_sysinfo *, struct oval_sysint *);

struct oval_sysdata *oval_sysdata_new();
void oval_sysdata_free(struct oval_sysdata *);

void set_oval_sysdata_family(struct oval_sysdata *, oval_family_enum);
void set_oval_sysdata_subtype(struct oval_sysdata *, oval_subtype_enum);

struct oval_syschar *oval_syschar_new();
void oval_syschar_free(struct oval_syschar *);

void set_oval_syschar_flag(struct oval_syschar *,
			   oval_syschar_collection_flag_enum);
void add_oval_syschar_messages(struct oval_syschar *, char *);
void set_oval_syschar_sysinfo(struct oval_syschar *, struct oval_sysinfo *);
void set_oval_syschar_object(struct oval_syschar *, struct oval_object *);
void add_oval_syschar_variable_binding(struct oval_syschar *,
				       struct oval_variable_binding *);
void add_oval_syschar_sysdata(struct oval_syschar *, struct oval_sysdata *);

#endif
