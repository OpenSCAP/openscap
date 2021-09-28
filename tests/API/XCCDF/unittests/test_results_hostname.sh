#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

result=$(mktemp)
tmpdir=$(mktemp -d)

export OSCAP_PROBE_ROOT="$tmpdir"

mkdir -p "$tmpdir/etc"
echo "hostname_defined_in_etc_hostname" > "$tmpdir/etc/hostname"
$OSCAP xccdf eval --results "$result" "$srcdir/test_single_rule.ds.xml" || ret=$?
assert_exists 1 '/Benchmark/TestResult/target[text()="hostname_defined_in_etc_hostname"]'
assert_exists 0 '/Benchmark/TestResult/target[text()="hostname_defined_in_proc_sys_kernel"]'

rm -rf "$tmpdir/etc/hostname"
mkdir -p "$tmpdir/proc/sys/kernel/"
echo "hostname_defined_in_proc_sys_kernel" > "$tmpdir/proc/sys/kernel/hostname"
$OSCAP xccdf eval --results "$result" "$srcdir/test_single_rule.ds.xml" || ret=$?
assert_exists 0 '/Benchmark/TestResult/target[text()="hostname_defined_in_etc_hostname"]'
assert_exists 1 '/Benchmark/TestResult/target[text()="hostname_defined_in_proc_sys_kernel"]'

rm -f "$result"
rm -rf "$tmpdir"
