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
set_oscap_generic_properties(ovalcmp_object)

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
set_oscap_generic_properties(ovalresults_object)
