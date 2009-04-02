#ifndef OVAL_SYSTEM_CHARACTERISTICS
#define OVAL_SYSTEM_CHARACTERISTICS
#include "oval_definitions.h"

typedef enum {
	SYSCHAR_FLAG_ERROR,
	SYSCHAR_FLAG_COMPLETE,
	SYSCHAR_FLAG_INCOMPLETE,
	SYSCHAR_FLAG_DOES_NOT_EXIST,
	SYSCHAR_FLAG_NOT_COLLECTED,
	SYSCHAR_FLAG_NOT_APPLICABLE
} oval_syschar_collection_flag_enum;

struct oval_sysint;
struct oval_iterator_sysint;

struct oval_sysinfo;
struct oval_iterator_sysinfo;

struct oval_sysdata;
struct oval_iterator_sysdata;

struct oval_syschar;
struct oval_iterator_syschar;

int oval_iterator_sysint_has_more(struct oval_iterator_sysint *);
struct oval_sysint *oval_iterator_sysint_next(struct oval_iterator_sysint *);

char *oval_sysint_name(struct oval_sysint *);
char *oval_sysint_ip_address(struct oval_sysint *);
char *oval_sysint_mac_address(struct oval_sysint *);

int oval_iterator_sysinfo_has_more(struct oval_iterator_sysinfo *);
struct oval_sysinfo *oval_iterator_sysinfo_next(struct oval_iterator_sysinfo *);

char *oval_sysinfo_os_name(struct oval_sysinfo *);
char *oval_sysinfo_os_version(struct oval_sysinfo *);
char *oval_sysinfo_os_architecture(struct oval_sysinfo *);
char *oval_sysinfo_primary_host_name(struct oval_sysinfo *);
struct oval_iterator_sysint *oval_sysinfo_interfaces(struct oval_sysinfo *);

int oval_iterator_sysdata_has_more(struct oval_iterator_sysdata *);
struct oval_sysdata *oval_iterator_sysdata_next(struct oval_iterator_sysdata *);

oval_family_enum oval_sysdata_family(struct oval_sysdata *);
oval_subtype_enum oval_sysdata_subtype(struct oval_sysdata *);

int oval_iterator_syschar_has_more(struct oval_iterator_syschar *);
struct oval_syschar *oval_iterator_syschar_next(struct oval_iterator_syschar *);

oval_syschar_collection_flag_enum oval_syschar_flag(struct oval_syschar *);
struct oval_iterator_string *oval_syschar_messages(struct oval_syschar *);
struct oval_sysinfo *oval_syschar_sysinfo(struct oval_syschar *);
struct oval_object *oval_syschar_object(struct oval_syschar *);
struct oval_iterator_variable_binding *oval_syschar_variable_bindings(struct
								      oval_syschar
								      *);
struct oval_iterator_sysdata *oval_syschar_sysdata(struct oval_syschar *);

#endif
