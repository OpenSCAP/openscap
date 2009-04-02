noinst_LTLIBRARIES = liboval.la

liboval_la_SOURCES  = oval_affected.c oval_behavior.c oval_collection.c
liboval_la_SOURCES += oval_component.c oval_criteriaNode.c oval_definition.c
liboval_la_SOURCES += oval_entity.c oval_object.c oval_objectContent.c
liboval_la_SOURCES += oval_reference.c oval_result.c oval_resultCriteriaNode.c
liboval_la_SOURCES += oval_resultDirectives.c oval_resultItem.c oval_resultTest.c
liboval_la_SOURCES += oval_set.c oval_state.c oval_sysData.c oval_sysInfo.c
liboval_la_SOURCES += oval_sysInterface.c oval_syschar.c oval_test.c
liboval_la_SOURCES += oval_value.c oval_variable.c oval_variableBinding.c
liboval_la_SOURCES += oval_collection_impl.h oval_definitions_impl.h
liboval_la_SOURCES += oval_results_impl.h oval_system_characteristics_impl.h
liboval_la_CFLAGS  = @libxml_cflags@
liboval_la_LDFLAGS = @libxml_libs@

include_HEADERS  = api/oval_agent_api.h api/oval_definitions.h 
include_HEADERS += api/oval_results.h api/oval_system_characteristics.h

includedir = $(prefix)/include/lib@PACKAGE@
