AM_CPPFLAGS =   -I$(top_srcdir)/tests/include \
		-I$(top_srcdir)/src/CVE/public \
		-I${top_srcdir}/src/CVSS/public \
		-I$(top_srcdir)/src/CPE/public \
		-I$(top_srcdir)/src/CCE/public \
		-I$(top_srcdir)/src/OVAL/public \
		-I$(top_srcdir)/src/XCCDF/public \
	 	-I$(top_srcdir)/src/common/public \
		-I$(top_srcdir)/src/OVAL/probes/public \
		-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
		-I$(top_srcdir)/src/DS/public \
		-I$(top_srcdir)/src \
		@xml2_CFLAGS@

LDADD = $(top_builddir)/src/libopenscap_testing.la @pcre_LIBS@

DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS = all.sh
check_PROGRAMS = test_ds_sds_index test_ds_sds_index_multiple test_ds_sds_index_invalid

test_ds_sds_index_SOURCES = test_ds_sds_index.c
test_ds_sds_index_multiple_SOURCES = test_ds_sds_index_multiple.c
test_ds_sds_index_invalid_SOURCES = test_ds_sds_index_invalid.c

EXTRA_DIST = all.sh test_ds_sds_index.c test_ds_sds_index_multiple.c test_ds_sds_index_invalid.c sds.xml sds_multiple.xml sds_invalid.xml
