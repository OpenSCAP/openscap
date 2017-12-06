from __future__ import print_function

import argparse as ap
from xml.etree import ElementTree


NAMESPACES = dict(
    xccdf_ns="http://scap.nist.gov/schema/scap/source/1.2",
    profile_ns="http://checklists.nist.gov/xccdf/1.2",
)


class RuleResult(object):
    def __init__(self, element):
        self.idref = element.attrib["idref"]
        self.weight = element.attrib["weight"]

        result_element = element.find("{%s}result" % NAMESPACES["profile_ns"])
        self.result_text = result_element.text

        check_element = element.find("{%s}check" % NAMESPACES["profile_ns"])
        self.check_system = check_element.attrib["system"]
        self.check_system = check_element.attrib["system"]

        check_content_ref_element = check_element.find(
            "{%s}check-content-ref" % NAMESPACES["profile_ns"])
        self.content_ref_href = check_content_ref_element.attrib["href"]
        self.content_ref_name = check_content_ref_element.attrib["name"]

        check_export_element = check_element.find(
            "{%s}check-export" % NAMESPACES["profile_ns"])
        self.check_export_name = None
        self.check_export_value_id = None
        if check_export_element is not None:
            self.check_export_name = check_export_element.attrib["export-name"]
            self.check_export_value_id = check_export_element.attrib["value-id"]

    def __hash__(self):
        return hash(frozenset(self.__dict__.items()))

    def __eq__(self, rhs):
        return hash(self) == hash(rhs)


def fname_to_etree(fname):
    input_tree = ElementTree.parse(fname)
    return input_tree


def get_rule_results_from_etree(tree):
    xpath_expr =  ".//{%s}rule-result" % NAMESPACES["profile_ns"]
    xccdfs = tree.findall(xpath_expr)
    return xccdfs


def extract_results_from_file(fname):
    tree = fname_to_etree(fname)
    results = get_rule_results_from_etree(tree)
    results = {RuleResult(r) for r in results}
    return results


def make_parser():
    parser = ap.ArgumentParser()
    parser.add_argument("first")
    parser.add_argument("second")
    return parser


if __name__ == "__main__":
    parser = make_parser()
    args = parser.parse_args()
    first_results = extract_results_from_file(args.first)
    second_results = extract_results_from_file(args.second)
    assert len(first_results) == len(second_results), \
        "There is not the same number of results in the two input files"
    for result in first_results:
        assert result in second_results, \
            "There is a result in the first file that isn't in the second file"
