set (OVAL_SOURCES
    "collectVarRefs.c"
    "collectVarRefs_impl.h"
    "oval_agent.c"
    "oval_session.c"
    "oval_defModel.c"
    "oval_sysModel.c"
    "oval_affected.c"
    "oval_agent_api_impl.h"
    "oval_behavior.c"
    "oval_component.c"
    "oval_criteriaNode.c"
    "oval_definition.c"
    "oval_definitions_impl.h"
    "oval_entity.c"
    "oval_enumerations.c"
    "oval_filter.c"
    "oval_generator.c"
    "oval_glob_to_regex.c"
    "oval_glob_to_regex.h"
    "oval_message.c"
    "oval_object.c"
    "oval_objectContent.c"
    "oval_parser.c"
    "oval_parser_impl.h"
    "oval_recordField.c"
    "oval_reference.c"
    "oval_directives.c"
    "oval_directives_impl.h"
    "oval_schema_version.c"
    "oval_set.c"
    "oval_state.c"
    "oval_stateContent.c"
    "oval_sysEnt.c"
    "oval_sysInfo.c"
    "oval_sysInterface.c"
    "oval_sysItem.c"
    "oval_syschar.c"
    "oval_syscharIterator.c"
    "oval_system_characteristics_impl.h"
    "oval_test.c"
    "oval_value.c"
    "oval_variable.c"
    "oval_variableBinding.c"
    "oval_sys_parser.c"
    "oval_varModel.c"
    "oval_vardefMapping.c"
    "oval_version.c"
)

list(APPEND OVAL_SOURCES
    "oval_probe.c"
    "oval_probe_hint.c"
    "oval_probe_session.c"
    "_oval_probe_session.h"
    "oval_probe_handler.c"
    "_oval_probe_handler.h"
    "fts_sun.c"
    "fts_sun.h"
    "oval_probe_meta.h"
    "probes/probe-api.c"
    "probes/_probe-api.h"
    "probes/fsdev.c"
    "probes/oval_fts.c"
    "probes/oval_fts.h"
    "probes/public/probe-api.h"
    "probes/public/probe-common.h"
    "probes/public/fsdev.h"
    "probes/public/findfile.h"
    "probes/probe/ncache.c"
    "probes/probe/ncache.h"
    "probes/probe/rcache.c"
    "probes/probe/rcache.h"
    "probes/probe/entcmp.c"
    "probes/probe/entcmp.h"
    "oval_sexp.c"
    "oval_sexp.h"
    "oval_probe_ext.h"
    "oval_probe_impl.h"
)

list(APPEND OVAL_SOURCES
    "oval_probe_ext.c"
)

include_directories(
    "../../"
    "probes"
    "probes/public"
    #"probes/SEAP"
    #"probes/SEAP/public"
)

add_library(oval_object OBJECT ${OVAL_SOURCES})
add_library(oval $<TARGET_OBJECTS:oval_object>)
target_link_libraries(oval oscapsource ovalresults)

set_target_properties(oval_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)

add_subdirectory("results")
add_subdirectory("adt")
add_subdirectory("probes")
