#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail


kickstart=$(mktemp)
stderr=$(mktemp)

$OSCAP xccdf generate fix --fix-type kickstart --output "$kickstart" --result-id xccdf_org.open-scap_testresult_xccdf_org.ssgproject.content_profile_ospp "$srcdir/test_remediation_kickstart.ds.xml" 2> "$stderr" || ret=$?

[ $ret = 1 ]
grep -q "It isn't possible to generate results-oriented Kickstarts." $stderr

rm -rf "$kickstart"
rm -rf "$stderr"


kickstart=$(mktemp)
stderr=$(mktemp)

$OSCAP xccdf generate fix --fix-type kickstart --output "$kickstart" --profile common "$srcdir/test_remediation_kickstart.ds.xml"

grep -q '# Kickstart for Common hardening profile' "$kickstart"
grep -q 'services --disabled=telnet --enabled=auditd,rsyslog,sshd' "$kickstart"
grep -q 'logvol /var/tmp --fstype=xfs --name=vartmp --vgname=VolGroup --size=1024' "$kickstart"
grep -q 'mkdir /etc/scap' "$kickstart"
grep -q '\-usbguard' "$kickstart"

rm -rf "$kickstart"
rm -rf "$stderr"
