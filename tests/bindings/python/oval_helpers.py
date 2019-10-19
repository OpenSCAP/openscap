#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#
# Description:
#   Helpers functions in order to factorize other tests features
#   It can potentially be integrated later in the high layer python api
#   to provide convenience utilities.
#

def browse_criteria(crit_node, mode=0):
    '''
    Browse recursively criteria of an oval test and build a representation of it
    First list item is the operator, and the next ones are the tests
    If the item is a list, it's a criteria (else a criterion, or string for an
    extended defintion , or None for OVAL_NODETYPE_UNKNOWN

    Ex: [operator, "extended def", [operator, test_result2, test_result3]]

    Mode : with mode = 0, test_result is the swig object oval_test_result
    with mode = 1, test result is a a tuple like (test_subtype, test_result)
    for instance (oscap.oval.OVAL_LINUX_DPKG_INFO, OVAL_FAMILY_LINUX
    oscap.xccdf.XCCDF_RESULT_PASS)
    '''

    # init the critria list
    rs = list()

    if crit_node.get_type() == oscap.oval.OVAL_NODETYPE_CRITERIA:
        for c in crit_node.get_subnodes():
            rs.append(browse_criteria(c, mode))

    elif crit_node.get_type() == oscap.oval.OVAL_NODETYPE_CRITERION:
        if mode == 0:
            rs.append(crit_node.get_test())
        elif mode == 1:
            rs.append((crit_node.get_test().get_test().get_family(),
                       crit_node.get_test().get_test().get_subtype(),
                       crit_node.get_test().get_result()))
        else:
            raise ValueError("param mode in browse_criteria should be 0 or 1")

    elif crit_node.get_type() == oscal.oval.OVAL_NODETYPE_EXTENDDEF:  # !!! TODO !!!
        rs.append("extended def")

    else:
        rs.append(None)

    return rs
