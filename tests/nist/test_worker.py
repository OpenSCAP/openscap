#!/usr/bin/python2

import xml.etree.ElementTree as etree
import subprocess
import sys
import os
import argparse

xccdf_ns = "http://checklists.nist.gov/xccdf/1.2"
xccdf_1_1_ns = "http://checklists.nist.gov/xccdf/1.1"
oval_result_ns = "http://oval.mitre.org/XMLSchema/oval-results-5"

def get_test_property(test, property_name):
    property_element = test.find(property_name)
    if property_element is not None:
        return property_element.text
    else:
        return None

def get_datastream_file_name(test):
    return get_test_property(test, "datastreamFile")

def get_profile_name(test):
    return get_test_property(test, "xccdfProfile")

def get_datastream_id(test):
    return get_test_property(test, "datastreamID")

def get_tailoring_id(test):
    return get_test_property(test, "tailoringID")

def get_benchmark_id(test):
    return get_test_property(test, "benchmarkID")

def get_schema(test):
    return get_test_property(test, "schema")

def get_oval_file_name(test):
    return get_test_property(test, "ovalFilename")

def get_oval_variables_file_name(test):
    return get_test_property(test, "ovalVariables")

def are_remote_resources_allowed(test):
    remote_resources = get_test_property(test, "remoteResources")
    return remote_resources == "true"

def run_test(test, scanner):
    test_id = test.get("suiteId")
    datastream = get_datastream_file_name(test)
    oval = get_oval_file_name(test)
    oval_variables = get_oval_variables_file_name(test)
    datastream_id = get_datastream_id(test)
    tailoring_id = get_tailoring_id(test)
    benchmark_id = get_benchmark_id(test)
    profile = get_profile_name(test)
    command = scanner
    if datastream:
        results_file = test_id + ".results_arf.xml"
        command += " xccdf eval"
        if are_remote_resources_allowed(test):
            command += " --fetch-remote-resources"
        if datastream_id:
            command += " --datastream-id " + datastream_id
        if tailoring_id:
            command += " --tailoring-id " + tailoring_id
        if profile:
            command += " --profile " + profile
        if benchmark_id:
            command += " --benchmark-id " + benchmark_id
        command += " --results-arf " + results_file + " " + datastream
    elif oval:
        results_file = test_id + ".results_oval.xml"
        command += " oval eval"
        if oval_variables:
            command += " --variables " + oval_variables
        command += " --results " + results_file + " " + oval
    else:
        sys.stderr.write("Unable to run the test! Check the catalog.xml.\n")
        return None
    print(command)
    stdout_file = open(test_id + ".stdout", "w")
    oscap_return_code = subprocess.call(["bash", "-c", command], stdout=stdout_file)
    stdout_file.close()

def find_actual_result_in_arf(arf_root, rule_id):
    rule_results = arf_root.findall(".//{%s}rule-result" % xccdf_ns)
    rule_results.extend(arf_root.findall(".//{%s}rule-result" % xccdf_1_1_ns))
    #print(rule_results)
    for rule_result in rule_results:
        if rule_result.get("idref") == rule_id:
            result = rule_result.find("{%s}result" % xccdf_ns)
            if result is None:
                result = rule_result.find("{%s}result" % xccdf_1_1_ns)
            if result is not None:
                return result.text
            else:
                return None
    return None

def find_actual_result_in_oval(oval_root, definition_id):
    definition_results = oval_root.findall("{" + oval_result_ns + "}results/{" +
            oval_result_ns + "}system/{" + oval_result_ns + "}definitions/{" +
            oval_result_ns + "}definition")
    for definition_result in definition_results:
        if definition_result.get("definition_id") == definition_id:
            result = definition_result.get("result")
            return result
    return None

def check_results(test):
    test_id = test.get("suiteId")
    all_expected_results_elements = test.findall("expectedResults")
    expected_results = None
    report_type = None
    for x in all_expected_results_elements:
        expected_type = x.get("reportType")
        if expected_type == "arf" or expected_type == "oval":
            expected_results = x
            report_type = expected_type
    if expected_results is None:
        print("Catalog doesn't provide expected results.")
        return False
    if report_type == "arf":
        results_file = test_id + ".results_arf.xml"
    elif report_type == "oval":
        results_file = test_id + ".results_oval.xml"
    try:
        actual_results_xml_tree = etree.parse(results_file)
    except IOError:
        print("ERROR: No results were generated in test " + test_id)
        return False
    actual_results_root = actual_results_xml_tree.getroot()
    rules = expected_results.findall("result")
    test_result = True
    for rule in rules:
        rule_id = rule.get("definition")
        expected_result = rule.get("expectedResult")
        if report_type == "arf":
            actual_result = find_actual_result_in_arf(actual_results_root, rule_id)
        if report_type == "oval":
            actual_result = find_actual_result_in_oval(actual_results_root, rule_id)
        if actual_result is None:
            test_result = False
            actual_result = "<None>"
        if actual_result != expected_result:
            print("Failing rule: rule_id: " + rule_id + ", expected: " + expected_result + ", actual: " + actual_result)
            test_result = False
    return test_result

def run_script(script_node):
    script = script_node.find("command").text
    if script is None:
        return
    script_type = script_node.get("commandType")
    old_wd = os.getcwd()
    os.chdir("configuration_scripts")
    print("Running setup script " + script)
    if script_type == "python":
        interpreter = "/usr/bin/python"
    else:
        interpreter = "/bin/bash"
    subprocess.call([interpreter, script])
    os.chdir(old_wd)
    # refresh bash
    subprocess.call(["/bin/bash", "-c", "exit"])

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--scanner", default="oscap",
            help="Specifies a custom path to oscap scanner.")
    parser.add_argument("testdir",
            help="Specifies a directory containing test case, its SCAP content and catalog.")
    parser.add_argument("--nosetup", default=False, action="store_true",
            help="Skips test setup, will not run configuartion scripts.")
    args = parser.parse_args()
    test_dir = args.testdir
    scanner_path = args.scanner

    os.chdir(test_dir)
    # open and parse the Catalog
    catalog_xml_tree = etree.parse("catalog.xml")
    catalog = catalog_xml_tree.getroot()
    return_value = 0
    for test in catalog.findall("testSuite"):
        test_id = test.get("suiteId")
        print("# " + test_id)
        schema = get_schema(test)
        if schema == "windows":
            print("SKIPPED")
            continue
        if "remote" in test_id:
            print("SKIPPED")
            continue
        run_test(test, scanner_path)
        if check_results(test):
            print("PASS")
        else:
            print("FAIL")
            return_value = 1
        print("")

    sys.exit(return_value)
