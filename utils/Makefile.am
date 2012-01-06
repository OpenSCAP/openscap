SUBDIRS =
oscap_LDADD =

if WANT_UTIL_OSCAP
bin_PROGRAMS	= oscap
man_MANS	= oscap.8
oscap_SOURCES	= oscap.c oscap-tool.h oscap-tool.c
oscap_CPPFLAGS	= -I$(top_srcdir)/src/common/public @xml2_CFLAGS@
oscap_LDADD	+= $(top_builddir)/src/libopenscap.la
EXTRA_DIST	= $(man_MANS)


if WANT_PROBES
oscap_CPPFLAGS	+= -DENABLE_OVAL -DOVAL_PROBE_DIR='"$(probe_dir)"'
oscap_SOURCES	+= oscap-oval.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/OVAL/public

if WANT_XCCDF
oscap_CPPFLAGS	+= -DENABLE_XCCDF
oscap_SOURCES	+= oscap-xccdf.c
oscap_CPPFLAGS	+=-I$(top_srcdir)/src/XCCDF/public \
		  -I$(top_srcdir)/src/XCCDF_POLICY/public
endif
endif


if WANT_CVSS
oscap_CPPFLAGS	+= -DENABLE_CVSS
oscap_SOURCES	+= oscap-cvss.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/CVSS/public
endif

if WANT_CPE
oscap_CPPFLAGS  += -DENABLE_CPE
oscap_SOURCES   += oscap-cpe.c
oscap_CPPFLAGS  += -I$(top_srcdir)/src/CPE/public
endif

if WANT_SCE
oscap_CPPFLAGS  += -DENABLE_SCE
oscap_CPPFLAGS  += -I$(top_srcdir)/src/SCE/public
endif

endif
