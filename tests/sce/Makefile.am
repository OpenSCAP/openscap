DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = test_sce.sh \
		test_passing_vars.sh \
		test_check_engine_results.sh

EXTRA_DIST =	test_sce.sh \
		sce_xccdf.xml \
		bash_passer.sh \
		lua_passer.lua \
		python_passer.py \
		python_is16.py \
		test_check_engine_results.xccdf.xml \
		empty_stdout.sh
