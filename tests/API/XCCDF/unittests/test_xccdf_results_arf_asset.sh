#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e
set -o pipefail

function test_xccdf_results_arf_asset {
    local DS="${srcdir}/test_xccdf_results_arf_no_oval.xccdf.xml"
    local result="results.xml"
    local stderr="error.log"

    [ -f $result ] && rm -f $result

    $OSCAP xccdf eval --results-arf $result $DS 2> $stderr

    [ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

    [ -f $result ]

    local asset='/arf:asset-report-collection/arf:assets/arf:asset[@id="asset0"]/ai:computing-device'

    local hostname=`hostname`
    echo $asset'/ai:hostname[text()="'$hostname'"]'
    assert_exists 1 $asset'/ai:hostname[text()="'$hostname'"]'

    local fqdn=`hostname --fqdn`
    echo $asset'/ai:fqdn[text()="'$fqdn'"]'
    assert_exists 1 $asset'/ai:fqdn[text()="'$fqdn'"]'

    local macs=`ifconfig -a | grep ether | uniq | awk -F ' ' '{print toupper($2);}'`
    for mac in $macs; do
        echo $asset'/ai:connections/ai:connection/ai:mac-address[text()="'${mac}'"]'
        assert_exists 1 $asset'/ai:connections/ai:connection/ai:mac-address[text()="'$mac'"]'
    done

    local ip4s=`ifconfig -a | grep 'inet ' | uniq | awk -F ' ' '{print $2;}'`
    for ip in $ip4s; do
        echo $asset'/ai:connections/ai:connection/ai:mac-address[text()="'$ip'"]'
        assert_exists 1 $asset'/ai:connections/ai:connection/ai:ip-address/ai:ip-v4[text()="'$ip'"]'
    done

    rm $result
}

test_xccdf_results_arf_asset