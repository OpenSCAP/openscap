DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = all.sh \
			  sds-valid.xml \
			  sds-invalid.xml \
			  sds-invalid-xccdf.xml \
			  sds-invalid-oval.xml \
			  rds-valid.xml \
			  rds-invalid.xml
