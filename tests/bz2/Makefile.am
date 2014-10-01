
EXTRA_DIST = \
	$(top_srcdir)/tests/assume.h \
	$(top_srcdir)/tests/test_common.sh \
	$(top_srcdir)/tests/DS/sds_multiple_oval/multiple-oval-xccdf.xml

DISTCLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)
CLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

if HAVE_BZIP2
TESTS = all.sh
endif

EXTRA_DIST += \
	all.sh \
	test_bz2_datastream.sh
