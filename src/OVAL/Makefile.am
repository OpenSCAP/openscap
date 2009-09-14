noinst_LTLIBRARIES = liboval.la
liboval_la_SOURCES = \
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

liboval_la_CFLAGS  = @libxml_cflags@ -DPROBEPATH_ENV
liboval_la_LDFLAGS = @libxml_libs@

include_HEADERS  = api/oval_agent_api.h api/oval_definitions.h 
include_HEADERS += api/oval_results.h api/oval_system_characteristics.h

includedir = $(prefix)/include/lib@PACKAGE@

if WANT_PROBES
liboval_la_CFLAGS  += -I$(top_srcdir)/src -I$(top_srcdir)/src/OVAL/probes/ -I$(top_srcdir)/src/OVAL/probes/SEAP/public
liboval_la_SOURCES += probes/probe-api.c \
		      probes/fsdev.c \
		      probes/findfile.c \
		      oval_probe.c \
		      oval_probe.h
include_HEADERS += \
	probes/findfile.h \
	probes/fsdev.h \
	probes/probe-entcmp.h \
	probes/probe-api.h

liboval_la_LIBADD  = probes/SEAP/libseap.la
SUBDIRS = probes/SEAP
endif
