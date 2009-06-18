pkglibexecdir= openscap
pkglibexec_PROGRAMS=

CFLAGS += -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL/probes/SEAP

if WANT_PROBES_INDEPENDENT
pkglibexec_PROGRAMS += probe_textfilecontent54

probe_textfilecontent54_SOURCES= probe-main.c independent/textfilecontent54.c
probe_textfilecontent54_CFLAGS= @pcre_cflags@
probe_textfilecontent54_LDFLAGS= @pcre_libs@
probe_textfilecontent54_LDADD= $(top_builddir)/src/libopenscap.la
endif

if WANT_PROBES_UNIX
pkglibexec_PROGRAMS += probe_runlevel

probe_runlevel_SOURCES= probe-main.c unix/runlevel.c
probe_runlevel_CFLAGS=
probe_runlevel_LDFLAGS=
probe_runlevel_LDADD= $(top_builddir)/src/libopenscap.la

if WANT_PROBES_REDHAT
pkglibexec_PROGRAMS += probe_rpminfo

probe_rpminfo_SOURCES= probe-main.c unix/linux/redhat/rpminfo.c
probe_rpminfo_CFLAGS= @rpm_cflags@ 
probe_rpminfo_LDFLAGS= @rpm_libs@
probe_rpminfo_LDADD= $(top_builddir)/src/libopenscap.la
endif
endif