include $(top_srcdir)/aminclude_static.am

if ENABLE_VALGRIND_TESTS
  export ENABLE_VALGRIND_TESTS=1
endif

PROBE_SUBDIRS = probes mitre

if WANT_SCE
SCE_SUBDIRS = sce
endif

BINDINGS_SUBDIRS=
if WANT_PYTHON2
  BINDINGS_SUBDIRS += bindings
else
if WANT_PERL
    BINDINGS_SUBDIRS += bindings
endif
endif

NUL =

SUBDIRS = \
	API \
	bz2 \
	codestyle \
	CPE \
	DS \
	sources \
	schemas \
	nist \
	offline_mode \
	oscap_string \
	oval_details \
	$(PROBE_SUBDIRS) $(SCE_SUBDIRS) $(BINDINGS_SUBDIRS)

EXTRA_DIST = \
	suppressions.supp \
	valgrind_test.sh \
	xmldiff.pl \
	test_common.sh.in

CONFIG_CLEAN_FILES = test_common.sh
