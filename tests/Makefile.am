if ENABLE_VALGRIND_TESTS
  export ENABLE_VALGRIND_TESTS=1
endif

if WANT_PROBES
PROBE_SUBDIRS = probes mitre
else
PROBE_SUBDIRS =
endif

if WANT_SCE
SCE_SUBDIRS = sce
endif

BINDINGS_SUBDIRS=
if WANT_PYTHON
  BINDINGS_SUBDIRS += bindings
else
if WANT_PERL
    BINDINGS_SUBDIRS += bindings
endif
endif

SUBDIRS = \
	API \
	bz2 \
	codestyle \
	DS \
	schemas \
	oscap_string \
	oval_details \
	$(PROBE_SUBDIRS) $(SCE_SUBDIRS) $(BINDINGS_SUBDIRS)

EXTRA_DIST = \
	suppressions.supp \
	valgrind_test.sh \
	xmldiff.pl

CONFIG_CLEAN_FILES = test_common.sh
