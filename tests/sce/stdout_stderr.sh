#!/bin/bash

echo "test_out" > /dev/stdout
echo "test_err" > /dev/stderr
exit $XCCDF_RESULT_PASS
