#!/bin/bash
lines=${XCCDF_VALUE_LINES}

for i in `seq -w 1 ${lines}`; do
    echo "0${i}0"
done

for i in `seq -w 1 ${lines}`; do
    echo "1${i}0" >&2
done

exit ${XCCDF_RESULT_PASS}
