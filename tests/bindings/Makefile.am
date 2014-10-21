DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT = \
		$(top_builddir)/run

TESTS =
if WANT_PYTHON
TESTS +=test_python.sh
endif
if WANT_PERL
TESTS += test_perl.sh
endif

EXTRA_DIST =	test_python.sh \
		test_perl.sh
