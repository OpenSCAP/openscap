#!/bin/bash
lines=${XCCDF_VALUE_LINES}

for i in `seq -w 00000001 0000${lines}`; do
    echo "${i}0"
done

for i in `seq -w 10000001 1000${lines}`; do
    echo "${i}0" >&2
done

exit ${XCCDF_RESULT_PASS}
