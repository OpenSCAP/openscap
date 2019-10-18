#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
# !! WIP !!

import os
from import_handler import oscap, result2str, get_path


'''
Basic OVAL evaluation:
 
Import an oval file with oscap.oval.definition_model_import_source
and run evalutation of some sample checks with oscap.oval.agent_new_session
and oscap.oval.agent_eval_system using a callback
'''

def oval_callback(msg, usr):
	print("Msg returned by callback: "+msg.__repr__());
	#result: <Oscap Object of type 'oval_result_definition' with instance '<Swig Object of type 'struct oval_result_definition *' at 0x7f54f8f92870>'>
	print(result2str(msg.get_result()));


# eval oval defs oval:[foo_pass|fail]:def:1
def sample_oval_eval(oval_defs):
	states = {'false':0,'true':0,'err':0,'unknown':0,'neval':0,'na':0,'verbose':True}
	sess = oscap.oval.agent_new_session(oval_def, "oval:foo_pass:def:1");
	ret = oscap.oval.agent_eval_system(sess, oval_callback, states)
