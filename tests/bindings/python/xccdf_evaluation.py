#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#

import os
from pprint import pprint
import openscap_api as oscap


# ======================     Part 1   ============================

print("opening ./samples/xccdf_sample.xml ...")
sess = oscap.xccdf.session_new("./samples/xccdf_sample.xml")
sess.load()
#pprint(sess.introspect_all())
print(sess.evaluate())

print("ok")
print(sess.get_base_score())
print(sess.get_xccdf_policy().get_result_by_id("R-SHOULD_PASS"))
for r in sess.get_xccdf_policy().get_results():
    for rr in r.get_rule_results():
        pprint(rr.get_idref())
        pprint(rr.get_result())
        
    
