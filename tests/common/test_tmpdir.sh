#!/usr/bin/env bash
. "$builddir/tests/test_common.sh"

# This test is a regression test for https://redhat.atlassian.net/browse/RHEL-222370

set -e -o pipefail

function setup_tmpdir {
    local workdir="$1"
    local suffix="$2"
    mkdir "$workdir/tmp dir"
    export TMPDIR="$workdir/tmp dir$suffix"
    cd "$workdir"
}

function check_tmpdir_cleanup {
    # The owning session (or fix executor) must also remove its directory.
    if compgen -G "$TMPDIR/oscap.*" > /dev/null; then
        echo "Temporary directory was not cleaned up under $TMPDIR"
        exit 1
    fi
}

function test_tmpdir_remediation {
    (
        local workdir
        local suffix="$1"
        workdir=$(mktemp -d)
        trap 'rm -rf "$workdir"' EXIT
        setup_tmpdir "$workdir" "$suffix"

        local status=0
        # Export OVAL results to '.' to isolate the fix's temporary directory.
        $OSCAP xccdf eval --remediate --oval-results --results results.xml \
            "$srcdir/test_tmpdir.xccdf.xml" || status=$?
        # The harmless fix leaves the deterministic check unsuccessful.
        [[ "$status" -eq 2 ]]
        local fix_path
        IFS= read -r fix_path < "$TMPDIR/fix-path"
        case "$fix_path" in
            "$TMPDIR"/oscap.*/fix-*) ;;
            *) echo "Fix script was not created under TMPDIR: $fix_path"; exit 1 ;;
        esac
        [[ ! -e "$fix_path" ]]
        # assert_exists reads the result variable from test_common.sh.
        # shellcheck disable=SC2034
        local result="$workdir/results.xml"
        assert_exists 1 '//rule-result/message[contains(text(), "Fix execution completed and returned: 0")]'
        rm -f "$result"

        check_tmpdir_cleanup
    )
}

function test_tmpdir_oval {
    (
        local workdir
        local suffix="$1"
        workdir=$(mktemp -d)
        trap 'rm -rf "$workdir"' EXIT
        setup_tmpdir "$workdir" "$suffix"

        bash "$builddir/run" "$builddir/tests/common/test_tmpdir" \
            oval "$srcdir/test_tmpdir.xccdf.xml"

        check_tmpdir_cleanup
    )
}

function test_tmpdir_datastream {
    (
        local workdir
        local suffix="$1"
        workdir=$(mktemp -d)
        trap 'rm -rf "$workdir"' EXIT
        setup_tmpdir "$workdir" "$suffix"

        bash "$builddir/run" "$builddir/tests/common/test_tmpdir" \
            datastream "$srcdir/test_tmpdir.sds.xml"

        check_tmpdir_cleanup
    )
}

test_init
if [[ -z "${CUSTOM_OSCAP+x}" ]]; then
    for scenario in remediation oval datastream; do
        test_run "TMPDIR: $scenario" "test_tmpdir_$scenario" ""
        test_run "TMPDIR with trailing slash: $scenario" "test_tmpdir_$scenario" "/"
    done
fi
test_exit
