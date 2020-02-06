#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#
# This test aims to verify if xccdf_session works well with
# the xccdf_session_set_rule call.
#

import os
from import_handler import oscap, result2str, get_path

# ======================     Part 1   ============================

print("opening ./samples/xccdf_sample.xml ...")
sess = oscap.xccdf.session_new(get_path("samples/xccdf_sample.xml"))

res = sess.load()

''' rules to test which desired return value in this format:
    (rule_name, expected_result_id, expected_base_score)
'''

rules = (("R-SHOULD_PASS", oscap.common.XCCDF_RESULT_PASS, 100),
         ("R-SHOULD_FAIL", oscap.common.XCCDF_RESULT_FAIL, 0))

for rule in rules:
    print("evaluating rule {0}".format(rule[0]))
    sess.set_rule(rule[0])

    if not res == 0:
        raise ValueError("Return value of xccdf_session_load isn't 0 but {0} !".format(res))

    res = sess.evaluate()

    if not res == 0:
        raise ValueError("Return value of xccdf_session_evaluate isn't 0 but {0} !".format(res))

    if sess.get_base_score() != rule[2]:
        print("base score for {0} should be {1) but is {2} "
              .format(rule[0], rule[2], sess.get_base_score()))

    rs = sess.get_xccdf_policy().get_results()[-1]  # get last value

    for rr in rs.get_rule_results():
        if rule[0] == rr.get_idref():
            print("Result = {0}".format(result2str(rr.get_result())))
            if rule[1] != rr.get_result():
                raise ValueError("Result value of {0} should be {1} but is {2}"
                                 .format(rule[0], rule[1], rr.get_result()))

# release session
sess.free()
