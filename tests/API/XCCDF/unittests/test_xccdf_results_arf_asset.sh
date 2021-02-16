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
    assert_exists 1 $asset'/ai:hostname[text()="'$hostname'"]'

    # We are not using --fqdn here because oscap in fact fills ai:fqdn from
    # the XCCDF's <target> entity, which is a plain hostname
    local fqdn=`hostname`
    assert_exists 1 $asset'/ai:fqdn[text()="'$fqdn'"]'

    # TODO: oscap does not collect MACs from all interfaces that are UP
    #macs=`ifconfig | grep ether | awk -F ' ' '{print toupper($2);}'`
    #for mac in "$macs"; do
    #    assert_exists 1 $asset'/ai:connections/ai:connection/ai:mac-address[text()="'$mac'"]'
    #done

    assert_exists 1 $asset'/ai:connections/ai:connection/ai:ip-address/ai:ip-v4[text()="127.0.0.1"]'

    rm $result
}

test_xccdf_results_arf_asset