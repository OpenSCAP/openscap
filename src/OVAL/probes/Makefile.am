SUBDIRS= probe crapi

pkglibexecdir= $(libexecdir)/openscap
pkglibexec_PROGRAMS=

CFLAGS += @pthread_cflags@ -I$(top_srcdir)/src -I$(top_srcdir)/src/common -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/public -I$(top_srcdir)/src/OVAL/probes/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public @pcre_cflags@ -DTHREAD_SAFE -DSEAP_THREAD_SAFE @libxml_cflags@
LDFLAGS += @pthread_ldflags@

LDADD= probe/libprobe.la @pcre_libs@ @sigwaitinfo_libs@

if WANT_PROBES
pkglibexec_PROGRAMS += probe_system_info
probe_system_info_SOURCES= independent/system_info.c
endif

if WANT_PROBES_INDEPENDENT
pkglibexec_PROGRAMS += probe_family
probe_family_SOURCES= independent/family.c
probe_family_CFLAGS=
probe_family_LDFLAGS=


pkglibexec_PROGRAMS += probe_textfilecontent
probe_textfilecontent_SOURCES= independent/textfilecontent.c
probe_textfilecontent_CFLAGS= @pcre_cflags@
probe_textfilecontent_LDFLAGS= @pcre_libs@

pkglibexec_PROGRAMS += probe_textfilecontent54
probe_textfilecontent54_SOURCES= independent/textfilecontent54.c
probe_textfilecontent54_CFLAGS= @pcre_cflags@
probe_textfilecontent54_LDFLAGS= @pcre_libs@

pkglibexec_PROGRAMS += probe_xmlfilecontent
probe_xmlfilecontent_SOURCES= independent/xmlfilecontent.c
probe_xmlfilecontent_CFLAGS= @libxml_cflags@
probe_xmlfilecontent_LDFLAGS= @libxml_libs@

pkglibexec_PROGRAMS += probe_filehash
probe_filehash_SOURCES= independent/filehash.c
probe_filehash_CFLAGS=
probe_filehash_LDFLAGS= crapi/libcrapi.la

if HAVE_ODBX
pkglibexec_PROGRAMS += probe_sql
probe_sql_SOURCES= independent/sql.c
probe_sql_CFLAGS= @libodbx_cflags@
probe_sql_LDFLAGS= @libodbx_libs@

pkglibexec_PROGRAMS += probe_sql57
probe_sql57_SOURCES= independent/sql57.c
probe_sql57_CFLAGS= @libodbx_cflags@
probe_sql57_LDFLAGS= @libodbx_libs@
endif

if ENABLE_PROBE_LDAP57
pkglibexec_PROGRAMS += probe_ldap57
probe_ldap57_SOURCES= independent/ldap57.c
probe_ldap57_CFLAGS= @libopenldap_cflags@
probe_ldap57_LDFLAGS= @libopenldap_libs@
endif
endif

if WANT_PROBES_UNIX
pkglibexec_PROGRAMS += probe_dnscache
probe_dnscache_SOURCES= unix/dnscache.c
probe_dnscache_CFLAGS=
probe_dnscache_LDFLAGS=

pkglibexec_PROGRAMS += probe_runlevel
probe_runlevel_SOURCES= unix/runlevel.c
probe_runlevel_CFLAGS=
probe_runlevel_LDFLAGS=

pkglibexec_PROGRAMS += probe_file
probe_file_SOURCES= unix/file.c
probe_file_CFLAGS= @libacl_cflags@
probe_file_LDFLAGS= @libacl_libs@

pkglibexec_PROGRAMS += probe_password
probe_password_SOURCES= unix/password.c
probe_password_CFLAGS=
probe_password_LDFLAGS=

pkglibexec_PROGRAMS += probe_process
probe_process_SOURCES= unix/process.c
probe_process_CFLAGS=
probe_process_LDFLAGS=

pkglibexec_PROGRAMS += probe_shadow
probe_shadow_SOURCES= unix/shadow.c
probe_shadow_CFLAGS=
probe_shadow_LDFLAGS=

pkglibexec_PROGRAMS += probe_uname
probe_uname_SOURCES= unix/uname.c
probe_uname_CFLAGS=
probe_uname_LDFLAGS=

pkglibexec_PROGRAMS += probe_interface
probe_interface_SOURCES= unix/interface.c

pkglibexec_PROGRAMS += probe_xinetd
probe_xinetd_SOURCES = unix/xinetd.c
probe_xinetd_LDADD = $(LDADD) $(top_builddir)/src/common/liboscapcommon.la

pkglibexec_PROGRAMS += probe_sysctl
probe_sysctl_SOURCES= unix/sysctl.c

if WANT_PROBES_LINUX
pkglibexec_PROGRAMS += probe_partition
probe_partition_SOURCES= unix/linux/partition.c
probe_partition_CFLAGS= -DPROC_CHECK @pcre_cflags@ @libblkid_cflags@
probe_partition_LDFLAGS= @pcre_libs@ @libblkid_libs@

pkglibexec_PROGRAMS += probe_inetlisteningservers
probe_inetlisteningservers_SOURCES= unix/linux/inetlisteningservers.c
probe_inetlisteningservers_CFLAGS=
probe_inetlisteningservers_LDFLAGS=

if ENABLE_PROBE_RPMINFO
pkglibexec_PROGRAMS += probe_rpminfo
probe_rpminfo_SOURCES= unix/linux/rpminfo.c
probe_rpminfo_CFLAGS= @rpm_cflags@
probe_rpminfo_LDFLAGS= @rpm_libs@
endif

if ENABLE_PROBE_DPKGINFO
pkglibexec_PROGRAMS += probe_dpkginfo
probe_dpkginfo_SOURCES= unix/linux/dpkginfo.c \
       unix/linux/dpkginfo-helper.cxx \
       unix/linux/dpkginfo-helper.h
probe_dpkginfo_CFLAGS= @deb_cflags@
probe_dpkginfo_CXXFLAGS= @deb_cflags@
probe_dpkginfo_LDFLAGS= @deb_libs@
endif
endif
endif
