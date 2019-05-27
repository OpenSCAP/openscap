DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = all.sh \
			 rds-invalid.xml \
			 rds-valid.xml \
			 sds-1.3-invalid.xml \
			 sds-1.3-valid.xml \
			 sds-invalid-oval.xml \
			 sds-invalid-xccdf.xml \
			 sds-invalid.xml \
			 sds-valid.xml
