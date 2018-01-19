#!/bin/bash
set -o pipefail

. $builddir/tests/test_common.sh

function test_filecontent_non_utf {
    RESULT=$1
    FILE_SUFFIX=${2}

    result=`mktemp`
    output=`mktemp`
    test_dir=$(mktemp -d)
    exit_code=0
    cp ${srcdir}/test_filecontent_non_utf.${FILE_SUFFIX} ${test_dir}/${FILE_SUFFIX}

    cp ${srcdir}/test_filecontent_non_utf.oval.xml ${test_dir}/oval.xml
    sed -i "s:TEST_FILE:${test_dir}/${FILE_SUFFIX}:" ${test_dir}/oval.xml
    $OSCAP oval eval --results $result ${test_dir}/oval.xml &> $output || exit_code=1
    if ! grep ${RESULT} $output; then
        echo "Failed to output \"${RESULT}\", something is wrong"
        cat $output
        exit_code=1
    fi
    rm -f $result $output
    rm -rf ${test_dir}
    return ${exit_code}
}

test_init

test_run "filecontent regex of utf8 file" test_filecontent_non_utf "false" "utf8"
test_run "filecontent regex of iso8859 file" test_filecontent_non_utf "error" "iso8859"

test_exit

