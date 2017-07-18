#!/usr/bin/env bash

set -x

. ../../test_common.sh

function test_api_cvrf_eval {
	./test_api_cvrf --eval $srcdir/cvrf_1.1_template.xml cvrf_1.1_results.xml
}

function test_api_cvrf_export {
    local ret_val=0

    ./test_api_cvrf --export-all $srcdir/cvrf_1.1_template.xml cvrf_1.1_template.out.xml
    if [ $? -eq 0 ] && [ -f cvrf_1.1_template.out.xml ]; then
 	if ! $XMLDIFF $srcdir/cvrf_1.1_template.xml cvrf_1.1_template.out.xml; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi
    return $ret_val
}

test_init "test_api_cvrf.log"
test_run "test_api_cvrf_export" test_api_cvrf_export
test_run "test_api_cvrf_eval" test_api_cvrf_eval
test_exit