EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

# We use absolute paths for LD_* because relative paths would break tests
# whenever cd/pushd is used in them.
TESTS_ENVIRONMENT =	OVAL_PROBE_DIR=$(shell cd $(top_builddir); pwd)/src/OVAL/probes \
			LD_PRELOAD=$(shell cd $(top_builddir); pwd)/src/.libs/libopenscap_testing.so \
			LD_LIBRARY_PATH=$(shell cd $(top_builddir); pwd)/src/.libs:$(shell cd $(top_builddir); pwd)/swig/.libs \
			PYTHONPATH=$(shell cd $(top_builddir); pwd)/swig:$(shell cd $(top_srcdir); pwd)/swig:$(shell cd $(top_builddir); pwd)/swig/.libs \
			PERL5LIB=$(shell cd $(top_builddir); pwd)/swig:$(shell cd $(top_srcdir); pwd)/swig:$(shell cd $(top_builddir); pwd)/swig/.libs \
			OSCAP_SCHEMA_PATH=$(shell cd $(top_srcdir); pwd)/schemas \
			OSCAP_CPE_PATH=$(shell cd $(top_srcdir); pwd)/cpe

TESTS =
if WANT_PYTHON
TESTS +=test_python.sh
endif
if WANT_PERL
TESTS += test_perl.sh
endif

EXTRA_DIST +=	test_python.sh \
		test_perl.sh
