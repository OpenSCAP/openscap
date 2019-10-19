#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#

'''
Basic OVAL evaluation:

Import an oval file with oscap.oval.definition_model_import_source
and run evalutation of some sample checks with oscap.oval.agent_new_session
and oscap.oval.agent_eval_system using a callback
'''

import os
import time
from import_handler import oscap, result2str, get_path
from pprint import pprint
import inspect


'''
Intermediate functions
'''


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


# if you return something in callback else than 0, the current session stops
def oval_sample_callback(ovdef, usr):
    # .eval or .get_result return the same thing
    print(ovdef.get_id()+" => "+result2str(ovdef.get_result()))

    # retrieve the tests tree and replace test_results by [subtype => result]
    tests_tree = browse_criteria(ovdef.get_criteria(), 1)

    '''
    Expected tree for foo_pass: [[(7006, 1)]]
    = [[(OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, XCCDF_RESULT_PASS)]]
    or [[(OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54, XCCDF_RESULT_PASS)]] for foo_fail
    '''

    expected_trees = {"oval:foo_pass:def:1": [[
                         (oscap.oval.OVAL_FAMILY_INDEPENDENT,
                          oscap.oval.OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54,
                          oscap.oval.XCCDF_RESULT_PASS)]],
                      "oval:foo_fail:def:1": [[
                         (oscap.oval.OVAL_FAMILY_INDEPENDENT,
                          oscap.oval.OVAL_INDEPENDENT_TEXT_FILE_CONTENT_54,
                          oscap.oval.XCCDF_RESULT_FAIL)]]
                      }

    expected_tree = expected_trees.get(ovdef.get_id())

    if expected_tree is None:
        print("Unexpected oval def : {0}. No expected tests tree defined for it"
              .format(ovdef.get_id()))
    elif expected_tree == tests_tree:
        print("Tests tree of {0} is like expected : {1}".format(ovdef.get_id(),
                                                                tests_tree))
    else:
        raise ValueError("Tests tree of {0} doesn't match with the expected tree.\n"
              "Extracted tree: {1}\nExpected tree: {2}"
              .format(ovdef.get_id(), tests_tree, expected_tree))

    return 0


# eval oval defs oval:[foo_pass|fail]:def:1
def oval_eval_sample(oval_defs):
    states = {'false': 0, 'true': 0, 'err': 0, 'unknown': 0, 'neval': 0,
              'na': 0, 'verbose': True}

    sess = oscap.oval.agent_new_session(oval_defs, "")
    ret = oscap.oval.agent_eval_system(sess, oval_sample_callback, states)

'''
Main test
'''


print("Opening oval file with original C functions ... ")
oval_defs = oscap.oval.definition_model_import_source(
    oscap.common.source_new_from_file(get_path("samples/oval_sample.xml")))

oval_eval_sample(oval_defs)

# TODO: do the same thing with oval_import


