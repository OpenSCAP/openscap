pkglibexecdir= openscap
pkglibexec_PROGRAMS=

CFLAGS += @pthread_cflags@ -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL/probes/SEAP
LDFLAGS += @pthread_ldflags@

noinst_LIBRARIES= libprobe.a
libprobe_a_SOURCES= probe-main.c probe-cache.c probe-init.c probe-fini.c

LDADD= $(top_builddir)/src/libopenscap.la libprobe.a

if WANT_PROBES_INDEPENDENT
pkglibexec_PROGRAMS += probe_textfilecontent54 probe_xmlfilecontent

probe_textfilecontent54_SOURCES= independent/textfilecontent54.c
probe_textfilecontent54_CFLAGS= @pcre_cflags@
probe_textfilecontent54_LDFLAGS= @pcre_libs@

probe_xmlfilecontent_SOURCES= independent/xmlfilecontent.c
probe_xmlfilecontent_CFLAGS= @libxml_cflags@
probe_xmlfilecontent_LDFLAGS= @libxml_libs@
endif

if WANT_PROBES_UNIX
pkglibexec_PROGRAMS += probe_runlevel

probe_runlevel_SOURCES= unix/runlevel.c
probe_runlevel_CFLAGS=
probe_runlevel_LDFLAGS=

if WANT_PROBES_REDHAT
pkglibexec_PROGRAMS += probe_rpminfo

probe_rpminfo_SOURCES= unix/linux/redhat/rpminfo.c
probe_rpminfo_CFLAGS= @rpm_cflags@ 
probe_rpminfo_LDFLAGS= @rpm_libs@
endif
endif
