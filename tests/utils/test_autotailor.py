import importlib
import pathlib
import json

import pytest

NS = "http://checklists.nist.gov/xccdf/1.2"


def import_arbitrary_file_as_module(path, module_name):
    spec = importlib.util.spec_from_loader(
        module_name, importlib.machinery.SourceFileLoader(module_name, path))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


autotailor = import_arbitrary_file_as_module("../../../utils/autotailor", "autotailor")


def test_is_valid_xccdf_id():
    assert autotailor.is_valid_xccdf_id(
        "xccdf_com.example.www_profile_abcd")
    assert autotailor.is_valid_xccdf_id(
        "xccdf_com.example.www_rule_selinux_state")
    assert not autotailor.is_valid_xccdf_id("")
    assert not autotailor.is_valid_xccdf_id("foo")
    assert not autotailor.is_valid_xccdf_id(
        "xccdf_com_example_www_rule_selinux_state")
    assert not autotailor.is_valid_xccdf_id("xccdf_rule_selinux_state")
    assert not autotailor.is_valid_xccdf_id("xccdf_com.example.www_rule_")


def test_full_id():
    p = autotailor.Profile()
    assert p._full_rule_id("accounts_tmout") == \
        "xccdf_org.ssgproject.content_rule_accounts_tmout"
    assert p._full_rule_id(
        "xccdf_org.ssgproject.content_rule_accounts_tmout") == \
        "xccdf_org.ssgproject.content_rule_accounts_tmout"
    assert p._full_profile_id("stig") == \
        "xccdf_org.ssgproject.content_profile_stig"
    assert p._full_profile_id(
        "xccdf_org.ssgproject.content_profile_stig") == \
        "xccdf_org.ssgproject.content_profile_stig"
    assert p._full_var_id("var_crypto_policy") == \
        "xccdf_org.ssgproject.content_value_var_crypto_policy"
    assert p._full_var_id(
        "xccdf_org.ssgproject.content_value_var_crypto_policy") == \
        "xccdf_org.ssgproject.content_value_var_crypto_policy"


def test_customized_profile_id():
    p = autotailor.Profile()
    p.extends = "stig"
    assert p.profile_id == "stig_customized"
    p.profile_id = "my_cool_profile"
    assert p.profile_id == "my_cool_profile"


def test_refine_rule():
    p = autotailor.Profile()
    with pytest.raises(ValueError) as e:
        p.refine_rule("selinux_state", "severity", "high")
    assert str(e.value) == "Rule id 'selinux_state' is invalid!"
    with pytest.raises(ValueError) as e:
        p.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout", "foo", "bar")
    assert str(e.value) == "Unsupported refine-rule attribute foo"
    with pytest.raises(ValueError) as e:
        p.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout",
            "role", "mnau")
    assert str(e.value) == (
        "Can't refine role of rule 'xccdf_org.ssgproject.content_rule_accounts"
        "_tmout' to 'mnau'. Allowed role values are: \"full\", \"unscored\", "
        "\"unchecked\".")
    with pytest.raises(ValueError) as e:
        p.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout",
            "severity", "mnau")
    assert str(e.value) == (
        "Can't refine severity of rule 'xccdf_org.ssgproject.content_rule_"
        "accounts_tmout' to 'mnau'. Allowed severity values are: \"unknown\", "
        "\"info\", \"low\", \"medium\", \"high\".")
    fav = "xccdf_org.ssgproject.content_rule_accounts_tmout"
    p.refine_rule(fav, "severity", "high")
    assert p.rule_refinements(fav, "severity") == "high"
    p.refine_rule(fav, "role", "full")
    assert p.rule_refinements(fav, "severity") == "high"
    assert p.rule_refinements(fav, "role") == "full"
    with pytest.raises(ValueError) as e:
        p.refine_rule(fav, "severity", "low")
    assert str(e.value) == (
        "Can't refine severity of rule 'xccdf_org.ssgproject.content_rule_"
        "accounts_tmout' to 'low'. This rule severity is already refined to "
        "'high'.")
    assert p.rule_refinements(fav, "severity") == "high"
    assert p.rule_refinements(fav, "role") == "full"

def test_no_id():
    p = autotailor.Profile()
    profile_dict = None
    file_path = pathlib.Path(__file__).parent.joinpath("custom_no_ids.json")
    with open(file_path) as fp:
        json_data = json.load(fp)
        profile_dict = json_data["profiles"][0]
    with pytest.raises(ValueError) as e:
        p.import_json_tailoring_profile(profile_dict)
    assert str(e.value) == "You must define a base_profile_id or an id"


def test_get_datastream_uri():
    t = autotailor.Tailoring()

    # Test default behavior with absolute path (file:// URI)
    t.original_ds_filename = "/nonexistent/path/test-ds.xml"
    t.use_local_path = False
    uri = t._get_datastream_uri()
    assert uri.startswith("file://")
    assert uri.endswith("/nonexistent/path/test-ds.xml")

    # Test local path mode with absolute path (basename only)
    t.use_local_path = True
    uri = t._get_datastream_uri()
    assert uri == "test-ds.xml"

    # Test local path mode with relative path (preserved as-is)
    t.original_ds_filename = "relative/path/to/ds.xml"
    uri = t._get_datastream_uri()
    assert uri == "relative/path/to/ds.xml"

    # Test default behavior with relative path (file:// URI)
    t.use_local_path = False
    uri = t._get_datastream_uri()
    assert uri.startswith("file://")
    assert "relative/path/to/ds.xml" in uri
