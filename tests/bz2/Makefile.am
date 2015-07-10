
EXTRA_DIST = \
	$(top_srcdir)/tests/assume.h \
	$(top_srcdir)/tests/DS/sds_multiple_oval/multiple-oval-xccdf.xml

DISTCLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)
CLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)

AM_CPPFLAGS = \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/source/public \
	-I$(top_srcdir)/tests/include
LDADD = $(top_builddir)/src/libopenscap_testing.la

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

if HAVE_BZIP2
TESTS = all.sh
check_PROGRAMS = \
	test_bz2_memory_source
endif

test_bz2_memory_source_SOURCES = test_bz2_memory_source.c

EXTRA_DIST += \
	all.sh \
	test_bz2_datastream.sh
