#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>

import sys
import os
import openscap_api as oscap


'''
Ensure that the openscap_api is really imported from the desired install dir
Python doesn't directly use env(PYTHONPATH), but sys.path which aggregate
potential paths. So we check that the import path is in PYTHONPATH.

To learn more about sys.path:
https://stackoverflow.com/questions/897792/where-is-pythons-sys-path-initialized-from/38403654#38403654
'''
if os.path.dirname(oscap.__file__) not in os.getenv('PYTHONPATH'):
    raise Exception("openscap_api is loaded but from the local env "
                    "instead of the tested environment.\n"
                    "Loaded module path = {0}".format(oscap.__file__))
else:
    print("openscap_api loaded from "+oscap.__file__)
    pass  # import is loaded from the right env


'''
Return the string corresponding to the oscap result (PASS, FAIL etc.)
'''


def result2str(result):
    if result == oscap.xccdf.XCCDF_RESULT_PASS:
        return "PASS"
    elif result == oscap.xccdf.XCCDF_RESULT_FAIL:
        return "FAIL"
    elif result == oscap.xccdf.XCCDF_RESULT_ERROR:
        return "ERROR"
    elif result == oscap.xccdf.XCCDF_RESULT_UNKNOWN:
        return "UNKNOWN"
    elif result == oscap.xccdf.XCCDF_RESULT_NOT_APPLICABLE:
        return "NOT_APPLICABLE"
    elif result == oscap.xccdf.XCCDF_RESULT_NOT_CHECKED:
        return "NOT_CHECKED"
    elif result == oscap.xccdf.XCCDF_RESULT_NOT_SELECTED:
        return "NOT_SELECTED"
    elif result == oscap.xccdf.XCCDF_RESULT_INFORMATIONAL:
        return "INFORMATIONAL"
    elif result == oscap.xccdf.XCCDF_RESULT_FIXED:
        return "FIXED"


'''
Return the absolute path of a relative path (located in this folder)
Required because the tests are run from the build directory,
so you need to use this functions for all relative paths.
'''


def get_path(path_str):
    return os.path.join(os.path.dirname(__file__), path_str)
