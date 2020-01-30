#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#

import os
from import_handler import oscap, result2str, get_path

# ======================     Part 1   ============================

print("opening ./samples/xccdf_sample.xml ...")
sess = oscap.xccdf.session_new(get_path("samples/xccdf_sample.xml"))


''' rules to test which desired return value '''
rules=(("R-SHOULD_PASS", oscap.common.XCCDF_RESULT_PASS),
       ("R-SHOULD_FAIL", oscap.common.XCCDF_RESULT_FAIL))


for rule in rules:
    print("evaluating rule {0}".format(rule[0]))
    sess.set_rule_py(rule[0])
    res=sess.load()

    if not res == 0:
        raise ValueError("Return value of xccdf_session_load isn't 0 but {0} !".format(res))


    res=sess.evaluate()
    if not res == 0:
        raise ValueError("Return value of xccdf_session_evaluate isn't 0 but {0} !".format(res))


    print("base score : {0} ".format(sess.get_base_score()))

    for r in sess.get_xccdf_policy().get_results():
        for rr in r.get_rule_results():
            print(rr.get_idref())
            print(rr.get_result())
        
    
