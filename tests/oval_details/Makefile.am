EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.* *.result.xml countries.xml foo.txt
CLEANFILES = *.log *.results oscap_debug.log.* *.result.xml countries.xml foo.txt

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_oval_details.sh

EXTRA_DIST +=	test_oval_details.sh \
		countries.src.xml \
		file.oval.xml \
		file.xccdf.xml \
		foo.src.txt \
		partition.oval.xml \
		partition.xccdf.xml \
		rpminfo.oval.xml \
		rpminfo.xccdf.xml \
		runlevel.oval.xml \
		runlevel.xccdf.xml \
		sysctl.oval.xml \
		sysctl.xccdf.xml \
		textfilecontent54.oval.xml \
		textfilecontent54.xccdf.xml \
		variable.oval.xml \
		variable.xccdf.xml \
		xmlfilecontent.oval.xml \
		xmlfilecontent.xccdf.xml
