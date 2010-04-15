#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE Lang Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

. ${srcdir}/test_common.sh

# Setup.
function test_cpelang_setup {           
    local ret_val=0
    
    KEY_123="123::Microsoft Windows XP with Adobe Reader.en,:(AND(cpe:/o:microsoft:windows_xp)(cpe:/a:adobe:reader))"
    KEY_456="456::Sun Solaris 5.8 or 5.9 or 5.10.,ľščťžýýáíéúôä.,:(OR(cpe:/o:sun:solaris:5.8)(cpe:/o:sun:solaris:5.9)(cpe:/o:sun:solaris:5.10))"
    KEY_789="789:Foolish Remark:Microsoft Windows XP with Office 2003 or 2007.,:(AND(cpe:/o:microsoft:windows_xp)(OR(cpe:/a:microsoft:office:2003)(cpe:/a:microsoft:office:2007)))"

    echo "http://cpe.mitre.org/language/2.0:cpe" > get-all
    echo $KEY_123 >> get-all
    echo $KEY_456 >> get-all
    echo $KEY_789 >> get-all

    return $ret_val
}

# Test Cases.

function test_cpelang_smoke {
    local ret_val=0;

    ./test_cpelang --smoke-test
    ret_val=$?

    return $ret_val
}
   
function test_cpelang_import {
    local ret_val=0;

    ./test_cpelang --get-all ${srcdir}/CPE/lang.xml "UTF-8" > get-all.out
    ret_val=$?
    if [ $ret_val -eq 0 ]; then
	cmp get-all get-all.out >&2
	ret_val=$?
    fi    

    return $ret_val
}

function test_cpelang_import_damaged {
    local ret_val=0;

    ./test_cpelang --get-all ${srcdir}/CPE/lang-damaged.xml "UTF-8" 

    case $? in
	0) ret_val=1 ;;   # success
	139) ret_val=1 ;; # segfault
    esac

    return $ret_val
}

function test_cpelang_import_key {
    local ret_val=0;

    ./test_cpelang --get-key ${srcdir}/CPE/lang.xml "UTF-8" "123" > get-key.out
    ret_val=$[$ret_val + $?]
    if [ $ret_val -eq 0 ]; then
	[ "`cat get-key.out`X" = "${KEY_123}X" ] || ret_val=$[$ret_val + $?]
    fi

    ./test_cpelang --get-key ${srcdir}/CPE/lang.xml "UTF-8" "456" > get-key.out
    ret_val=$[$ret_val + $?]
    if [ $ret_val -eq 0 ]; then
	[ "`cat get-key.out`X" = "${KEY_456}X" ] || ret_val=$[$ret_val + $?]
    fi

    ./test_cpelang --get-key ${srcdir}/CPE/lang.xml "UTF-8" "789" > get-key.out
    ret_val=$[$ret_val + $?]
    if [ $ret_val -eq 0 ]; then
	[ "`cat get-key.out`X" = "${KEY_789}X" ] || ret_val=$[$ret_val + $?]
    fi

    return $ret_val
}

function test_cpelang_export_new_empty_model {
    local ret_val=0;

cat > export.xml <<EOF
<?xml version="1.0"  encoding="UTF-8"?>
<Foo:platform-specification xmlns:Foo="Bar"/>
EOF

    ./test_cpelang --set-new export.xml.out.0 "UTF-8" "Foo" "Bar"
    ret_val=$?
    
    [ ! -e export.xml.out.0 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp export.xml export.xml.out.0
	ret_val=$?
    fi    

    return $ret_val
}

# Function tests adding platform elements and xml namespace to Foo 
# in platform-specification element.
function test_cpelang_export_new_model {
    local ret_val=0;

cat > export.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<Foo:platform-specification xmlns:Foo="Bar">
<platform id="1"/>
<platform id="2"/>
<platform id="3"/>
<platform id="4"/>
<platform id="5"/>
<platform id="6"/>
<platform id="7"/>
<platform id="8"/>
<platform id="9"/>
<platform id="10"/>
</Foo:platform-specification>
EOF

    ./test_cpelang --set-new export.xml.out.1 "UTF-8" "Foo" "Bar" 1 2 3 4 5 6 7 8 9 10
    ret_val=$?
    
    [ ! -e export.xml.out.1 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp export.xml export.xml.out.1
	ret_val=$?
    fi    

    return $ret_val
}

# Function tests default behaviour of setting an unknown
# encoding. Library should set default behaviour, that is 
# not setting any encoding
function test_cpelang_export_new_encoding {
    local ret_val=0;

cat > export.xml <<EOF
<?xml version="1.0"?>
<Foo:platform-specification xmlns:Foo="Bar"/>
EOF

    ./test_cpelang --set-new export.xml.out.2 "UnknownEncoding" "Foo" "Bar"
    ret_val=$?
    
    [ ! -e export.xml.out.2 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp export.xml export.xml.out.2
	ret_val=$?
    fi    

    return $ret_val
}

# Function tests xml namespace
function test_cpelang_export_new_namespace {
    local ret_val=0;

cat > export.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<foo:platform-specification xmlns:foo="Bar" xmlns:bla="Bla"/>
EOF

    ./test_cpelang --export-all export.xml "UTF-8" export.xml.out.3 "UTF-8"
    ret_val=$?

    [ ! -e export.xml.out.3 ] && [ $ret_val -eq 0 ] && ret_val=1

    if [ $ret_val -eq 0 ]; then
	xml_cmp export.xml export.xml.out.3
	ret_val=$?
    fi

    return $ret_val
}

# Function for testing CPE match system
function test_cpelang_match_cpe {
    local ret_val=0;

cat > export.xml <<EOF
<?xml version="1.0"  encoding="UTF-8"?>
<cpe:platform-specification xmlns:cpe="http://cpe.mitre.org/language/2.0">
	<cpe:platform id="123">
		<cpe:title xml:lang="en">Microsoft Windows XP with Adobe Reader</cpe:title>
		<cpe:logical-test operator="AND" negate="FALSE">
			<cpe:fact-ref name="cpe:/o:microsoft:windows_xp" />
			<cpe:fact-ref name="cpe:/a:adobe:reader" />
		</cpe:logical-test>
	</cpe:platform>
</cpe:platform-specification>
EOF


    ./test_cpelang --match-cpe export.xml "UTF-8" "cpe:/a:adobe:reader" "cpe:/o:microsoft:windows_xp"
    ret_val=$?

    return $ret_val
}

# Cleanup.
function test_cpelang_cleanup {
    local ret_val=0;

    rm -f export.xml \
          export.xml.out.0 \
          export.xml.out.1 \
          export.xml.out.2 \
          export.xml.out.3 \
    	  diff.out get-all \
    	  get-all \
    	  get-all.out \
    	  get-key.out

    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=test_cpelang.log

exec 2>$log

test_cpelang_setup
ret_val=$? 
report_result "test_cpelang_setup" $ret_val 
result=$[$result+$ret_val]

test_cpelang_smoke
ret_val=$? 
report_result "test_cpelang_smoke" $ret_val 
result=$[$result+$ret_val]   

test_cpelang_import
ret_val=$? 
report_result "test_cpelang_import" $ret_val 
result=$[$result+$ret_val]   

test_cpelang_import_damaged
ret_val=$? 
report_result "test_cpelang_import_damaged" $ret_val
result=$[$result+$ret_val]

test_cpelang_import_key
ret_val=$?
report_result "test_cpelang_import_key" $ret_val 
result=$[$result+$ret_val]   

test_cpelang_export_new_empty_model
ret_val=$? 
report_result "test_cpelang_export_new_empty_model" $ret_val 
result=$[$result+$ret_val]

test_cpelang_export_new_model
ret_val=$? 
report_result "test_cpelang_export_new_model" $ret_val
result=$[$result+$ret_val]

test_cpelang_export_new_encoding
ret_val=$? 
report_result "test_cpelang_export_new_encoding" $ret_val
result=$[$result+$ret_val]

test_cpelang_export_new_namespace
ret_val=$? 
report_result "test_cpelang_export_new_namespace" $ret_val
result=$[$result+$ret_val]

test_cpelang_match_cpe
ret_val=$? 
report_result "test_cpelang_match_cpe" $ret_val
result=$[$result+$ret_val]

test_cpelang_cleanup 
ret_val=$? 
report_result "test_cpelang_cleanup" $ret_val
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result
