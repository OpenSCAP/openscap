#!/usr/bin/env bash

. "$builddir/tests/test_common.sh"

set -e -o pipefail

function test_offline_mode_textfilecontent54_symlink {
    local temp_dir result tfc_item
    temp_dir="$(mktemp -d)"

    # authselect makes /etc/pam.d/system-auth a link to
    # /etc/authselect/system-auth. The probe follows the absolute link
    # inside the root.
    mkdir -p "$temp_dir/etc/authselect" "$temp_dir/etc/pam.d"
    echo "auth required pam_faillock.so preauth" > "$temp_dir/etc/authselect/system-auth"
    ln -s /etc/authselect/system-auth "$temp_dir/etc/pam.d/system-auth"
    result="$(mktemp)"

    set_chroot_offline_test_mode "$temp_dir"
    $OSCAP oval eval --results "$result" "$srcdir/test_offline_mode_textfilecontent54_symlink.xml"
    unset_chroot_offline_test_mode

    [[ -s "$result" ]]

    tfc_item='/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item'
    assert_exists 1 "$tfc_item"
    assert_exists 1 "$tfc_item/ind-sys:filepath[text()=\"/etc/pam.d/system-auth\"]"
    assert_exists 1 "$tfc_item/ind-sys:text[text()=\"auth required pam_faillock.so preauth\"]"
    assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'

    rm -rf "$temp_dir"
    rm -f "$result"
    return 0
}

test_init "test_offline_mode_textfilecontent54_symlink.log"

test_run "test_offline_mode_textfilecontent54_symlink" test_offline_mode_textfilecontent54_symlink

test_exit
