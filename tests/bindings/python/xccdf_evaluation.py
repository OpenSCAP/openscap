#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#
# This script aims to verify the evaluation of an xccdf_session
#

import os
from import_handler import oscap, result2str, get_path


print("opening ./samples/xccdf_sample.xml ...")
sess = oscap.xccdf.session_new(get_path("samples/xccdf_sample.xml"))

res = sess.load()

if not res == 0:
    raise ValueError("Return value of xccdf_session_load isn't 0 but {0} !".format(res))

'''
rules to test whith desired return value
'''

rules = {"R-SHOULD_PASS": oscap.common.XCCDF_RESULT_PASS,
         "R-SHOULD_FAIL": oscap.common.XCCDF_RESULT_FAIL}

res = sess.evaluate()

if not res == 0:
    raise ValueError("Return value of xccdf_session_evaluate isn't 0 but {0} !".format(res))

if sess.get_base_score() != 50:
    raise ValueError("base score should be 50 but is {0} ", sess.get_base_score())

for rs in sess.get_xccdf_policy().get_results():
    for rr in rs.get_rule_results():
        print("Rule {0} evaluated as {1}"
              .format(rr.get_idref(), result2str(rr.get_result())))

        if rules[rr.get_idref()] != rr.get_result():
                raise ValueError("Result value of {0} should be {1} but is {2}"
                                 .format(rr.get_idref(), rules[rr.get_idref()], rr.get_result()))

sess.free()
