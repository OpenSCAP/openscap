#!/usr/bin/python3

import os, sys

# The following code is meant as a snippet and might go into a reusable package
# in the future. For now it's just a proof of concept that languages can employ
# means to make reading bound xccdf values more convenient

class XccdfValue(object):
    """A wrapper containing all exposed XCCDF value binding data
    """
    
    def __init__(self, type_, name, value, operator):
        self.type_ = type_
        self.name = name
        self.value = value
        self.operator = operator

def get_xccdf_values():
    """Parses all passed environment variables and tries to deduce XccdfValues

    the result is delivered as a dict mapping value name to xccdf value wrapper
    containing all exposed info
    """

    ret = {}
    for key, value in os.environ.items():
        if key.startswith("XCCDF_VALUE_"):
            name = key[12:]
            value = value
            type_ = os.environ["XCCDF_TYPE_%s" % (name)]

            operator = "EQUALS"
            try:
                operator = os.environ["XCCDF_OPERATOR_%s" % (name)]
            except:
                # operator is not mandatory
                pass

            wrapper = XccdfValue(type_, name, value, operator)
            ret[name] = wrapper

    return ret

values = get_xccdf_values()

passed_value = values["passed_value"]

if int(passed_value.value) == 16:
    sys.exit(int(os.environ["XCCDF_RESULT_PASS"]))
else:
    sys.exit(int(os.environ["XCCDF_RESULT_FAIL"]))

