EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log oscap_debug.log.*
CLEANFILES = *.log oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST += \
	all.sh \
	selinux_domain_label.oval.xml \
	selinux_domain_label.sh \
	dev_to_tty.oval.xml \
	dev_to_tty.sh \
	loginuid.oval.xml \
	loginuid.sh \
	sessionid.oval.xml \
	sessionid.sh \
	command_line.xml \
	command_line.sh
