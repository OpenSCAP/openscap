pkglibexecdir= openscap
pkglibexec_PROGRAMS=

CFLAGS += -I$(top_srcdir)/src/OVAL -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL/probes/SEAP

if WANT_PROBES_REDHAT
pkglibexec_PROGRAMS += probe_rpminfo

probe_rpminfo_SOURCES= unix/linux/redhat/rpminfo.c
probe_rpminfo_CFLAGS= @rpm_cflags@ 
probe_rpminfo_LDFLAGS= @rpm_libs@
probe_rpminfo_LDADD= $(top_builddir)/src/libopenscap.la
endif
