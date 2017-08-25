add_library(ovalcmp_object OBJECT
    "oval_cmp.c"
    "oval_cmp_impl.h"
    "oval_cmp_basic.c"
    "oval_cmp_basic_impl.h"
    "oval_cmp_evr_string.c"
    "oval_cmp_evr_string_impl.h"
    "oval_cmp_ip_address.c"
    "oval_cmp_ip_address_impl.h"
)
add_library(ovalcmp $<TARGET_OBJECTS:ovalcmp_object>)
set_target_properties(ovalcmp_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)

add_library(ovalresults_object OBJECT
    "oval_resModel.c"
    "oval_resultCriteriaNode.c"
    "oval_resultDefinition.c"
    "oval_resultDefinitionIterator.c"
    "oval_resultItem.c"
    "oval_results_impl.h"
    "oval_resultSystem.c"
    "oval_resultTest.c"
    "oval_resultTestIterator.c"
    "oval_status_counter.c"
    "oval_status_counter.h"
)
add_library(ovalresults $<TARGET_OBJECTS:ovalresults_object>)
target_link_libraries(ovalresults oval)
set_target_properties(ovalresults_object PROPERTIES
    CXX_VISIBILITY_PRESET hidden	# Make global variables and functions HIDDEN by default.
    POSITION_INDEPENDENT_CODE ON	# Compile this object code position independent.
)
