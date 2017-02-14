#!/bin/bash
lines=${XCCDF_VALUE_LINES}

# padding numbers so one line is exactly 10 bytes - 9 numbers and newline
# this way last line prints size of the output
for i in `seq -w 0000001 ${lines}`; do
    echo "0${i}0"
done

for i in `seq -w 0000001 ${lines}`; do
    echo "1${i}0" >&2
done

exit ${XCCDF_RESULT_PASS}
