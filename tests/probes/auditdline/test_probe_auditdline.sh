#!/usr/bin/env bash

# Copyright 2021 Sopra Steria 
# All Rights Reserved.

set -o pipefail


. $builddir/tests/test_common.sh

function audit_rules_query {

    if [ "$EUID" -ne 0 ]
        then echo "Tests for auditdline need to be run as root because of the use of auditctl"
        exit 255
    fi


    require "auditctl" || exit 255

    current_audit_rules=$(auditctl -l)

}

function test_probe_auditdline {

    local RF=`mktemp`
    export OSCAP_SCHEMA_PATH=/home/disco-dev/Sopra/MCO/openscap/schemas

    probecheck "auditdline" || return 255

    audit_rules_query

    # The use of --skip-valid is temporary until auditd is present in the unix schema
    $OSCAP oval eval --results $RF --skip-valid $srcdir/auditdline-test.xml

    oscap_result=$($XPATH $RF 'count(//auditdline_item/auditline)')

    current_audit_rules_count=$(echo "$current_audit_rules" | wc -l )

    if [ $current_audit_rules_count -eq $oscap_result ]; then
        return 0
    else
        return 1
    fi
}

test_init

test_run "auditdline probe test" test_probe_auditdline \
    test_probe_auditdline

#test_exit
