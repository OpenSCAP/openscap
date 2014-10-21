DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = all.sh \
			  sds-signed.xml \
			  sds-signed-key.pem \
			  sds-signed-key.pub \
			  sds-signed-fake-x509.xml
