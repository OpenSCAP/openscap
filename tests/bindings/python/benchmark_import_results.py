#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>

import os
from import_handler import oscap, result2str, get_path


'''
Import and browse benchmark results

'''

benchmark = oscap.xccdf.benchmark_import(get_path("samples/xccdf_sample_results.xml"))
if benchmark.instance is None:
    raise Exception("Cannot import the benchmark {0}"
                    .format(get_path("samples/xccdf_sample_results.xml")))
else:
    print("Benchmark id: ", benchmark.get_id())
    results = benchmark.get_results()
    test_result = results.pop()
    print(test_result.get_id())

    for rs in test_result.get_rule_results():
        idref, result = rs.get_idref(), rs.get_result()
        print(idref, result2str(result))

        if (idref == "R-SHOULD_PASS" and
                result != oscap.xccdf.XCCDF_RESULT_PASS):

            raise Exception("Rule result for {0} should be PASS but is currently {1}."
                            .format(idref,
                                    result2str(result)))

        elif (idref == "R-SHOULD_FAIL" and
              result != oscap.xccdf.XCCDF_RESULT_FAIL):

            raise Exception("Rule result for {0} should be FAIL but is currently {1}."
                            .format(idref,
                                    result2str(result)))


# Now ensure that benchmark_import return None if the file doesn't exists

benchmark = oscap.xccdf.benchmark_import("../../oval_details/file_doesnt_exists.xml")
if benchmark.instance is not None:
    raise Exception("benchmark_import should return None if it can't open the file")
else:
    print("benchmark_import on a non existing file returns None.")
