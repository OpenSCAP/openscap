pkglibexecdir= $(libexecdir)/openscap
pkglibexec_PROGRAMS=

CFLAGS += @pthread_cflags@ -I$(top_srcdir)/src -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/public -I$(top_srcdir)/src/OVAL/probes/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public @pcre_cflags@ -DTHREAD_SAFE -DSEAP_THREAD_SAFE @libxml_cflags@
LDFLAGS += @pthread_ldflags@

noinst_LIBRARIES= libprobe.a
libprobe_a_SOURCES= \
	probe-cache.c \
	probe-cache.h \
	probe-entcmp.c \
	probe-entcmp.h \
	probe-fini.c \
	probe-init.c \
	probe-main.c

LDADD= $(top_builddir)/src/libopenscap.la libprobe.a @pcre_libs@

if WANT_PROBES_INDEPENDENT
pkglibexec_PROGRAMS += probe_family
probe_family_SOURCES= independent/family.c
probe_family_CFLAGS=
probe_family_LDFLAGS=

pkglibexec_PROGRAMS += probe_system_info
probe_system_info_SOURCES= independent/system_info.c
probe_system_info_CFLAGS= @libnl_cflags@
probe_system_info_LDFLAGS= @libnl_libs@

pkglibexec_PROGRAMS += probe_textfilecontent54
probe_textfilecontent54_SOURCES= independent/textfilecontent54.c
probe_textfilecontent54_CFLAGS= @pcre_cflags@
probe_textfilecontent54_LDFLAGS= @pcre_libs@

pkglibexec_PROGRAMS += probe_xmlfilecontent
probe_xmlfilecontent_SOURCES= independent/xmlfilecontent.c
probe_xmlfilecontent_CFLAGS= @libxml_cflags@
probe_xmlfilecontent_LDFLAGS= @libxml_libs@
endif

if WANT_PROBES_UNIX
pkglibexec_PROGRAMS += probe_runlevel
probe_runlevel_SOURCES= unix/runlevel.c
probe_runlevel_CFLAGS=
probe_runlevel_LDFLAGS=

pkglibexec_PROGRAMS += probe_file
probe_file_SOURCES= unix/file.c
probe_file_CFLAGS=
probe_file_LDFLAGS=

pkglibexec_PROGRAMS += probe_password
probe_password_SOURCES= unix/password.c
probe_password_CFLAGS=
probe_password_LDFLAGS=

pkglibexec_PROGRAMS += probe_shadow
probe_shadow_SOURCES= unix/shadow.c
probe_shadow_CFLAGS=
probe_shadow_LDFLAGS=

pkglibexec_PROGRAMS += probe_uname
probe_uname_SOURCES= unix/uname.c
probe_uname_CFLAGS=
probe_uname_LDFLAGS=

if WANT_PROBES_LINUX
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
