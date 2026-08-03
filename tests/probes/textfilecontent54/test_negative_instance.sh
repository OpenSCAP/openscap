#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_negative_instance {

    probecheck "textfilecontent54" || return 255
	
    local ret_val=0;
    local DF="${srcdir}/test_negative_instance.xml"
    local RF="$(mktemp results.XXXXXXX.xml)"
    
    [ -f $RF ] && rm -f $RF

    local FILE_A="/tmp/test_negative_instance.tmp_file"

    touch "$FILE_A"

    echo "valid_key = valid_value" > "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"

    $OSCAP oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "tst" $DF $RF 13 && verify_results "def" $DF $RF 1
	ret_val=$?
    else 
	ret_val=1
    fi

    rm -f $FILE_A $FILE_B $FILE_C

    return $ret_val
}

test_negative_instance
