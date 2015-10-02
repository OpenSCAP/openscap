SUBDIRS =
oscap_LDADD =

bin_PROGRAMS =
bin_SCRIPTS =
man_MANS =

if WANT_UTIL_OSCAP
bin_PROGRAMS += oscap
man_MANS += oscap.8
oscap_SOURCES	= oscap.c oscap-tool.h oscap-tool.c oscap-ds.c
oscap_CPPFLAGS	= \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/DS/public \
	-I$(top_srcdir)/src/source/public

oscap_LDADD	+= $(top_builddir)/src/libopenscap.la

oscap_CPPFLAGS	+= -DOVAL_PROBE_DIR='"$(probe_dir)"'
oscap_SOURCES	+= oscap-oval.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/OVAL/public

oscap_SOURCES	+= oscap-xccdf.c
oscap_CPPFLAGS	+=-I$(top_srcdir)/src/XCCDF/public \
		  -I$(top_srcdir)/src/XCCDF_POLICY/public

oscap_SOURCES	+= oscap-cvss.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/CVSS/public

oscap_SOURCES   += oscap-cpe.c
oscap_CPPFLAGS  += -I$(top_srcdir)/src/CPE/public

oscap_SOURCES   += oscap-cve.c
oscap_CPPFLAGS  += -I$(top_srcdir)/src/CVE/public

oscap_SOURCES   += oscap-info.c
endif

if WANT_UTIL_SCAP_AS_RPM
bin_SCRIPTS += scap-as-rpm
man_MANS += scap-as-rpm.8
endif
if WANT_UTIL_OSCAP_DOCKER
bin_SCRIPTS += oscap-docker
man_MANS += oscap-docker.8
oscap_docker_PYTHON = \
	oscap_docker_python/__init__.py \
	oscap_docker_python/get_cve_input.py \
	oscap_docker_python/oscap_docker_util.py
oscap_dockerdir = $(oscapdocker_pythondir)/oscap_docker_python
endif
if WANT_UTIL_OSCAP_SSH
bin_SCRIPTS += oscap-ssh
man_MANS += oscap-ssh.8
endif

CONFIG_CLEAN_FILES = oscap-docker


EXTRA_DIST = $(filter-out oscap-docker, $(bin_SCRIPTS)) $(man_MANS)
