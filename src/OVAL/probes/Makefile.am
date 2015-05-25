SUBDIRS= probe crapi

pkglibexecdir= $(libexecdir)/openscap
pkglibexec_PROGRAMS=

CFLAGS +=	@xml2_CFLAGS@ @pthread_CFLAGS@ @pcre_CFLAGS@	\
		-DTHREAD_SAFE -DSEAP_THREAD_SAFE		\
		-I$(top_srcdir)/src				\
		-I$(top_srcdir)/src/common 			\
		-I$(top_srcdir)/src/common/public		\
		-I$(top_srcdir)/src/source/public		\
		-I$(top_srcdir)/src/OVAL			\
		-I$(top_srcdir)/src/OVAL/public			\
		-I$(top_srcdir)/src/OVAL/probes/public		\
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public	\
		-I$(top_srcdir)/lib

LDADD= probe/libprobe.la @sigwaitinfo_LIBS@

pkglibexec_PROGRAMS += probe_system_info
probe_system_info_SOURCES= independent/system_info.c

if WANT_PROBES_INDEPENDENT

if probe_family_enabled
pkglibexec_PROGRAMS += probe_family
probe_family_SOURCES= independent/family.c
endif

if probe_textfilecontent_enabled
pkglibexec_PROGRAMS += probe_textfilecontent
probe_textfilecontent_SOURCES= independent/textfilecontent.c
probe_textfilecontent_CFLAGS= @pcre_CFLAGS@
probe_textfilecontent_LDFLAGS= @pcre_LIBS@
endif

if probe_textfilecontent54_enabled
pkglibexec_PROGRAMS += probe_textfilecontent54
probe_textfilecontent54_SOURCES= independent/textfilecontent54.c
probe_textfilecontent54_CFLAGS= @pcre_CFLAGS@
probe_textfilecontent54_LDFLAGS= @pcre_LIBS@
endif

if probe_variable_enabled
pkglibexec_PROGRAMS += probe_variable
probe_variable_SOURCES= independent/variable.c
endif

if probe_xmlfilecontent_enabled
pkglibexec_PROGRAMS += probe_xmlfilecontent
probe_xmlfilecontent_SOURCES= independent/xmlfilecontent.c
probe_xmlfilecontent_CFLAGS= @xml2_CFLAGS@
probe_xmlfilecontent_LDFLAGS= @xml2_LIBS@
endif

if probe_filehash_enabled
pkglibexec_PROGRAMS += probe_filehash
probe_filehash_SOURCES= independent/filehash.c
probe_filehash_LDFLAGS= crapi/libcrapi.la
endif

if probe_filehash58_enabled
pkglibexec_PROGRAMS += probe_filehash58
probe_filehash58_SOURCES= independent/filehash58.c
probe_filehash58_LDFLAGS= crapi/libcrapi.la ../../common/liboscapcommon.la
endif

if probe_environmentvariable_enabled
pkglibexec_PROGRAMS += probe_environmentvariable
probe_environmentvariable_SOURCES= independent/environmentvariable.c
endif

if probe_environmentvariable58_enabled
pkglibexec_PROGRAMS += probe_environmentvariable58
probe_environmentvariable58_SOURCES= independent/environmentvariable58.c
endif

if probe_sql_enabled
pkglibexec_PROGRAMS += probe_sql
probe_sql_SOURCES= independent/sql.c
probe_sql_CFLAGS= @opendbx_CFLAGS@
probe_sql_LDFLAGS= @opendbx_LIBS@
endif

if probe_sql57_enabled
pkglibexec_PROGRAMS += probe_sql57
probe_sql57_SOURCES= independent/sql57.c
probe_sql57_CFLAGS= @opendbx_CFLAGS@
probe_sql57_LDFLAGS= @opendbx_LIBS@
endif

if probe_ldap57_enabled
pkglibexec_PROGRAMS += probe_ldap57
probe_ldap57_SOURCES= independent/ldap57.c
probe_ldap57_CFLAGS= @ldap_CFLAGS@ @lber_CFLAGS@
probe_ldap57_LDFLAGS= @ldap_LIBS@ @lber_LIBS@
endif
endif

if WANT_PROBES_UNIX

if probe_dnscache_enabled
pkglibexec_PROGRAMS += probe_dnscache
probe_dnscache_SOURCES= unix/dnscache.c
endif

if probe_runlevel_enabled
pkglibexec_PROGRAMS += probe_runlevel
probe_runlevel_SOURCES= unix/runlevel.c
endif

if probe_file_enabled
pkglibexec_PROGRAMS += probe_file
probe_file_SOURCES= unix/file.c
probe_file_CFLAGS= @acl_CFLAGS@
probe_file_LDFLAGS= @acl_LIBS@
endif

if probe_fileextendedattribute_enabled
pkglibexec_PROGRAMS += probe_fileextendedattribute
probe_fileextendedattribute_SOURCES= unix/fileextendedattribute.c
endif

if probe_password_enabled
pkglibexec_PROGRAMS += probe_password
probe_password_SOURCES= unix/password.c
endif

if probe_process_enabled
pkglibexec_PROGRAMS += probe_process
probe_process_SOURCES= unix/process.c unix/process58-devname.c unix/process58-devname.h
probe_process_CFLAGS= @procps_CFLAGS@
probe_process_LDFLAGS= @procps_LIBS@
endif

if probe_process58_enabled
pkglibexec_PROGRAMS += probe_process58
probe_process58_SOURCES= unix/process58.c unix/process58-capability.h unix/process58-devname.c unix/process58-devname.h
probe_process58_CFLAGS= @selinux_CFLAGS@ @cap_CFLAGS@ @procps_CFLAGS@
probe_process58_LDFLAGS= @selinux_LIBS@ @cap_LIBS@ @procps_LIBS@ ../../common/liboscapcommon.la
endif

if probe_shadow_enabled
pkglibexec_PROGRAMS += probe_shadow
probe_shadow_SOURCES= unix/shadow.c
endif

if probe_uname_enabled
pkglibexec_PROGRAMS += probe_uname
probe_uname_SOURCES= unix/uname.c
endif

if probe_interface_enabled
pkglibexec_PROGRAMS += probe_interface
probe_interface_SOURCES= unix/interface.c
endif

if probe_xinetd_enabled
pkglibexec_PROGRAMS += probe_xinetd
probe_xinetd_SOURCES = unix/xinetd.c
endif

if probe_sysctl_enabled
pkglibexec_PROGRAMS += probe_sysctl
probe_sysctl_SOURCES= unix/sysctl.c
endif

if probe_routingtable_enabled
pkglibexec_PROGRAMS += probe_routingtable
probe_routingtable_SOURCES= unix/routingtable.c
endif

if probe_symlink_enabled
pkglibexec_PROGRAMS += probe_symlink
probe_symlink_SOURCES = unix/symlink.c
endif

if probe_gconf_enabled
pkglibexec_PROGRAMS += probe_gconf
probe_gconf_SOURCES= unix/gconf.c
probe_gconf_CFLAGS= @gconf2_CFLAGS@
probe_gconf_LDFLAGS= @gconf2_LIBS@
endif

#
# Solaris
#
if WANT_PROBES_SOLARIS

if probe_isainfo_enabled
pkglibexec_PROGRAMS += probe_isainfo
probe_isainfo_SOURCES= unix/solaris/isainfo.c
endif

if probe_package_enabled
pkglibexec_PROGRAMS += probe_package
probe_package_SOURCES= unix/solaris/package.c
endif

if probe_patch_enabled
pkglibexec_PROGRAMS += probe_patch
probe_patch_SOURCES= unix/solaris/patch.c
endif

if probe_smf_enabled
pkglibexec_PROGRAMS += probe_smf
probe_smf_SOURCES= unix/solaris/smf.c
endif

endif

#
# Linux
#
if WANT_PROBES_LINUX
if probe_partition_enabled
pkglibexec_PROGRAMS += probe_partition
probe_partition_SOURCES= unix/linux/partition.c
probe_partition_CFLAGS= -DPROC_CHECK @pcre_CFLAGS@ @blkid_CFLAGS@
probe_partition_LDFLAGS= @pcre_LIBS@ @blkid_LIBS@
endif

if probe_inetlisteningservers_enabled
pkglibexec_PROGRAMS += probe_inetlisteningservers
probe_inetlisteningservers_SOURCES= unix/linux/inetlisteningservers.c
endif

if probe_iflisteners_enabled
pkglibexec_PROGRAMS += probe_iflisteners
probe_iflisteners_SOURCES= unix/linux/iflisteners.c unix/linux/iflisteners-proto.h
probe_iflisteners_LDFLAGS= ../../common/liboscapcommon.la
endif

if probe_selinuxboolean_enabled
pkglibexec_PROGRAMS += probe_selinuxboolean
probe_selinuxboolean_SOURCES= unix/linux/selinuxboolean.c
probe_selinuxboolean_CFLAGS= @selinux_CFLAGS@
probe_selinuxboolean_LDFLAGS= @selinux_LIBS@
endif

if probe_selinuxsecuritycontext_enabled
pkglibexec_PROGRAMS += probe_selinuxsecuritycontext
probe_selinuxsecuritycontext_SOURCES= unix/linux/selinuxsecuritycontext.c
probe_selinuxsecuritycontext_CFLAGS= @selinux_CFLAGS@
probe_selinuxsecuritycontext_LDFLAGS= @selinux_LIBS@
endif

if probe_rpminfo_enabled
pkglibexec_PROGRAMS += probe_rpminfo
probe_rpminfo_SOURCES= unix/linux/rpminfo.c
probe_rpminfo_CFLAGS= @rpm_CFLAGS@
probe_rpminfo_LDFLAGS= @rpm_LIBS@
endif

if probe_rpmverify_enabled
pkglibexec_PROGRAMS += probe_rpmverify
probe_rpmverify_SOURCES= unix/linux/rpmverify.c
probe_rpmverify_CFLAGS= @rpm_CFLAGS@
probe_rpmverify_LDFLAGS= @rpm_LIBS@
endif

if probe_rpmverifyfile_enabled
pkglibexec_PROGRAMS += probe_rpmverifyfile
probe_rpmverifyfile_SOURCES= unix/linux/rpmverifyfile.c
probe_rpmverifyfile_CFLAGS= @rpm_CFLAGS@
probe_rpmverifyfile_LDFLAGS= @rpm_LIBS@
endif

if probe_rpmverifypackage_enabled
pkglibexec_PROGRAMS += probe_rpmverifypackage
probe_rpmverifypackage_SOURCES= unix/linux/rpmverifypackage.c
probe_rpmverifypackage_CFLAGS= @rpm_CFLAGS@
probe_rpmverifypackage_LDFLAGS= @rpm_LIBS@ -lpopt
endif

if probe_dpkginfo_enabled
pkglibexec_PROGRAMS += probe_dpkginfo
probe_dpkginfo_SOURCES= unix/linux/dpkginfo.c \
       unix/linux/dpkginfo-helper.cxx \
       unix/linux/dpkginfo-helper.h
probe_dpkginfo_CFLAGS= @apt_pkg_CFLAGS@
probe_dpkginfo_CXXFLAGS= @apt_pkg_CFLAGS@
probe_dpkginfo_LDFLAGS= @apt_pkg_LIBS@
endif

if probe_systemdunitproperty_enabled
pkglibexec_PROGRAMS += probe_systemdunitproperty
probe_systemdunitproperty_SOURCES= unix/linux/systemdunitproperty.c \
       unix/linux/systemdshared.h
probe_systemdunitproperty_CFLAGS= @dbus1_CFLAGS@
probe_systemdunitproperty_CXXFLAGS = @dbus1_CFLAGS@
probe_systemdunitproperty_LDFLAGS= @dbus1_LIBS@
endif

if probe_systemdunitdependency_enabled
pkglibexec_PROGRAMS += probe_systemdunitdependency
probe_systemdunitdependency_SOURCES= unix/linux/systemdunitdependency.c \
       unix/linux/systemdshared.h
probe_systemdunitdependency_CFLAGS= @dbus1_CFLAGS@
probe_systemdunitdependency_CXXFLAGS = @dbus1_CFLAGS@
probe_systemdunitdependency_LDFLAGS= @dbus1_LIBS@
endif

endif
endif
