#!/usr/bin/env python

# Author:
#   Dominique Blaze <contact@d0m.tech>

import os
from import_handler import oscap, rule_result2str


'''
Import and browse benchmark results

'''

abs_path = os.path.join(os.path.dirname(__file__), "samples/xccdf_sample_results.arf")

benchmark = oscap.xccdf.benchmark_import(abs_path)
if benchmark.instance is None:
    raise Exception("Cannot import the benchmark {0}".format(abs_path))
else:
    print("Benchmark id: ", benchmark.get_id())
    results = benchmark.get_results()
    test_result = results.pop()
    print(test_result.get_id())

    for rs in test_result.get_rule_results():
        print(rs.get_idref(), rule_result2str(rs.get_result()))

        if rs.get_result() != oscap.xccdf.XCCDF_RESULT_PASS:
            raise Exception("Rule result should be PASS but is currently {0}."
                            .format(rule_result2str(rs.get_result())))

# Now ensure that benchmark_import return None if the file doesn't exists

benchmark = oscap.xccdf.benchmark_import("../../oval_details/file_doesnt_exists.xml")
if benchmark.instance is not None:
    raise Exception("benchmark_import should return None if it can't open the file")
else:
    print("benchmark_import on a non existing file returns None.")
