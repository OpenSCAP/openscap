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


def test_datastream_validator():
    """Test that DataStreamValidator properly validates IDs."""
    ds_path = pathlib.Path(__file__).parent.joinpath("data_stream.xml")
    validator = autotailor.DataStreamValidator(str(ds_path))

    # Test valid profile validation
    validator.validate_profile("xccdf_com.example.www_profile_P1")

    # Test valid value validation
    validator.validate_value("xccdf_com.example.www_value_V1")
    validator.validate_value("xccdf_com.example.www_value_V2")

    # Test valid rule validation
    validator.validate_rule("xccdf_com.example.www_rule_R1")
    validator.validate_rule("xccdf_com.example.www_rule_R2")
    validator.validate_rule("xccdf_com.example.www_rule_R3")
    validator.validate_rule("xccdf_com.example.www_rule_R4")

    # Test valid group validation
    validator.validate_group("xccdf_com.example.www_group_G34")

    # Test invalid profile
    with pytest.raises(ValueError) as e:
        validator.validate_profile("xccdf_com.example.www_profile_INVALID")
    assert "Profile ID 'xccdf_com.example.www_profile_INVALID' does not exist" in str(e.value)

    # Test invalid value with suggestion
    with pytest.raises(ValueError) as e:
        validator.validate_value("xccdf_com.example.www_value_V3")
    assert "Value ID 'xccdf_com.example.www_value_V3' does not exist" in str(e.value)

    # Test invalid rule with suggestion
    with pytest.raises(ValueError) as e:
        validator.validate_rule("xccdf_com.example.www_rule_R5")
    assert "Rule ID 'xccdf_com.example.www_rule_R5' does not exist" in str(e.value)

    # Test invalid group
    with pytest.raises(ValueError) as e:
        validator.validate_group("xccdf_com.example.www_group_INVALID")
    assert "Group ID 'xccdf_com.example.www_group_INVALID' does not exist" in str(e.value)


def test_profile_with_validator():
    """Test that Profile uses validator to check IDs."""
    ds_path = pathlib.Path(__file__).parent.joinpath("data_stream.xml")
    validator = autotailor.DataStreamValidator(str(ds_path))

    p = autotailor.Profile(validator=validator)
    p.reverse_dns = "com.example.www"

    # Test valid variable change works
    p.add_value_change("V1", "30")

    # Test invalid variable name fails
    with pytest.raises(ValueError) as e:
        p.add_value_change("INVALID_VAR", "test")
    assert "Value ID 'xccdf_com.example.www_value_INVALID_VAR' does not exist" in str(e.value)

    # Test valid rule selection works
    p.select_rule("R1")

    # Test invalid rule selection fails
    with pytest.raises(ValueError) as e:
        p.select_rule("INVALID_RULE")
    assert "Rule ID 'xccdf_com.example.www_rule_INVALID_RULE' does not exist" in str(e.value)

    # Test valid base profile validation
    p.validate_base_profile("P1")

    # Test invalid base profile fails
    with pytest.raises(ValueError) as e:
        p.validate_base_profile("INVALID_PROFILE")
    assert "Profile ID 'xccdf_com.example.www_profile_INVALID_PROFILE' does not exist" in str(e.value)


def test_validator_suggestions():
    """Test that validator provides helpful suggestions for typos."""
    ds_path = pathlib.Path(__file__).parent.joinpath("data_stream.xml")
    validator = autotailor.DataStreamValidator(str(ds_path))

    # Test suggestion for value with typo (V11 instead of V1)
    with pytest.raises(ValueError) as e:
        validator.validate_value("xccdf_com.example.www_value_V11")
    error_msg = str(e.value)
    assert "Did you mean one of these?" in error_msg
    assert "xccdf_com.example.www_value_V1" in error_msg

    # Test suggestion for rule with typo (R11 instead of R1)
    with pytest.raises(ValueError) as e:
        validator.validate_rule("xccdf_com.example.www_rule_R11")
    error_msg = str(e.value)
    assert "Did you mean one of these?" in error_msg
    assert "xccdf_com.example.www_rule_R1" in error_msg


def test_validate_selector():
    """Test that validate_selector rejects selectors not present in the data stream."""
    ds_path = pathlib.Path(__file__).parent.joinpath("data_stream.xml")
    validator = autotailor.DataStreamValidator(str(ds_path))

    # V1 has selector "thirty"; V2 has "some" and "other"
    validator.validate_selector("xccdf_com.example.www_value_V1", "thirty")
    validator.validate_selector("xccdf_com.example.www_value_V2", "some")
    validator.validate_selector("xccdf_com.example.www_value_V2", "other")

    # Invalid selector for V1
    with pytest.raises(ValueError) as e:
        validator.validate_selector("xccdf_com.example.www_value_V1", "invalid")
    error_msg = str(e.value)
    assert "Selector 'invalid' does not exist for Value 'xccdf_com.example.www_value_V1'" in error_msg
    assert "thirty" in error_msg

    # Invalid selector for V2 with a close-enough typo triggers a suggestion
    with pytest.raises(ValueError) as e:
        validator.validate_selector("xccdf_com.example.www_value_V2", "ther")
    error_msg = str(e.value)
    assert "Selector 'ther' does not exist for Value 'xccdf_com.example.www_value_V2'" in error_msg
    assert "other" in error_msg


def test_profile_selector_validation():
    """Test that Profile validates selectors via -V/--var-select through refine_value."""
    ds_path = pathlib.Path(__file__).parent.joinpath("data_stream.xml")
    validator = autotailor.DataStreamValidator(str(ds_path))

    p = autotailor.Profile(validator=validator)
    p.reverse_dns = "com.example.www"

    # Valid selector passes
    p.change_selectors(["V1=thirty"])
    p.change_selectors(["V2=some"])

    # Invalid selector raises
    with pytest.raises(ValueError) as e:
        p.change_selectors(["V1=invalid"])
    assert "Selector 'invalid' does not exist for Value 'xccdf_com.example.www_value_V1'" in str(e.value)

    # Invalid value ID still raises before reaching selector check
    with pytest.raises(ValueError) as e:
        p.change_selectors(["NONEXISTENT=thirty"])
    assert "Value ID 'xccdf_com.example.www_value_NONEXISTENT' does not exist" in str(e.value)
