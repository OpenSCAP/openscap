# the result should be two libraries
# liboval.la is the regular one for make install
# liboval_testing.la is for purpose of make check
# why?
# because _testing looks for probes in different path!!!
#

# lib_ right now, but will get a better prefix according to where it shoud go to
noinst_LTLIBRARIES = liboval.la
# this is not built during "make all" nor "make install," only during "make check"
check_LTLIBRARIES = liboval_testing.la

# the common sources are not going to be built twice because we do not use
# per-target flag variables (liboval_la_*FLAGS etc.)
liboval_la_SOURCES = $(common_oval_sources)
liboval_testing_la_SOURCES = $(common_oval_sources)

common_oval_sources = \
	oval_affected.c \
	oval_agent.c \
	oval_agent_api_impl.h \
	oval_behavior.c \
	oval_collection.c \
	oval_collection_impl.h \
	oval_component.c \
	oval_criteriaNode.c \
	oval_definition.c \
	oval_definitions_impl.h \
	oval_entity.c \
	oval_enumerations.c \
	oval_errno.h \
	oval_message.c \
	oval_object.c \
	oval_objectContent.c \
	oval_parser.c \
	oval_parser_impl.h \
	oval_reference.c \
	oval_result.c \
	oval_resultCriteriaNode.c \
	oval_resultDirectives.c \
	oval_resultDefinition.c \
	oval_resultItem.c \
	oval_resultSystem.c \
	oval_resultTest.c \
	oval_results_impl.h \
	oval_results_parser.c \
	oval_set.c \
	oval_state.c \
	oval_stateContent.c \
	oval_stringMap.c \
	oval_string_map_impl.h \
	oval_sysData.c \
	oval_sysInfo.c \
	oval_sysInterface.c \
	oval_sysItem.c \
	oval_syschar.c \
	oval_system_characteristics_impl.h \
	oval_test.c \
	oval_value.c \
	oval_variable.c \
	oval_variableBinding.c \
	ovalsys_parser.c

# -I options go to CPPFLAGS, not CFLAGS
# lets build one common for both normal and probe sources (does it mind?)
AM_CPPFLAGS = $(libxml_cflags) -I.. $(pthread_cflags)
if WANT_PROBES
AM_CPPFLAGS += -I$(top_srcdir)/src -I$(srcdir)/probes -I$(srcdir)/probes/SEAP/public
endif

# -l or -L options go to LIBADD (or LDADD), not to LDFLAGS
liboval_la_LIBADD = $(libxml_libs) $(pthread_ldflags)
liboval_testing_la_LIBADD = $(liboval_la_LIBADD)

pkginclude_HEADERS  =	api/oval_agent_api.h api/oval_definitions.h \
			api/oval_results.h api/oval_system_characteristics.h


if WANT_PROBES
common_oval_sources += probes/probe-api.c 	\
		       probes/fsdev.c		\
		       probes/findfile.c 	\
		       oval_sexp.c		\
		       oval_sexp.h		\
		       oval_probe.h

liboval_la_SOURCES += oval_probe.c
nodist_liboval_testing_la_SOURCES = oval_testing_probe.c

SUBDIRS = probes/SEAP
liboval_la_LIBADD  += probes/SEAP/libseap.la

pkginclude_HEADERS += \
	probes/findfile.h \
	probes/fsdev.h \
	probes/probe-entcmp.h \
	probes/probe-api.h
endif

CLEANFILES = $(nodist_liboval_testing_la_SOURCES)

oval_testing_probe.c: Makefile
	(echo '#define PROBEPATH_ENV 1'; \
	 echo '#include <oval_probe.c>' ) >$@

