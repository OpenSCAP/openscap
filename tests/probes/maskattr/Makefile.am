DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*.out.??????
CLEANFILES = $(DISTCLEANFILES)

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

if WANT_PROBES_UNIX
if probe_file_enabled
TESTS = all.sh
endif
endif

check_SCRIPTS = test_object_entity_mask.sh test_object_entity_nomask.sh

EXTRA_DIST = \
	all.sh \
	test_object_entity_mask.sh \
	test_object_entity_mask.xml \
	test_object_entity_mask_oval_5_9.sh \
	test_object_entity_mask_oval_5_9.xml \
	test_object_entity_nomask.sh \
	test_object_entity_nomask.xml
