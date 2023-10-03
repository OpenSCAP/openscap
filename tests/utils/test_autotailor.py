import importlib
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
    t = autotailor.Tailoring()
    assert t._full_rule_id("accounts_tmout") == \
        "xccdf_org.ssgproject.content_rule_accounts_tmout"
    assert t._full_rule_id(
        "xccdf_org.ssgproject.content_rule_accounts_tmout") == \
        "xccdf_org.ssgproject.content_rule_accounts_tmout"
    assert t._full_profile_id("stig") == \
        "xccdf_org.ssgproject.content_profile_stig"
    assert t._full_profile_id(
        "xccdf_org.ssgproject.content_profile_stig") == \
        "xccdf_org.ssgproject.content_profile_stig"
    assert t._full_var_id("var_crypto_policy") == \
        "xccdf_org.ssgproject.content_value_var_crypto_policy"
    assert t._full_var_id(
        "xccdf_org.ssgproject.content_value_var_crypto_policy") == \
        "xccdf_org.ssgproject.content_value_var_crypto_policy"


def test_customized_profile_id():
    t = autotailor.Tailoring()
    t.extends = "stig"
    assert t.profile_id == "stig_customized"
    t.profile_id = "my_cool_profile"
    assert t.profile_id == "my_cool_profile"


def test_refine_rule():
    t = autotailor.Tailoring()
    with pytest.raises(ValueError) as e:
        t.refine_rule("selinux_state", "severity", "high")
    assert str(e.value) == "Rule id 'selinux_state' is invalid!"
    with pytest.raises(ValueError) as e:
        t.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout", "foo", "bar")
    assert str(e.value) == "Unsupported refine-rule attribute foo"
    with pytest.raises(ValueError) as e:
        t.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout",
            "role", "mnau")
    assert str(e.value) == (
        "Can't refine role of rule 'xccdf_org.ssgproject.content_rule_accounts"
        "_tmout' to 'mnau'. Allowed role values are: \"full\", \"unscored\", "
        "\"unchecked\".")
    with pytest.raises(ValueError) as e:
        t.refine_rule(
            "xccdf_org.ssgproject.content_rule_accounts_tmout",
            "severity", "mnau")
    assert str(e.value) == (
        "Can't refine severity of rule 'xccdf_org.ssgproject.content_rule_"
        "accounts_tmout' to 'mnau'. Allowed severity values are: \"unknown\", "
        "\"info\", \"low\", \"medium\", \"high\".")
    fav = "xccdf_org.ssgproject.content_rule_accounts_tmout"
    t.refine_rule(fav, "severity", "high")
    assert t.rule_refinements(fav, "severity") == "high"
    t.refine_rule(fav, "role", "full")
    assert t.rule_refinements(fav, "severity") == "high"
    assert t.rule_refinements(fav, "role") == "full"
    with pytest.raises(ValueError) as e:
        t.refine_rule(fav, "severity", "low")
    assert str(e.value) == (
        "Can't refine severity of rule 'xccdf_org.ssgproject.content_rule_"
        "accounts_tmout' to 'low'. This rule severity is already refined to "
        "'high'.")
    assert t.rule_refinements(fav, "severity") == "high"
    assert t.rule_refinements(fav, "role") == "full"
