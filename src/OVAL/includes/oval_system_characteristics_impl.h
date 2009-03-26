#ifndef OVAL_SYSCHAR_IMPL
#define OVAL_SYSCHAR_IMPL
#include <liboval/oval_system_characteristics.h>
#include "oval_definitions_impl.h"

	struct oval_sysint_s *oval_sysint_new();
	void oval_sysint_free(struct oval_sysint_s*);

	void set_oval_sysint_name       (struct oval_sysint_s*, char*);
	void set_oval_sysint_ip_address (struct oval_sysint_s*, char*);
	void set_oval_sysint_mac_address(struct oval_sysint_s*, char*);

	struct oval_sysinfo_s *oval_sysinfo_new();
	void oval_sysinfo_free(struct oval_sysinfo_s*);

	void set_oval_sysinfo_os_name           (struct oval_sysinfo_s*, char*);
	void set_oval_sysinfo_os_version        (struct oval_sysinfo_s*, char*);
	void set_oval_sysinfo_os_architecture   (struct oval_sysinfo_s*, char*);
	void set_oval_sysinfo_primary_host_name (struct oval_sysinfo_s*, char*);
	void add_oval_sysinfo_interface         (struct oval_sysinfo_s*, struct oval_sysint_s*);

	struct oval_sysdata_s *oval_sysdata_new();
	void oval_sysdata_free(struct oval_sysdata_s*);

	void set_oval_sysdata_family  (struct oval_sysdata_s*, oval_family_enum);
	void set_oval_sysdata_subtype(struct oval_sysdata_s*, oval_subtype_enum);

	struct oval_syschar_s *oval_syschar_new();
	void oval_syschar_free(struct oval_syschar_s*);

	void set_oval_syschar_flag            (struct oval_syschar_s*, oval_syschar_collection_flag_enum);
	void add_oval_syschar_messages        (struct oval_syschar_s*, char*);
	void set_oval_syschar_sysinfo         (struct oval_syschar_s*, struct oval_sysinfo_s*);
	void set_oval_syschar_object          (struct oval_syschar_s*, struct oval_object_s*);
	void add_oval_syschar_variable_binding(struct oval_syschar_s*, struct oval_variable_binding_s*);
	void add_oval_syschar_sysdata         (struct oval_syschar_s*, struct oval_sysdata_s*);

#endif
