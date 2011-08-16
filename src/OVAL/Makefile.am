# the result should be two libraries
# liboval.la is the regular one for make install
# liboval_testing.la is for purpose of make check
# why?
# because _testing looks for probes in different path!!!
#

# lib_ right now, but will get a better prefix according to where it shoud go to
noinst_LTLIBRARIES = liboval.la
# this is not built during "make all" nor "make install," only during "make check"
noinst_LTLIBRARIES += liboval_testing.la

# the common sources are not going to be built twice because we do not use
# per-target flag variables (liboval_la_*FLAGS etc.)
liboval_la_SOURCES = $(common_oval_sources)
liboval_testing_la_SOURCES = $(common_oval_sources)

common_oval_sources = \
	oval_agent.c \
	oval_defModel.c \
	oval_sysModel.c \
	oval_resModel.c \
	oval_affected.c \
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
	oval_filter.c \
	oval_generator.c \
	oval_message.c \
	oval_object.c \
	oval_objectContent.c \
	oval_parser.c \
	oval_parser_impl.h \
	oval_probe.c	\
	oval_recordField.c \
	oval_reference.c \
	oval_resultCriteriaNode.c \
	oval_resultDefinition.c \
	oval_resultItem.c \
	oval_resultSystem.c \
	oval_resultTest.c \
	oval_results_impl.h \
	oval_directives.c \
	oval_directives_impl.h \
	oval_set.c \
	oval_state.c \
	oval_stateContent.c \
	oval_stringMap.c \
	oval_string_map_impl.h \
	oval_sysEnt.c \
	oval_sysInfo.c \
	oval_sysInterface.c \
	oval_sysItem.c \
	oval_syschar.c \
	oval_system_characteristics_impl.h \
	oval_test.c \
	oval_value.c \
	oval_variable.c \
	oval_variableBinding.c \
	oval_sys_parser.c \
	oval_varModel.c \
	oval_probe_session.c	\
	_oval_probe_session.h	\
	oval_probe_handler.c	\
	_oval_probe_handler.h \
	fts_sun.c \
	fts_sun.h \
	oval_probe_meta.h

# -I options go to CPPFLAGS, not CFLAGS
# lets build one common for both normal and probe sources (does it mind?)
AM_CPPFLAGS = $(libxml_cflags) $(pthread_cflags) $(pcre_cflags) -I$(srcdir)/public -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src -I$(top_srcdir)/src/OVAL/probes/SEAP/generic
if WANT_PROBES
AM_CPPFLAGS += -I$(top_srcdir)/src -I$(top_srcdir)/src/common -I$(srcdir)/probes -I$(srcdir)/probes/public -I$(srcdir)/probes/SEAP/public -DSEAP_MSGID_BITS=32 -DSEAP_THREAD_SAFE -DOVAL_PROBE_DIR='"$(probe_dir)"'
endif
if WANT_XCCDF
AM_CPPFLAGS += -I$(top_srcdir)/src/XCCDF_POLICY/public -I$(top_srcdir)/src/XCCDF/public -DENABLE_XCCDF
endif

# -l or -L options go to LIBADD (or LDADD), not to LDFLAGS
liboval_la_LIBADD = $(libxml_libs) $(pthread_ldflags) $(pcre_libs)
liboval_testing_la_LIBADD = $(liboval_la_LIBADD)

pkginclude_HEADERS  =	public/oval_agent_api.h \
			public/oval_definitions.h \
			public/oval_system_characteristics.h \
			public/oval_results.h \
			public/oval_variables.h \
			public/oval_directives.h \
			public/oval_types.h \
			public/oval_probe.h \
			public/oval_probe_session.h \
			public/oval_probe_handler.h

if WANT_PROBES
common_oval_sources += probes/probe-api.c 	\
		       probes/_probe-api.h	\
		       probes/fsdev.c		\
		       probes/oval_fts.c	\
		       probes/oval_fts.h	\
		       probes/public/probe-api.h\
		       probes/public/probe-common.h\
		       probes/public/fsdev.h	\
		       probes/public/findfile.h \
		       probes/probe/ncache.c	\
		       probes/probe/ncache.h	\
		       probes/probe/rcache.c	\
		       probes/probe/rcache.h	\
		       probes/probe/entcmp.c	\
		       probes/probe/entcmp.h	\
		       oval_sexp.c 		\
		       oval_sexp.h 		\
		       oval_probe_ext.h		\
		       oval_probe_impl.h

liboval_la_SOURCES += oval_probe_ext.c
nodist_liboval_testing_la_SOURCES = oval_testing_ext_probe.c

SUBDIRS = probes/SEAP
liboval_la_LIBADD  += probes/SEAP/libseap.la
endif

CLEANFILES = $(nodist_liboval_testing_la_SOURCES)

oval_testing_ext_probe.c: Makefile
	(echo '#define OVAL_PROBEDIR_ENV 1'; \
	 echo '#include <oval_probe_ext.c>' ) >$@
