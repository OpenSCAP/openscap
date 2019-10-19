#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#

'''
Basic OVAL evaluation:

Import an oval file with oscap.oval.definition_model_import_source
and run evalutation of some sample checks using a callback

Tested functions:
    - oscap.oval.definition_model_import_source
    - oscap.common.source_new_from_file
    - TODO oscap.oval.import_model
    - oscap.oval.agent_new_session
    - oscap.oval.agent_eval_system + associated callback
    - oval_result_definition.get_id()
    - oval_result_definition.get_result()
    - oval_result_definition.get_criteria()

Tested in oval_helpers.browse_criteria:
    - oval_result_criteria.get_type() + some of associated constants OVAL_NODETYPE_*
    - oval_result_criteria.get_subnodes()
    - oval_result_criteria.get_test()

    - oval_result_test.get_test()
    - oval_result_test.get_result()

    - oval_test.get_subtype() + some of associated constants OVAL_SUBYTPE_*
    - oval_test.get_family() + some of associated constants
'''

import os
import time
from import_handler import oscap, result2str, get_path
from oval_helpers import browse_criteria


'''
Intermediate functions
'''


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
       ================        MAIN TEST           ====================
'''


print("Opening oval file with original C functions ... ")
oval_defs = oscap.oval.definition_model_import_source(
    oscap.common.source_new_from_file(get_path("samples/oval_sample.xml")))

oval_eval_sample(oval_defs)

# TODO: do the same thing with oval_import
