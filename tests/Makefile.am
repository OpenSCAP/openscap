# Some of the tests rely on the "C" locale and would fail with some locales.
LC_ALL = C
export LC_ALL

if ENABLE_VALGRIND_TESTS
  export ENABLE_VALGRIND_TESTS=1
endif

PROBE_SUBDIRS = probes mitre

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
	$(PROBE_SUBDIRS) $(SCE_SUBDIRS) $(BINDINGS_SUBDIRS)

EXTRA_DIST = $(top_srcdir)/tests/test_common.sh \
	     $(top_srcdir)/tests/xmldiff.pl

