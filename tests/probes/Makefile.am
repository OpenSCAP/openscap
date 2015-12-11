INDEPENDENT_SUBDIRS=
UNIX_SUBDIRS=
LINUX_SUBDIRS=
SOLARIS_SUBDIRS=
VALGRIND_SUBDIRS=
ENV_SUBDIRS=

if WANT_PROBES_INDEPENDENT
if probe_family_enabled
INDEPENDENT_SUBDIRS += family
endif
if probe_filehash_enabled
INDEPENDENT_SUBDIRS += filehash
endif
if probe_filehash58_enabled
INDEPENDENT_SUBDIRS += filehash58
endif
if probe_textfilecontent54_enabled
INDEPENDENT_SUBDIRS += textfilecontent54
endif
if probe_system_info_enabled
INDEPENDENT_SUBDIRS += sysinfo
endif
if probe_sql57_enabled
INDEPENDENT_SUBDIRS += sql57
endif
endif

if WANT_PROBES_UNIX
if probe_file_enabled
UNIX_SUBDIRS += file
endif
if probe_uname_enabled
UNIX_SUBDIRS += uname
endif
if probe_shadow_enabled
UNIX_SUBDIRS += shadow
endif
if probe_process58_enabled
UNIX_SUBDIRS += process58
endif
if probe_password_enabled
UNIX_SUBDIRS += password
endif
if probe_runlevel_enabled
UNIX_SUBDIRS += runlevel
endif
if probe_interface_enabled
UNIX_SUBDIRS += interface
endif
if probe_xinetd_enabled
UNIX_SUBDIRS += xinetd
endif
if probe_fileextendedattribute_enabled
UNIX_SUBDIRS += fileextendedattribute
endif
if probe_sysctl_enabled
UNIX_SUBDIRS += sysctl
endif
endif

if WANT_PROBES_LINUX
if probe_rpminfo_enabled
LINUX_SUBDIRS += rpminfo
endif
if probe_iflisteners_enabled
LINUX_SUBDIRS += iflisteners
endif
if probe_selinuxboolean_enabled
LINUX_SUBDIRS += selinuxboolean
endif
endif

if WANT_PROBES_SOLARIS
if probe_isainfo_enabled
SOLARIS_SUBDIRS += isainfo
endif
endif

if ENABLE_VALGRIND_TESTS
VALGRIND_SUBDIRS += vgtest
endif

if ENV_PRESENT
if probe_environmentvariable_enabled
ENV_SUBDIRS += environmentvariable
endif
if probe_environmentvariable58_enabled
ENV_SUBDIRS += environmentvariable58
endif
endif

SUBDIRS = $(INDEPENDENT_SUBDIRS) $(UNIX_SUBDIRS) $(LINUX_SUBDIRS) $(SOLARIS_SUBDIRS) $(VALGRIND_SUBDIRS) $(ENV_SUBDIRS)
SUBDIRS += rpmverify maskattr
