#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>
#
# Description:
#   Test introspection features of python the api
#    - Test the presence or the absence of expected builtins functions
#    - Indentify a constant name by its value and prefix (prefix XCCDF_RESULT
#       with value 1 (usually) should identify XCCDF_RESULT_PASS
#    - Check presence of constants filtered by a prefix (XCCDF_RESULT with
#       value = None should returns all XCCDF_RESULT_* constants).
#


from import_handler import oscap
from pprint import pprint

oval_funcs = oscap.oval.introspect()

if oval_funcs.get('oval_variable_get_values') is None:
    raise Exception("method 'oval_variable_get_values' not found in builtins"
                    "functions of oval object (should be in oscap.oval.introspect())")

if oval_funcs.get('xccdf_check_get_id') is not None:
    raise Exception("method 'xccdf_check_get_id' should not be found in "
                    "oscap.oval.introspect(), because the prefix isn't 'oval')")

if oscap.oval.introspect_all().get('xccdf_check_get_id') is None:
    raise Exception("method 'xccdf_check_get_id' should be present in "
                    "oscap.oval.introspect_all(), which returns all available builtins functions")

print("Introspection of builtins functions seems working.")


# should returns {'XCCDF_RESULT_PASS': 1}
# but it's not impossible that the numeric value change (in fact we don't care of it)
pass_val = oscap.oval.XCCDF_RESULT_PASS  # is usually 1

var1 = oscap.oval.introspect_constants(pass_val, "XCCDF_RESULT")
if len(var1) == 1 and list(var1.keys())[0] == "XCCDF_RESULT_PASS":
    print("Introspection of oscap.oval.XCCDF_RESULT_PASS ok")


xccdf_results = oscap.oval.introspect_constants(None, "XCCDF_RESULT")
# should returns something like {
# 'XCCDF_RESULT_ERROR': 3,
# 'XCCDF_RESULT_FAIL': 2,
# 'XCCDF_RESULT_FIXED': 9,
# 'XCCDF_RESULT_INFORMATIONAL': 8,
# 'XCCDF_RESULT_NOT_APPLICABLE': 5,
# 'XCCDF_RESULT_NOT_CHECKED': 6,
# 'XCCDF_RESULT_NOT_SELECTED': 7,
# 'XCCDF_RESULT_PASS': 1,
# 'XCCDF_RESULT_UNKNOWN': 4}

expected_constants = ('XCCDF_RESULT_ERROR', 'XCCDF_RESULT_FAIL', 'XCCDF_RESULT_FIXED',
                      'XCCDF_RESULT_INFORMATIONAL', 'XCCDF_RESULT_NOT_APPLICABLE',
                      'XCCDF_RESULT_NOT_CHECKED', 'XCCDF_RESULT_PASS',
                      'XCCDF_RESULT_NOT_SELECTED', 'XCCDF_RESULT_UNKNOWN')

for c in expected_constants:
    if c not in xccdf_results:
        raise Exception("oscap.oval.introspect_constants(None, 'XCCDF_RESULT) should"
                        "contains the constant {0}.".format(c))

print("Introspection of constants ok with prefix XCCDF_RESULT_*")
