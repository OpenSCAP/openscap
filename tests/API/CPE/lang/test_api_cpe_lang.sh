#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE Lang Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

. $builddir/tests/test_common.sh

# Test cases. 

function test_api_cpe_lang_smoke {
    ./test_api_cpe_lang --smoke-test
}

function test_api_cpe_lang_import {
    ./test_api_cpe_lang --get-all $srcdir/lang.xml "UTF-8"
}

function test_api_cpe_lang_import_damaged {
    ./test_api_cpe_lang --get-all $srcdir/lang-damaged.xml "UTF-8"
    [ "$?" == "1" ]
}

function test_api_cpe_lang_import_key {
    KEY_123="123::Microsoft Windows XP with Adobe Reader.en,:(AND(cpe:/o:microsoft:windows_xp)(cpe:/a:adobe:reader))"
    KEY_456="456::Sun Solaris 5.8 or 5.9 or 5.10.,ľščťžýýáíéúôä.,:(OR(cpe:/o:sun:solaris:5.8)(cpe:/o:sun:solaris:5.9)(cpe:/o:sun:solaris:5.10))"
    KEY_789="789:Foolish Remark:Microsoft Windows XP with Office 2003 or 2007.,:(AND(cpe:/o:microsoft:windows_xp)(OR(cpe:/a:microsoft:office:2003)(cpe:/a:microsoft:office:2007)))"
    
    echo "" > get-all.out
    echo $KEY_123 >> get-all.out
    echo $KEY_456 >> get-all.out
    echo $KEY_789 >> get-all.out

    for K in "$KEY_123" "$KEY_456" "$KEY_789"; do
	N=`echo $K | awk -F':' '{print $1}'`
	./test_api_cpe_lang --get-key $srcdir/lang.xml "UTF-8" $N > get-key.out
	if [ $? -eq 0 ]; then
	    if [ "`cat get-key.out`X" != "${K}X" ]; then
		echo "`cat get-key.out`X != ${K}X"
		return 1
	    fi
	else 
	    echo "Cannot get key $K from lang.xml!"
	    return 1
	fi
    done
    return 0
}

function test_api_cpe_lang_export_empty {
    local ret_val=0;

    echo '<?xml version="1.0"  encoding="UTF-8"?>'       >  export.xml.out.1.0
    echo '<Foo:platform-specification xmlns:Foo="Bar"/>' >> export.xml.out.0

    ./test_api_cpe_lang --set-new export.xml.out.1 "UTF-8" "Foo" "Bar"
    if [ $? -eq 0 ] && [ -f export.xml.out.1 ]; then
	if ! $XMLDIFF export.xml.out.0 export.xml.out.1; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi
    return $ret_val
}

function test_cpelang_export_new_model {
    local ret_val=0;

    echo '<?xml version="1.0" encoding="UTF-8"?>'                             >  export.xml.out.0
    echo '<platform-specification xmlns="http://cpe.mitre.org/language/2.0">' >> export.xml.out.0
    echo '<platform id="1"/>'                                                 >> export.xml.out.0
    echo '<platform id="10"/>'                                                >> export.xml.out.0
    echo '</platform-specification>'                                          >> export.xml.out.0

    ./test_api_cpe_lang --set-new export.xml.out.1 "UTF-8" "Foo" "Bar" 1 10
    if [ $? -eq 0 ] && [ -f export.xml.out.1 ]; then
	if ! $XMLDIFF export.xml.out.0 export.xml.out.1; then
	    echo "Exported file differs from what is expected!"	    
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi    
    return $ret_val
}

function test_api_cpe_lang_export_encoding {
    local ret_val=0;

    echo '<?xml version="1.0"?>' > export.xml.out.0
    echo '<Foo:platform-specification xmlns:Foo="Bar"/>' >> export.xml.out.0

    ./test_api_cpe_lang --set-new export.xml.out.1 "UnknownEncoding" "Foo" "Bar"
    if [ $? -eq 0 ] && [ -f export.xml.out.1 ]; then
	if ! $XMLDIFF export.xml.out.0 export.xml.out.1; then
	    echo "Exported file differs from what is expected!"	    
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi    
    return $ret_val
}

function test_api_cpe_lang_export_namespace {
    local ret_val=0;

    echo '<?xml version="1.0" encoding="UTF-8"?>' > export.xml.out.0
    echo '<foo:platform-specification xmlns:foo="Bar" xmlns:bla="Bla"/>' >> export.xml.out.0

    ./test_api_cpe_lang --export-all export.xml.out.0 "UTF-8" export.xml.out.1 "UTF-8"
    if [ $? -eq 0 ] && [ -f export.xml.out.1 ]; then
	if ! $XMLDIFF export.xml.out.0 export.xml.out.1; then
	    echo "Exported file differs from what is expected!"	    
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi    
    return $ret_val
}

function test_api_cpe_lang_match {
    echo '<?xml version="1.0"  encoding="UTF-8"?>' > export.xml.out.0
    echo '<cpe:platform-specification xmlns:cpe="http://cpe.mitre.org/language/2.0">' >> export.xml.out.0
    echo '<cpe:platform id="123">' >> export.xml.out.0
    echo 'e<cpe:title xml:lang="en">Microsoft Windows XP with Adobe Reader</cpe:title>' >> export.xml.out.0
    echo '<cpe:logical-test operator="AND" negate="FALSE">' >> export.xml.out.0
    echo '<cpe:fact-ref name="cpe:/o:microsoft:windows_xp" />' >> export.xml.out.0
    echo '<cpe:fact-ref name="cpe:/a:adobe:reader" />' >> export.xml.out.0
    echo '</cpe:logical-test>' >> export.xml.out.0
    echo '</cpe:platform>' >> export.xml.out.0
    echo '</cpe:platform-specification>' >> export.xml.out.0

    ./test_api_cpe_lang --match-cpe export.xml.out.0 "UTF-8" "cpe:/a:adobe:reader" "cpe:/o:microsoft:windows_xp"
}

# Testing.

test_init

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_cpe_lang_smoke" test_api_cpe_lang_smoke
    test_run "test_api_cpe_lang_import" test_api_cpe_lang_import
    test_run "test_api_cpe_lang_import_damaged" test_api_cpe_lang_import_damaged    
    test_run "test_api_cpe_lang_import_key" test_api_cpe_lang_import_key
    # test_run "test_api_cpe_lang_export_empty" test_api_cpe_lang_export_empty
    # test_run "test_api_cpe_lang_export_ecoding" test_api_cpe_lang_export_encoding
    # test_run "test_api_cpe_lang_export_namespace" test_api_cpe_lang_export_namespace
    test_run "test_api_cpe_lang_match" test_api_cpe_lang_match
fi

test_exit 
