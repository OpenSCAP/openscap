#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CvE Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#

. ${srcdir}/test_common.sh

# Setup.
function test_cve_setup {
    local ret_val=0
    
cat > cve_export.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<nvd>
    <entry id="CVE-01"/>
    <entry id="CVE-02"/>
    <entry id="CVE-03"/>
    <entry id="CVE-04"/>
    <entry id="CVE-05"/>
</nvd>
EOF

    ./test_cve --test-export-all cve_export.xml "UTF-8" cve_export.xml "UTF-8"
    ret_val=$?

    return $ret_val
}

# Test Cases.

function test_cve_tc00 {
    local ret_val=0

    ./test_cve --sanity-check 
    ret_val=$?

    return $ret_val
}

function test_cve_tc01 {

    local ret_val=0

    ./test_cve --test-export-all cve_export.xml "UTF-8" cve_export.xml.out.1 "UTF-8"
    ret_val=$?
    [ ! -e cve_export.xml.out.1 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp ${srcdir}/cve_export.xml cve_export.xml.out.1
        ret_val=$?
    fi

    return $ret_val
}

function test_cve_tc02 {

    local ret_val=0

    ./test_cve --add-entries cve_export.xml.out.2 "UTF-8" "CVE-01" "CVE-02" "CVE-03" "CVE-04" "CVE-05"
    ret_val=$?
    [ ! -e cve_export.xml.out.1 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp ${srcdir}/cve_export.xml cve_export.xml.out.2
        ret_val=$?
    fi

    return $ret_val
}
function test_cve_tc03 {

    local ret_val=0

    ./test_cve --test-export-all CVE/nvdcve-2.0-recent.xml "UTF-8" cve_export.xml.out.3 "UTF-8"
	xml_cmp ${srcdir}/CVE/nvdcve-2.0-recent.xml cve_export.xml.out.3
    ret_val=$?

    return $ret_val
}

function test_cve_tc04 {
cat > cve_export.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<nvd xmlns:cvss="cvss" xmlns:vuln="vuln">
    <entry id="CVE-01">
        <vuln:cvss>
            <cvss:base_metrics>
                <cvss:score>4.3</cvss:score>
                <cvss:access-vector>NETWORK</cvss:access-vector>
                <cvss:access-complexity>MEDIUM</cvss:access-complexity>
                <cvss:authentication>NONE</cvss:authentication>
                <cvss:confidentiality-impact>NONE</cvss:confidentiality-impact>
                <cvss:integrity-impact>PARTIAL</cvss:integrity-impact>
                <cvss:availability-impact>NONE</cvss:availability-impact>
                <cvss:source>http://nvd.nist.gov</cvss:source>
                <cvss:generated-on-datetime>2009-03-10T13:30:00.000-04:00</cvss:generated-on-datetime>
            </cvss:base_metrics>
        </vuln:cvss>
    </entry>
</nvd>
EOF

    local ret_val=0

    ./test_cve --test-cvss cve_export.xml "UTF-8" 
    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_cve_cleanup {
    local ret_val=0

    rm cve_export.xml.out.1 cve_export.xml.out.2 cve_export.xml.out.3 cve_export.xml
    ret_val=$?
    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=test_cve.log

exec 2>$log

#---- function ------#-------------------------- reporting -----------------------#--------------------------#
test_cve_setup    ; ret_val=$? ; report_result "test_CVE_setup"      $ret_val  ; result=$[$result+$ret_val]
test_cve_tc00     ; ret_val=$? ; report_result 'Test_CVE_sanity'     $ret_val  ; result=$[$result+$ret_val]
test_cve_tc01     ; ret_val=$? ; report_result 'Test_CVE_export'     $ret_val  ; result=$[$result+$ret_val]
test_cve_tc02     ; ret_val=$? ; report_result 'Test_CVE_entries'    $ret_val  ; result=$[$result+$ret_val]
test_cve_tc03     ; ret_val=$? ; report_result 'Test_CVE_export_all' $ret_val  ; result=$[$result+$ret_val]
test_cve_tc04     ; ret_val=$? ; report_result 'Test_CVE_get_CVSS'   $ret_val  ; result=$[$result+$ret_val]
test_cve_cleanup  ; ret_val=$? ; report_result "test_CVE_cleanup" $ret_val  ; result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${srcdir}/${log}"

exit $result
