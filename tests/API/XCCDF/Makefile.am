
EXTRA_DIST =
TESTS =
check_DATA =

TESTS_ENVIRONMENT = \
	builddir=$(top_builddir) \
	OSCAP_FULL_VALIDATION=1 \
	$(top_builddir)/run


DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*


include unittests/Makefile.am
include variable_instance/Makefile.am


NUL =
SUBDIRS = \
	$(NUL)

X = \
	applicability \
	default_cpe \
	fix \
	guide \
	parser \
	progress \
	report \
	result_files \
	tailoring \
	$(NUL)
