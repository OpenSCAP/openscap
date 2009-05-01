pkglibexecdir= openscap
pkglibexec_PROGRAMS=

if WANT_PROBES_REDHAT
pkglibexec_PROGRAMS += probe_rpminfo

probe_rpminfo_SOURCES= unix/linux/redhat/rpminfo.c
probe_rpminfo_CFLAGS= -I$(top_srcdir)/src/OVAL/probes -I$(top_srcdir)/src/OVAL/probes/SEAP
probe_rpminfo_LDFLAGS= -L$(top_builddir)/src/ -lopenscap
endif
