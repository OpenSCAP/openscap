import importlib


def import_arbitrary_file_as_module(path, module_name):
    spec = importlib.util.spec_from_loader(
        module_name, importlib.machinery.SourceFileLoader(module_name, path))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


autotailor = import_arbitrary_file_as_module("utils/autotailor", "autotailor")


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
