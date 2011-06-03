#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>


. ../test_common.sh

# Test Cases.

function test_mitre {

    if [ ! -d /tmp/ValidationSupportFiles ]; then
        eval "which unzip > /dev/null 2>&1"    
        if [ ! $? -eq 0 ]; then	
            echo -e "No unzip found in $PATH!\n" 
	    return 255; # Test is not applicable.
        fi
        /usr/bin/unzip -u ValidationSupportFiles.zip -d /tmp
    fi

    local ret_val=0;
    local DEFFILE=$1
    local RESFILE="$DEFFILE".results

    [ -f $RESFILE ] && rm -f $RESFILE
    ../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE
    ret_val=$?
    
    if [ $ret_val -eq 0 ]; then
        return 0;
    elif [ $ret_val -eq 2 ]; then
	return 1;
    elif [ $ret_val -eq 1 ]; then
        return 255;
    else
	return "$ret_val"
    fi
}

# Testing.
test_init "test_mitre.log"

# INDEPENDET
#  our bug in internal probe
#test_run "ind-def_environmentvariable_test.xml" test_mitre ind-def_environmentvariable_test.xml
test_run "ind-def_family_test.xml" test_mitre ind-def_family_test.xml
#  their bug in: oval:org.mitre.oval.test:tst:102
#  unsupport recursion up: oval:org.mitre.oval.test:obj:562
#test_run "ind-def_filehash_test.xml" test_mitre ind-def_filehash_test.xml
#  unsupported recursion up: oval:org.mitre.oval.test:obj:577
#test_run "ind-def_textfilecontent54_test.xml" test_mitre ind-def_textfilecontent54_test.xml
#  unsupported recursion up: oval:org.mitre.oval.test:obj:800
#test_run "ind-def_textfilecontent_test.xml" test_mitre ind-def_textfilecontent_test.xml
test_run "ind-def_unknown_test.xml" test_mitre ind-def_unknown_test.xml
test_run "ind-def_variable_test.xml" test_mitre ind-def_variable_test.xml
#  unsupported recursion up: oval:org.mitre.oval.test:obj:701
#test_run "ind-def_xmlfilecontent_test.xml" test_mitre ind-def_xmlfilecontent_test.xml

# LINUX
#  do not pass content validation
#test_run "linux-def_inetlisteningservers_test.xml" test_mitre linux-def_inetlisteningservers_test.xml
#test_run "linux-def_partition_test.xml" test_mitre linux-def_partition_test.xml

test_exit

