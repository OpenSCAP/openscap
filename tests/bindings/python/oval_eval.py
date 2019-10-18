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
from import_handler import oscap, result2str, get_path
import time


'''
Functions
'''


def oval_callback(msg, usr):
    print(msg.get_id()+" => "+result2str(msg.eval())) #eval or get_result return the same thing
    print(msg.get_criteria().__repr__())
    return 0  # if you return something else than 0, the current session stops


# eval oval defs oval:[foo_pass|fail]:def:1
def oval_eval_sample(oval_defs):
    states = {'false': 0, 'true': 0, 'err': 0, 'unknown': 0, 'neval': 0,
              'na': 0, 'verbose': True}

    sess = oscap.oval.agent_new_session(oval_defs, "")
    ret = oscap.oval.agent_eval_system(sess, oval_callback, states)


'''
Main test
'''


print("Opening oval file with original C functions ... ")
oval_defs = oscap.oval.definition_model_import_source(
    oscap.common.source_new_from_file(get_path("samples/oval_sample.xml")))

oval_eval_sample(oval_defs)

# TODO: do the same thing with oval_import

print("finish")


